#include "connectiongraphviewmodel.h"
#include "constants.h"

namespace Pipeline
{
    namespace UI
    {
        ConnectionGraphViewModel::ConnectionGraphViewModel(QObject *parent)
            : QIdentityProxyModel(parent)
            , m_uiManager(nullptr)
        {
        }

        QVariant ConnectionGraphViewModel::data(const QModelIndex &proxyIndex, int role) const
        {
            switch (role)
            {
                case ConnectionRoles::ConnectionInX :
                    {
                        if (!m_uiManager)
                        {
                            return {};
                        }

                        //auto portCenter = m_uiManager->portSize() / 2;
                        auto portIndex = proxyIndex.data(Roles::InPortIndex).toModelIndex();
                        auto relatedNodeIndex = portIndex.data(Roles::RelatedNode).toModelIndex();
                        auto nodeX = relatedNodeIndex.data(Roles::PosX).toInt();
                        //auto portPos = m_uiManager->portPosition(portIndex);
                        return nodeX; // + portPos.x() + portCenter.width();
                    }

                case ConnectionRoles::ConnectionInY :
                    {
                        if (!m_uiManager)
                        {
                            return {};
                        }

                        auto portCenter = m_uiManager->portSize() / 2;
                        auto portIndex = proxyIndex.data(Roles::InPortIndex).toModelIndex();
                        auto relatedNodeIndex = portIndex.data(Roles::RelatedNode).toModelIndex();
                        auto nodeY = relatedNodeIndex.data(Roles::PosY).toInt();
                        auto portPos = m_uiManager->portPosition(portIndex);
                        return nodeY + portPos.y() + portCenter.height();
                    }

                case ConnectionRoles::ConnectionOutX :
                    {
                        if (!m_uiManager)
                        {
                            return {};
                        }

                        auto portCenter = m_uiManager->portSize() / 2;
                        auto portIndex = proxyIndex.data(Roles::OutPortIndex).toModelIndex();
                        auto relatedNodeIndex = portIndex.data(Roles::RelatedNode).toModelIndex();
                        auto nodeX = relatedNodeIndex.data(Roles::PosX).toInt();
                        auto portPos = m_uiManager->portPosition(portIndex);
                        return nodeX + portPos.x() + portCenter.width();
                    }

                case ConnectionRoles::ConnectionOutY :
                    {
                        if (!m_uiManager)
                        {
                            return {};
                        }

                        auto portCenter = m_uiManager->portSize() / 2;
                        auto portIndex = proxyIndex.data(Roles::OutPortIndex).toModelIndex();
                        auto relatedNodeIndex = portIndex.data(Roles::RelatedNode).toModelIndex();
                        auto nodeY = relatedNodeIndex.data(Roles::PosY).toInt();
                        auto portPos = m_uiManager->portPosition(portIndex);
                        return nodeY + portPos.y() + portCenter.height();
                    }

                default:
                    return QIdentityProxyModel::data(proxyIndex, role);
            }
        }

        void ConnectionGraphViewModel::setSourceModel(QAbstractItemModel *model)
        {
            auto *oldModel = sourceModel();

            if (oldModel)
            {
                disconnect(oldModel, &QAbstractItemModel::dataChanged, this, &ConnectionGraphViewModel::onDataChanged);
            }

            QIdentityProxyModel::setSourceModel(model);
            auto *sourceModel = this->sourceModel();

            if (sourceModel)
            {
                connect(sourceModel, &QAbstractItemModel::dataChanged, this, &ConnectionGraphViewModel::onDataChanged);
            }
        }

        void ConnectionGraphViewModel::setUIManager(NodeIUManager *manager)
        {
            if (manager == m_uiManager)
            {
                return;
            }

            m_uiManager = manager;
            emit uiManagerChanged();
        }

        QHash<int, QByteArray> ConnectionGraphViewModel::roleNames() const
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return {};
            }

            auto roles = sourceModel->roleNames();
            roles[ConnectionRoles::ConnectionInX] = "connectionInX";
            roles[ConnectionRoles::ConnectionInY] = "connectionInY";
            roles[ConnectionRoles::ConnectionOutX] = "connectionOutX";
            roles[ConnectionRoles::ConnectionOutY] = "connectionOutY";
            return roles;
        }

        void ConnectionGraphViewModel::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int>& roles)
        {
            if (
                roles.contains(ConnectionGraphRoles::InRelatedNodePosX) ||
                roles.contains(ConnectionGraphRoles::InRelatedNodePosY) ||
                roles.contains(ConnectionGraphRoles::OutRelatedNodePosX) ||
                roles.contains(ConnectionGraphRoles::OutRelatedNodePosY))
            {
                emit dataChanged(topLeft, bottomRight, {ConnectionRoles::ConnectionInX, ConnectionRoles::ConnectionInY, ConnectionRoles::ConnectionOutX, ConnectionRoles::ConnectionOutY});
            }
        }


    }
}

