#include "portgraphicsitem.h"
#include <QPen>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include "connectiongraphicsitem.h"
#include "basenodegraphicsitem.h"
#include <QGraphicsScene>
#include <QDebug>
#include <QDrag>
#include <QMimeData>
namespace Pipeline
{
    namespace UI
    {
        PortGraphicsItem::PortGraphicsItem(size_t portIndex, QGraphicsItem *parent)
            : QGraphicsEllipseItem(0, 0, 20, 20, parent)
            , m_hasMove(false)
            , m_dummyConnectionItem(nullptr)
            , m_portIndex(portIndex)
        {
            setAcceptDrops(true);
            setPen(QPen(QColor("blue")));
            setCursor(Qt::ClosedHandCursor);
        }

        void PortGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
        {
            if (!m_hasMove)
            {
                QDrag *drag = new QDrag(scene());
                QMimeData *mimeData = new QMimeData();
                mimeData->setProperty("nodeIndex", this->getNode()->getIndex());
                mimeData->setProperty("nodePort", this->m_portIndex);
                drag->setMimeData(mimeData);
                drag->exec();
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
                    m_dummyConnectionItem->setStart(mapToScene(rect().center()));
                }

                m_dummyConnectionItem->setEnd(event->scenePos());
            }
        }

        void PortGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
        {
            if (m_hasMove)
            {
                m_hasMove = false;
                scene()->removeItem(m_dummyConnectionItem);
                m_dummyConnectionItem = nullptr;
            }
        }

        void PortGraphicsItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
        {
            QGraphicsEllipseItem::dragEnterEvent(event);
            event->ignore();
        }

        void PortGraphicsItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
        {
            QGraphicsEllipseItem::dragLeaveEvent(event);
            event->ignore();
        }

        void PortGraphicsItem::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
        {
            QGraphicsEllipseItem::dragMoveEvent(event);
            qDebug() << event->scenePos();
        }

        void PortGraphicsItem::dropEvent(QGraphicsSceneDragDropEvent *event)
        {
            auto mimeData = event->mimeData();
            QModelIndex index = mimeData->property("nodeIndex").toModelIndex();
            size_t portIndex = mimeData->property("nodePort").toULongLong();
            qDebug() << "girdi" << portIndex;

            if (index.isValid() && this->getNode() && index != this->getNode()->getIndex())
            {
                qDebug() << "girdi" << portIndex;
            }
        }

        BaseNodeGraphicsItem* PortGraphicsItem::getNode() const
        {
            return dynamic_cast<BaseNodeGraphicsItem*>(parentItem());
        }
    }
}
