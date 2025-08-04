#include "nodegraphmodel.h"
#include <QDebug>

namespace Pipeline
{
    namespace UI
    {
        //TODOJ Şu anda port lar tutulmuyor bir sorun var mapToSource line 73 e girmiyor hiç bak buna bir
        NodeGraphModel::NodeGraphModel(QObject *parent)
            : QAbstractProxyModel(parent)
        {
        }

        QModelIndex NodeGraphModel::index(int row, int column, const QModelIndex &parent) const
        {
            // burada columa gör
            if (row >= rowCount(parent))
            {
                return QModelIndex();
            }

            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return QModelIndex();
            }

            auto parentSource = mapToSource(parent);

            if (parent.isValid())
            {
                parentSource = parentSource.siblingAtColumn(parent.column());
            }

            auto sourceIndex = sourceModel->index(row, column, parentSource);
            return createIndex(row, column, sourceIndex.internalPointer());
        }

        QModelIndex NodeGraphModel::parent(const QModelIndex &child) const
        {
            auto sourceIndex = mapToSource(child);
            return mapFromSource(sourceIndex.parent());
        }

        QModelIndex NodeGraphModel:: mapToSource(const QModelIndex &proxyIndex) const
        {
            if (!proxyIndex.isValid())
            {
                return m_flowIndex;
            }

            if (!graphModel())
            {
                return QModelIndex();
            }

            auto *interface = static_cast<ModelItemInterface*>(proxyIndex.internalPointer());

            if (interface->getType() == DataType::Port)
            {
                auto *port = dynamic_cast<Pipeline::UI::MPort*>(interface);
                Core::Node* ownerNode = port->getOwnerNode();
                bool has;
                bool isIn;
                ownerNode->findPortIndex(port, has, isIn);

                if (!has)
                {
                    return QModelIndex();
                }

                size_t nodeIndex = ownerNode->parent()->indexOf(ownerNode, has);
                QModelIndex parentSource = sourceModel()->index(static_cast<int>(nodeIndex), isIn ? ColumnNames::InPortColumn : ColumnNames::OutPortColumn);
                return sourceModel()->index(proxyIndex.row(), proxyIndex.column(), parentSource);
            }
            else
            {
                return sourceModel()->index(proxyIndex.row(), proxyIndex.column(), m_flowIndex);
            }
        }

        QModelIndex NodeGraphModel::mapFromSource(const QModelIndex &sourceIndex) const
        {
            if (sourceIndex == m_flowIndex)
            {
                return QModelIndex();
            }

            switch (sourceIndex.data(Roles::Type).toInt())
            {
                case DataType::Node:
                case DataType::Port:
                    {
                        return createIndex(sourceIndex.row(), 0, sourceIndex.internalPointer());
                    }

                default:
                    return QModelIndex();
            }
        }

        int NodeGraphModel::columnCount(const QModelIndex &parent) const
        {
            if (parent.isValid())
            {
                return 3;
            }
            else
            {
                return 1;
            }
        }

        int NodeGraphModel::rowCount(const QModelIndex &parent) const
        {
            if (!graphModel())
            {
                return 0;
            }

            QModelIndex p = m_flowIndex;

            if (parent.isValid())
            {
                p = mapToSource(parent);
            }

            return sourceModel()->rowCount(p);
        }

        void NodeGraphModel::setSourceModel(QAbstractItemModel *model)
        {
            auto *oldModel = sourceModel();

            if (oldModel)
            {
                disconnect(oldModel, &QAbstractItemModel::dataChanged, this, &NodeGraphModel::onDataChanged);
                disconnect(oldModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &NodeGraphModel::onRowsAboutToBeRemoved);
                disconnect(oldModel, &QAbstractItemModel::rowsInserted, this, &NodeGraphModel::onRowsInserted);
                disconnect(oldModel, &QAbstractItemModel::rowsRemoved, this, &NodeGraphModel::onRowsRemoved);
                disconnect(oldModel, &QAbstractItemModel::modelAboutToBeReset, this, &NodeGraphModel::onModelAboutToBeReset);
                disconnect(oldModel, &QAbstractItemModel::modelReset, this, &NodeGraphModel::onModelReset);
            }

            QAbstractProxyModel::setSourceModel(model);
            auto *sourceModel = this->sourceModel();

            if (sourceModel)
            {
                connect(sourceModel, &QAbstractItemModel::dataChanged, this, &NodeGraphModel::onDataChanged);
                connect(sourceModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &NodeGraphModel::onRowsAboutToBeRemoved);
                connect(sourceModel, &QAbstractItemModel::rowsInserted, this, &NodeGraphModel::onRowsInserted);
                connect(sourceModel, &QAbstractItemModel::rowsRemoved, this, &NodeGraphModel::onRowsRemoved);
                connect(sourceModel, &QAbstractItemModel::modelAboutToBeReset, this, &NodeGraphModel::onModelAboutToBeReset);
                connect(sourceModel, &QAbstractItemModel::modelReset, this, &NodeGraphModel::onModelReset);
            }
        }

        NodeGraphTreeModel* NodeGraphModel::graphModel() const
        {
            return qobject_cast<NodeGraphTreeModel*>(sourceModel());
        }

        void NodeGraphModel::setFlowIndex(const QModelIndex &flowIndex)
        {
            //
            // TODOJ bu parent index, bunun icindeki butun nodelari ve connection cikarip index olarak kullanacagiz
            this->beginResetModel();
            this->m_flowIndex = flowIndex;
            this->endResetModel();
            emit flowIndexChanged();
        }

        bool NodeGraphModel::addConnection(const QModelIndex inPortIndex, const QModelIndex outPortIndex)
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return false;
            }

            return graphModel()->addConnection(mapToSource(inPortIndex), mapToSource(outPortIndex));
        }

        void NodeGraphModel::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int>& roles)
        {
            emit this->dataChanged(this->mapFromSource(topLeft), this->mapFromSource(bottomRight), roles);
        }

        void NodeGraphModel::onRowsInserted(const QModelIndex &parent, int first, int last)
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return;
            }

            switch (sourceModel->data(parent, Roles::Type).toInt())
            {
                case DataType::Node:
                    {
                        this->beginInsertRows(QModelIndex(), first, last);
                        this->endInsertRows();
                        break;
                    }

                    // case DataType::Port:
                    //     {
                    //         // find start connection index of related port
                    //         int startConnectionIndex = findStartConnectionIndexPort(parent);
                    //         int start = sourceModel->rowCount(m_flowIndex) + startConnectionIndex + first;
                    //         int end = sourceModel->rowCount(m_flowIndex) + startConnectionIndex + last;
                    //         this->beginInsertRows(QModelIndex(), start, end);
                    //         this->endInsertRows();
                    //         break;
                    //     }
            }
        }

        void NodeGraphModel::onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return;
            }
        }

        void NodeGraphModel::onRowsRemoved(const QModelIndex & parent, int first, int last)
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return;
            }
        }
        void NodeGraphModel::onModelAboutToBeReset()
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return;
            }

            this->beginResetModel();
        }
        void NodeGraphModel::onModelReset()
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return;
            }

            this->endResetModel();
        }
    }
}
