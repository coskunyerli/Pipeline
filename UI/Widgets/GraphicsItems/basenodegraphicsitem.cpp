#include "basenodegraphicsitem.h"
#include <QPen>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>

namespace Pipeline
{
    namespace UI
    {
        BaseNodeGraphicsItem::BaseNodeGraphicsItem(QGraphicsItem *parent)
            : QGraphicsRectItem(0, 0, 100, 50, parent)
            , m_clickedPos(-1, -1)
            , m_hasMove(false)
        {
            this->setPen(QPen(QColor("red")));
            setCursor(Qt::PointingHandCursor);
        }

        void BaseNodeGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
        {
            if (!m_hasMove)
            {
                m_hasMove = true;
                m_clickedPos = event->scenePos();
            }
        }

        void BaseNodeGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
        {
            if (m_hasMove)
            {
                auto scenePos = event->scenePos();
                auto diff = scenePos - m_clickedPos;
                this->moveBy(diff.x(), diff.y());
                m_clickedPos = scenePos;
            }
        }

        void BaseNodeGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
        {
            if (m_hasMove)
            {
                m_clickedPos = QPointF(-1, -1);
                m_hasMove = false;
            }
        }
    }
}

