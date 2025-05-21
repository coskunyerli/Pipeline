#pragma once
#include <QGraphicsEllipseItem>
namespace Pipeline
{
    namespace UI
    {
        class ConnectionGraphicsItem;
        class PortGraphicsItem : public QGraphicsEllipseItem
        {
            public:
                explicit PortGraphicsItem(QGraphicsItem *parent = nullptr);
                void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
                void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
                void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
            private:
                QPointF m_clickedPos;
                bool m_hasMove;
                ConnectionGraphicsItem* m_dummyConnectionItem;

        };

    }
}
