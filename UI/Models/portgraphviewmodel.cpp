#include "portgraphviewmodel.h"
#include "constants.h"
namespace Pipeline
{
    namespace UI
    {
        PortGraphViewModel::PortGraphViewModel(QObject *parent)
            : QAbstractProxyModel(parent)
            , m_isInPort(false)
        {
        }

        QModelIndex PortGraphViewModel::index(int row, int column, const QModelIndex &parent) const
        {
            if (row >= rowCount())
            {
                return QModelIndex();
            }

            if (column >= columnCount())
            {
                return QModelIndex();
            }

            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return QModelIndex();
            }

            int parentColumn = isInPort() ? ColumnNames::InPortColumn : ColumnNames::OutPortColumn;
            // TODOJ Burada connection index invalid oluyor ondan sorun var bak buraya
            auto sourceIndex = sourceModel->index(row, 0, m_nodeIndex.sibling(m_nodeIndex.row(), parentColumn));
            auto index = createIndex(row, 0, sourceIndex.internalPointer());
            return index;
        }

        QModelIndex PortGraphViewModel::parent(const QModelIndex &child) const
        {
            return QModelIndex();
        }

        QVariant PortGraphViewModel::data(const QModelIndex &proxyIndex, int role) const
        {
            if (!sourceModel())
            {
                return {};
            }

            switch (role)
            {
                case PortModelRoles::PortX:
                    {
                        if (!m_uiManager)
                        {
                            return {};
                        }

                        return m_uiManager->portPosition(proxyIndex).x();
                    }

                case PortModelRoles::PortY:
                    {
                        if (!m_uiManager)
                        {
                            return {};
                        }

                        return m_uiManager->portPosition(proxyIndex).y();
                    }

                case PortModelRoles::PortWidth:
                    {
                        if (!m_uiManager)
                        {
                            return {};
                        }

                        return m_uiManager->portSize().width();
                    }

                case PortModelRoles::PortHeight:
                    {
                        if (!m_uiManager)
                        {
                            return {};
                        }

                        return m_uiManager->portSize().height();
                    }

                default:
                    auto d = sourceModel()->data(mapToSource(proxyIndex), role);
                    return d;
            }
        }

        QModelIndex PortGraphViewModel::mapToSource(const QModelIndex &proxyIndex) const
        {
            if (!proxyIndex.isValid())
            {
                return m_nodeIndex;
            }

            if (!sourceModel())
            {
                return QModelIndex();
            }

            if (!m_nodeIndex.isValid())
            {
                return QModelIndex();
            }

            int role = isInPort() ? Roles::InPortCount : Roles::OutPortCount;
            int sourceColumn = isInPort() ? ColumnNames::InPortColumn : ColumnNames::OutPortColumn;
            auto rowCount = m_nodeIndex.data(role).toInt();

            if (proxyIndex.row() >= 0 && proxyIndex.row() < rowCount)
            {
                // these are node sourse index;
                return sourceModel()->index(proxyIndex.row(), 0, m_nodeIndex.sibling(m_nodeIndex.row(), sourceColumn));
            }

            return QModelIndex();
        }

        QModelIndex PortGraphViewModel::mapFromSource(const QModelIndex &sourceIndex) const
        {
            if (!sourceIndex.isValid())
            {
                return QModelIndex();
            }

            return createIndex(sourceIndex.row(), 0, sourceIndex.internalPointer());
        }

        int PortGraphViewModel::columnCount(const QModelIndex &parent) const
        {
            return 1;
        }

        int PortGraphViewModel::rowCount(const QModelIndex &parent) const
        {
            int role = isInPort() ? Roles::InPortCount : Roles::OutPortCount;
            int count = m_nodeIndex.data(role).toInt();
            return count;
        }

        void PortGraphViewModel::setSourceModel(QAbstractItemModel *model)
        {
            auto *oldModel = sourceModel();

            if (oldModel)
            {
                disconnect(oldModel, &QAbstractItemModel::dataChanged, this, &PortGraphViewModel::onDataChanged);
                disconnect(oldModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &PortGraphViewModel::onRowsAboutToBeRemoved);
                disconnect(oldModel, &QAbstractItemModel::rowsAboutToBeInserted, this, &PortGraphViewModel::onRowsAboutToBeInserted);
                disconnect(oldModel, &QAbstractItemModel::rowsInserted, this, &PortGraphViewModel::onRowsInserted);
                disconnect(oldModel, &QAbstractItemModel::rowsRemoved, this, &PortGraphViewModel::onRowsRemoved);
                disconnect(oldModel, &QAbstractItemModel::modelAboutToBeReset, this, &PortGraphViewModel::onModelAboutToBeReset);
                disconnect(oldModel, &QAbstractItemModel::modelReset, this, &PortGraphViewModel::onModelReset);
            }

            QAbstractProxyModel::setSourceModel(model);
            auto *sourceModel = this->sourceModel();

            if (sourceModel)
            {
                connect(sourceModel, &QAbstractItemModel::dataChanged, this, &PortGraphViewModel::onDataChanged);
                connect(sourceModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &PortGraphViewModel::onRowsAboutToBeRemoved);
                connect(sourceModel, &QAbstractItemModel::rowsAboutToBeInserted, this, &PortGraphViewModel::onRowsAboutToBeInserted);
                connect(sourceModel, &QAbstractItemModel::rowsInserted, this, &PortGraphViewModel::onRowsInserted);
                connect(sourceModel, &QAbstractItemModel::rowsRemoved, this, &PortGraphViewModel::onRowsRemoved);
                connect(sourceModel, &QAbstractItemModel::modelAboutToBeReset, this, &PortGraphViewModel::onModelAboutToBeReset);
                connect(sourceModel, &QAbstractItemModel::modelReset, this, &PortGraphViewModel::onModelReset);
            }
        }

        QHash<int, QByteArray> PortGraphViewModel::roleNames() const
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return {};
            }

            auto roles = sourceModel->roleNames();
            roles[PortModelRoles::PortWidth] = "portWidth";
            roles[PortModelRoles::PortHeight] = "portHeight";
            roles[PortModelRoles::PortX] = "portX";
            roles[PortModelRoles::PortY] = "portY";
            return roles;
        }

        void PortGraphViewModel::setNodeIndex(const QModelIndex &nodeIndex)
        {
            this->beginResetModel();
            this->m_nodeIndex = nodeIndex;
            this->endResetModel();
            emit nodeIndexChanged();
        }

        void PortGraphViewModel::setIsInPort(bool res)
        {
            if (m_isInPort == res)
            {
                return;
            }

            m_isInPort = res;
            emit isInPortChanged();
        }

        void PortGraphViewModel::setUIManager(NodeIUManager *manager)
        {
            if (manager == m_uiManager)
            {
                return;
            }

            this->beginResetModel();
            m_uiManager = manager;
            this->endResetModel();
            emit uiManagerChanged();
        }

        void PortGraphViewModel::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int>& roles)
        {
            emit this->dataChanged(this->mapFromSource(topLeft), this->mapFromSource(bottomRight), roles);
        }

        void PortGraphViewModel::onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return;
            }
        }

        void PortGraphViewModel::onRowsAboutToBeInserted(const QModelIndex &parent, int first, int last)
        {
            // auto *sourceModel = this->sourceModel();
            // if (!sourceModel)
            // {
            //     return;
            // }
            // switch (sourceModel->data(parent, NodeGraphModel::Roles::Type).toInt())
            // {
            //     case DataType::Port:
            //         {
            //             // find start connection index of related port
            //             int startConnectionIndex = findStartConnectionIndexPort(parent);
            //             int start = startConnectionIndex + first;
            //             int end = startConnectionIndex + last;
            //             this->beginInsertRows(QModelIndex(), start, end);
            //             break;
            //         }
            // }
        }

        void PortGraphViewModel::onRowsInserted(const QModelIndex &parent, int first, int last)
        {
            // auto *sourceModel = this->sourceModel();
            // if (!sourceModel)
            // {
            //     return;
            // }
            // switch (sourceModel->data(parent, NodeGraphTreeModel::Roles::Type).toInt())
            // {
            //     case DataType::Port:
            //         {
            //             // find start connection index of related port
            //             this->endInsertRows();
            //             break;
            //         }
            // }
        }

        void PortGraphViewModel::onRowsRemoved(const QModelIndex &parent, int first, int last)
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return;
            }
        }

        void PortGraphViewModel::onModelAboutToBeReset()
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return;
            }
        }

        void PortGraphViewModel::onModelReset()
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return;
            }
        }
    }
}
