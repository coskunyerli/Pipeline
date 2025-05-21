#include "connectiongraphicsitem.h"
#include <QLineF>
#include <QPen>
namespace Pipeline
{
    namespace UI
    {
        ConnectionGraphicsItem::ConnectionGraphicsItem(QGraphicsItem *parent)
            : QGraphicsLineItem(parent)
        {
            setPen(QPen(QColor("blue")));
        }

        void ConnectionGraphicsItem::setStart(const QPointF &start)
        {
            setLine(QLineF(start, line().p2()));
        }

        void ConnectionGraphicsItem::setEnd(const QPointF &end)
        {
            setLine(QLineF(line().p1(), end));
        }
    }
}
