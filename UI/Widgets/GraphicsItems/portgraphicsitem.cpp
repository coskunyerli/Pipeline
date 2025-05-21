#include "portgraphicsitem.h"
#include <QPen>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include "connectiongraphicsitem.h"
#include <QGraphicsScene>
namespace Pipeline
{
    namespace UI
    {
        PortGraphicsItem::PortGraphicsItem(QGraphicsItem *parent)
            : QGraphicsEllipseItem(0, 0, 20, 20, parent)
            , m_clickedPos(-1, -1)
            , m_hasMove(false)
            , m_dummyConnectionItem(nullptr)
        {
            setPen(QPen(QColor("blue")));
            setCursor(Qt::ClosedHandCursor);
        }

        void PortGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
        {
            if (!m_hasMove)
            {
                m_hasMove = true;
                m_clickedPos = event->scenePos();
            }
        }

        void PortGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
        {
            if (m_hasMove)
            {
                if (!m_dummyConnectionItem)
                {
                    m_dummyConnectionItem = new ConnectionGraphicsItem();
                    scene()->addItem(m_dummyConnectionItem);
                    m_dummyConnectionItem->setStart(m_clickedPos);
                }

                m_dummyConnectionItem->setEnd(event->scenePos());
            }
        }

        void PortGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
        {
            if (m_hasMove)
            {
                m_clickedPos = QPointF(-1, -1);
                m_hasMove = false;
                scene()->removeItem(m_dummyConnectionItem);
                m_dummyConnectionItem = nullptr;
            }
        }
    }
}
