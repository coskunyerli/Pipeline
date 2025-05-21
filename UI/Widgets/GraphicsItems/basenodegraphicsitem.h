#pragma once
#include <QGraphicsRectItem>

namespace Pipeline
{
    namespace UI
    {
        class BaseNodeGraphicsItem : public QGraphicsRectItem
        {
            public:
                BaseNodeGraphicsItem(QGraphicsItem *parent = nullptr);
            protected:
                void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
                void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
                void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
            private:
                QPointF m_clickedPos;
                bool m_hasMove;
        };
    }
}
