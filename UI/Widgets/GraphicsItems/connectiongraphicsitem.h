#pragma once
#include <QGraphicsLineItem>

namespace Pipeline
{
    namespace UI
    {
        class ConnectionGraphicsItem : public QGraphicsLineItem
        {
            public:
                ConnectionGraphicsItem(QGraphicsItem* parent = nullptr);
                void setStart(const QPointF &start);
                void setEnd(const QPointF& end);
        };
    }
}
