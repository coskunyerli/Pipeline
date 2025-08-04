#include "nodegraphviewmodel.h"
#include "constants.h"

namespace Pipeline
{
    namespace UI
    {
        NodeGraphViewModel::NodeGraphViewModel(QObject *parent)
            : QIdentityProxyModel(parent)
            , m_uiManager(nullptr)
        {
        }

        bool NodeGraphViewModel::setData(const QModelIndex &index, const QVariant &value, int role)
        {
            if (role == Roles::PosX || role == Roles::PosY)
            {
                bool res = QIdentityProxyModel::setData(index, value, role);

                if (res && m_uiManager)
                {
                    emit m_uiManager->nodePositionChanged(index);
                }

                return res;
            }
            else
            {
                return QIdentityProxyModel::setData(index, value, role);
            }
        }

        QVariant NodeGraphViewModel::data(const QModelIndex &proxyIndex, int role) const
        {
            if (role == Roles::DataIndex)
            {
                return proxyIndex;
            }

            switch (role)
            {
                case NodeModelRoles::NodeWidth :
                    {
                        if (!m_uiManager)
                        {
                            return {};
                        }

                        return m_uiManager->nodeSize().width();
                    }

                case NodeModelRoles::NodeHeight :
                    {
                        if (!m_uiManager)
                        {
                            return {};
                        }

                        return m_uiManager->nodeSize(proxyIndex).height();
                    }

                case NodeModelRoles::PortModel :
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

                default:
                    return QIdentityProxyModel::data(proxyIndex, role);
            }
        }

        void NodeGraphViewModel::setUIManager(NodeIUManager *manager)
        {
            if (manager == m_uiManager)
            {
                return;
            }

            m_uiManager = manager;
            emit uiManagerChanged();
        }

        QHash<int, QByteArray> NodeGraphViewModel::roleNames() const
        {
            auto *sourceModel = this->sourceModel();

            if (!sourceModel)
            {
                return {};
            }

            auto roles = sourceModel->roleNames();
            roles[NodeModelRoles::NodeWidth] = "nodeWidth";
            roles[NodeModelRoles::NodeHeight] = "nodeHeight";
            roles[NodeModelRoles::PortModel] = "portModel";
            return roles;
        }
    }
}
