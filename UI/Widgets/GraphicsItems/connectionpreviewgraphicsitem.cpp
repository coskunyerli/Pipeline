#include "connectionpreviewgraphicsitem.h"
#include <QLineF>
#include <QPen>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
namespace Pipeline
{
    namespace UI
    {
        ConnectionPreviewGraphicsItem::ConnectionPreviewGraphicsItem(QGraphicsItem *parent)
            : QGraphicsLineItem(parent)
        {
            setPen(QPen(QColor("blue")));
        }

        void ConnectionPreviewGraphicsItem::setStart(const QPointF &start)
        {
            setLine(QLineF(start, line().p2()));
        }

        void ConnectionPreviewGraphicsItem::setEnd(const QPointF &end)
        {
            setLine(QLineF(line().p1(), end));
        }

        void ConnectionPreviewGraphicsItem::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
        {
            qDebug() << "p1";
        }
    }
}
