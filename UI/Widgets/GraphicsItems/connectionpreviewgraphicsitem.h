#pragma once
#pragma once
#include <QGraphicsLineItem>

namespace Pipeline
{
    namespace UI
    {
        class ConnectionPreviewGraphicsItem : public QGraphicsLineItem
        {
            public:
                ConnectionPreviewGraphicsItem(QGraphicsItem* parent = nullptr);
                void setStart(const QPointF &start);
                void setEnd(const QPointF& end);
                void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
        };
    }
}
