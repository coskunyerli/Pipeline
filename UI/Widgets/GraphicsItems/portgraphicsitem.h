#pragma once
#include <QGraphicsEllipseItem>
namespace Pipeline
{
    namespace UI
    {
        class ConnectionGraphicsItem;
        class BaseNodeGraphicsItem;
        class PortGraphicsItem : public QGraphicsEllipseItem
        {
            public:
                explicit PortGraphicsItem(size_t portIndex, QGraphicsItem *parent = nullptr);
                void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
                void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
                void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
                void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
                void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
                void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
                void dropEvent(QGraphicsSceneDragDropEvent *event) override;
                BaseNodeGraphicsItem* getNode() const;
            private:
                bool m_hasMove;
                ConnectionGraphicsItem* m_dummyConnectionItem;
                size_t m_portIndex;

        };

    }
}
