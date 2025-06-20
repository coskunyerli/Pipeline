#include "basenodegraphicsitem.h"
#include <QPen>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>
#include <QDebug>

namespace Pipeline
{
    namespace UI
    {
        BaseNodeGraphicsItem::BaseNodeGraphicsItem(const QModelIndex &index, QGraphicsItem *parent)
            : QGraphicsRectItem(0, 0, 100, 50, parent)
            , m_clickedPos(-1, -1)
            , m_hasMove(false)
            , m_index(index)
        {
            this->setPen(QPen(QColor("red")));
            setCursor(Qt::PointingHandCursor);
            setAcceptDrops(true);
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

        void BaseNodeGraphicsItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
        {
            this->setPen(QPen(QColor("red")));
        }

        void BaseNodeGraphicsItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
        {
            setPen(QPen(QColor("yellow")));
        }

        void BaseNodeGraphicsItem::dropEvent(QGraphicsSceneDragDropEvent *event)
        {
            auto mimeData = event->mimeData();
            QModelIndex index = mimeData->property("nodeIndex").toModelIndex();
            size_t portIndex = mimeData->property("nodePort").toULongLong();
            qDebug() << "girdi" << portIndex;
            this->setPen(QPen(QColor("red")));
        }
    }
}

