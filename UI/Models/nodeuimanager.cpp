#include "nodeuimanager.h"
#include <QModelIndex>
#include "constants.h"

namespace Pipeline
{
    namespace UI
    {
        NodeIUManager::NodeIUManager(QObject *parent)
            : QObject(parent)
            , m_leftMargin(16)
            , m_topMargin(4)
            , m_rightMargin(4)
            , m_bottomMargin(4)
        {
        }

        QSize NodeIUManager::nodeSize(const QModelIndex &nodeIndex) const
        {
            int inPortCount = nodeIndex.data(Roles::InPortCount).toInt();
            int outPortCount = nodeIndex.data(Roles::InPortCount).toInt();
            int maxCount = (std::max(inPortCount, outPortCount) - 1);
            int height = maxCount * this->portSpacing() + maxCount * this->portSize().height() + this->topMargin() + this->bottomMargin();
            return QSize(m_nodeSize.width(), std::max(this->nodeSize().height(), height));
        }

        QPoint NodeIUManager::portPosition(const QModelIndex& portIndex)
        {
            if (!portIndex.isValid())
            {
                return QPoint();
            }

            auto relatedNodeIndex = portIndex.data(Roles::RelatedNode).toModelIndex();
            auto nodeSize = this->nodeSize(relatedNodeIndex);
            bool isIn = portIndex.data(Roles::PortIsIn).toBool();
            auto nodeAreaHeight = nodeSize.height() - m_topMargin - m_bottomMargin;
            QSize portSize = m_portSize;
            int portTop = m_portSpacing * portIndex.row() + portSize.height() * portIndex.row();
            int portBottom = portTop + portSize.height();
            int centerMargin = std::max(0, (nodeAreaHeight - portBottom) / 2);

            if (isIn)
            {
                return QPoint(m_leftMargin, m_topMargin + portTop + centerMargin);
            }
            else
            {
                return QPoint(m_nodeSize.width() - m_rightMargin - portSize.width(), m_topMargin + portTop + centerMargin);
            }
        }

        void NodeIUManager::setNodeSize(QSize size)
        {
            if (m_nodeSize == size)
            {
                return;
            }

            m_nodeSize = size;
            emit nodeSizeChanged();
        }

        void NodeIUManager::setPortSize(QSize size)
        {
            if (m_portSize == size)
            {
                return;
            }

            m_portSize = size;
            emit portSizeChanged();
        }

        void NodeIUManager::setPortSpacing(int spacing)
        {
            if (m_portSpacing == spacing)
            {
                return;
            }

            m_portSpacing = spacing;
            emit portSpacingChanged();
        }

        void NodeIUManager::setPortStartPosition(QPoint position)
        {
            if (m_portStartPosition == position)
            {
                return;
            }

            m_portStartPosition = position;
            emit portStartPositionChanged();
        }
    }
}
