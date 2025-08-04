#include "connectiongraphmodel.h"
#include "constants.h"
#include <QSet>
namespace
{
    int findStartConnectionIndexPort(const QModelIndex &sourcePortIndex)
    {
        //auto *model = sourcePortIndex.model();
        auto sourceNodeIndex = sourcePortIndex.parent();
        int totalConnectionCount = 0;

        for (int i = 0; i < sourceNodeIndex.row(); i++)
        {
            totalConnectionCount += sourceNodeIndex.siblingAtRow(i).data(Pipeline::UI::Roles::OutConnectionCount).toInt();
        }

        for (int index = 0; index < sourcePortIndex.row(); index++)
        {
            totalConnectionCount += sourcePortIndex.siblingAtRow(index).data(Pipeline::UI::Roles::ConnectionCount).toInt();
        }

        return totalConnectionCount;
    }

    QPair<QPoint, QPoint> findMinMaxModelIndex(const QSet<QModelIndex>& indexSet)
    {
        int minX = INT_MAX, minY = INT_MAX;
        int maxX = -1, maxY = -1;

        for (auto &index : indexSet)
        {
            if (index.row() < minX)
                minX = index.row();

            if (index.column() < minY)
                minY = index.column();

            if (maxX < index.row())
                maxX = index.row();

            if (maxY < index.column())
                maxY = index.column();
        }

        return QPair<QPoint, QPoint>(QPoint(minX, minY), QPoint(maxX, maxY));
    }
}

namespace Pipeline
{
    namespace UI
    {
        ConnectionGraphModel::ConnectionGraphModel(QObject *parent)
            : QAbstractProxyModel(parent)
        {
        }

        QModelIndex ConnectionGraphModel::index(int row, int column, const QModelIndex &parent) const
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

            // TODOJ Burada connection index invalid oluyor ondan sorun var bak buraya
            const QModelIndex tempProxyIndex = createIndex(row, column);
            auto sourceIndex = this->mapToSource(tempProxyIndex);
            auto index = mapFromSource(sourceIndex);
            return index;
        }

        QModelIndex ConnectionGraphModel::parent(const QModelIndex &child) const
        {
            return QModelIndex();
        }

        QModelIndex ConnectionGraphModel::mapToSource(const QModelIndex &proxyIndex) const
        {
            int totalConnectionIndex = proxyIndex.row();

            for (int nodeIndex = 0; nodeIndex < sourceModel()->rowCount(m_flowIndex); nodeIndex++)
            {
                auto portContainerIndex = sourceModel()->index(nodeIndex, ColumnNames::OutPortColumn, m_flowIndex);

                for (int portIndex = 0; portIndex < sourceModel()->rowCount(portContainerIndex); portIndex++)
                {
                    auto portModelIndex = sourceModel()->index(portIndex, 0, portContainerIndex);
                    int connectionCount = portModelIndex.data(Roles::ConnectionCount).toInt();

                    if (totalConnectionIndex - connectionCount < 0)
                    {
                        return sourceModel()->index(totalConnectionIndex, 0, portModelIndex);
                    }

                    totalConnectionIndex -= connectionCount;
                }
            }

            return QModelIndex();
        }

        QModelIndex ConnectionGraphModel::mapFromSource(const QModelIndex &sourceIndex) const
        {
            switch (sourceIndex.data(Roles::Type).toInt())
            {
                case DataType::Connection:
                    {
                        // TODO burada node kadar gitmem gerekiyor daha sonra node üzerinden sayım yapacağım
                        if (sourceIndex.parent().data(Roles::Type).toInt() != DataType::Port)
                        {
                            return QModelIndex();
                        }

                        int connectionIndex = findStartConnectionIndexPort(sourceIndex.parent()); // this is connection index of port
                        connectionIndex = connectionIndex + sourceIndex.row();
                        return createIndex(connectionIndex, 0);
                    }

                default:
                    return QModelIndex();
            }
        }

        int ConnectionGraphModel::columnCount(const QModelIndex &parent) const
        {
            return 1;
        }

        int ConnectionGraphModel::rowCount(const QModelIndex &parent) const
        {
            auto count = graphModel()->data(this->m_flowIndex, Roles::ChildConnectionCount).toLongLong();
            return count;
        }

        void ConnectionGraphModel::setSourceModel(QAbstractItemModel *model)
        {
            auto *oldModel = sourceModel();

            if (oldModel)
            {
                disconnect(oldModel, &QAbstractItemModel::dataChanged, this, &ConnectionGraphModel::onDataChanged);
                disconnect(oldModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ConnectionGraphModel::onRowsAboutToBeRemoved);
                disconnect(oldModel, &QAbstractItemModel::rowsAboutToBeInserted, this, &ConnectionGraphModel::onRowsAboutToBeInserted);
                disconnect(oldModel, &QAbstractItemModel::rowsInserted, this, &ConnectionGraphModel::onRowsInserted);
                disconnect(oldModel, &QAbstractItemModel::rowsRemoved, this, &ConnectionGraphModel::onRowsRemoved);
                disconnect(oldModel, &QAbstractItemModel::modelAboutToBeReset, this, &ConnectionGraphModel::onModelAboutToBeReset);
                disconnect(oldModel, &QAbstractItemModel::modelReset, this, &ConnectionGraphModel::onModelReset);
            }

            QAbstractProxyModel::setSourceModel(model);
            auto *sourceModel = this->sourceModel();

            if (sourceModel)
            {
                connect(sourceModel, &QAbstractItemModel::dataChanged, this, &ConnectionGraphModel::onDataChanged);
                connect(sourceModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ConnectionGraphModel::onRowsAboutToBeRemoved);
                connect(sourceModel, &QAbstractItemModel::rowsAboutToBeInserted, this, &ConnectionGraphModel::onRowsAboutToBeInserted);
                connect(sourceModel, &QAbstractItemModel::rowsInserted, this, &ConnectionGraphModel::onRowsInserted);
                connect(sourceModel, &QAbstractItemModel::rowsRemoved, this, &ConnectionGraphModel::onRowsRemoved);
                connect(sourceModel, &QAbstractItemModel::modelAboutToBeReset, this, &ConnectionGraphModel::onModelAboutToBeReset);
                connect(sourceModel, &QAbstractItemModel::modelReset, this, &ConnectionGraphModel::onModelReset);
            }
        }

        NodeGraphTreeModel* ConnectionGraphModel::graphModel() const
        {
            return qobject_cast<NodeGraphTreeModel*>(sourceModel());
        }

        void ConnectionGraphModel::setFlowIndex(const QModelIndex &flowIndex)
        {
            this->beginResetModel();
            this->m_flowIndex = flowIndex;
            this->endResetModel();
        }

        void ConnectionGraphModel::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int>& roles)
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return;
            }

            QVector<int> newRoles = roles;
            QSet<QModelIndex> connectionList;

            // if node position is changed trigger related connection as new role name
            for (int i = topLeft.row(); i <= bottomRight.row(); i++)
            {
                for (int j = topLeft.column(); j <= bottomRight.column(); j++)
                {
                    auto sourceIndex = sourceModel->index(i, j, topLeft.parent());

                    if ((roles.contains(Roles::PosX) || roles.contains(Roles::PosY)) && sourceIndex.data(Roles::Type) == DataType::Node)
                    {
                        auto inPortCount = sourceIndex.data(Roles::InPortCount).toInt();
                        auto outPortCount = sourceIndex.data(Roles::OutPortCount).toInt();

                        for (int i = 0; i < outPortCount; i++)
                        {
                            auto outPortIndex = sourceIndex.data(Roles::NodePortIndex + i).toModelIndex();
                            auto connectionCount = outPortIndex.data(Roles::ConnectionCount).toInt();

                            for (int c = 0; c < connectionCount; c++)
                            {
                                auto connectionIndex = outPortIndex.data(Roles::PortConnectionIndex + c).toModelIndex();
                                connectionList.insert(mapFromSource(connectionIndex));
                            }

                            // outrelatednode
                        }

                        for (int i = outPortCount; i < outPortCount + inPortCount; i++)
                        {
                            auto inPortIndex = sourceIndex.data(Roles::NodePortIndex + i).toModelIndex();
                            auto connectionCount = inPortIndex.data(Roles::ConnectionCount).toInt();

                            for (int c = 0; c < connectionCount; c++)
                            {
                                auto connectionIndex = inPortIndex.data(Roles::PortConnectionIndex + c).toModelIndex();
                                connectionList.insert(mapFromSource(connectionIndex));
                            }

                            //inrelatednode
                        }
                    }
                }
            }

            if (!connectionList.empty())
            {
                // find min and max index of connectionList
                auto minMax = findMinMaxModelIndex(connectionList);
                auto minConnectionIndex = this->index(minMax.first.x(), minMax.first.y());
                auto maxConnectionIndex = this->index(minMax.second.x(), minMax.second.y());
                emit this->dataChanged(minConnectionIndex, maxConnectionIndex,
                {
                    ConnectionGraphRoles::OutRelatedNodePosX,
                    ConnectionGraphRoles::OutRelatedNodePosY,
                    ConnectionGraphRoles::InRelatedNodePosX,
                    ConnectionGraphRoles::InRelatedNodePosY
                });
            }

            auto proxyTopLeft = this->mapFromSource(topLeft);
            auto proxyBottomRight = this->mapFromSource(bottomRight);
            auto min = this->index(std::min(proxyTopLeft.row(), proxyBottomRight.row()), std::min(proxyTopLeft.column(), proxyBottomRight.column()));
            auto max = this->index(std::max(proxyTopLeft.row(), proxyBottomRight.row()), std::max(proxyTopLeft.column(), proxyBottomRight.column()));
            emit this->dataChanged(min, max, roles);
        }

        void ConnectionGraphModel::onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return;
            }
        }

        void ConnectionGraphModel::onRowsAboutToBeInserted(const QModelIndex &parent, int first, int last)
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return;
            }

            switch (sourceModel->data(parent, Roles::Type).toInt())
            {
                case DataType::Port:
                    {
                        // find start connection index of related port
                        break;
                    }
            }
        }

        void ConnectionGraphModel::onRowsInserted(const QModelIndex &parent, int first, int last)
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return;
            }

            switch (sourceModel->data(parent, Roles::Type).toInt())
            {
                case DataType::Port:
                    {
                        int startConnectionIndex = findStartConnectionIndexPort(parent);
                        int start = startConnectionIndex + first;
                        int end = startConnectionIndex + last;
                        this->beginInsertRows(QModelIndex(), start, end);
                        // find start connection index of related port
                        this->endInsertRows();
                        break;
                    }
            }
        }

        void ConnectionGraphModel::onRowsRemoved(const QModelIndex &parent, int first, int last)
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return;
            }
        }

        void ConnectionGraphModel::onModelAboutToBeReset()
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return;
            }
        }

        void ConnectionGraphModel::onModelReset()
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return;
            }
        }
    }
}
