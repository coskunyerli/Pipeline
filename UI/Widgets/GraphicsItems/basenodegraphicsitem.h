#pragma once
#include <QGraphicsRectItem>
#include <QPersistentModelIndex>
namespace Pipeline
{
    namespace UI
    {
        class BaseNodeGraphicsItem : public QGraphicsRectItem
        {
            public:
                BaseNodeGraphicsItem(const QModelIndex &index, QGraphicsItem *parent = nullptr);
                QModelIndex getIndex() const
                {
                    return this->m_index;
                }
            protected:
                void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
                void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
                void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

                void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
                void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
                void dropEvent(QGraphicsSceneDragDropEvent *event) override;

            private:
                QPointF m_clickedPos;
                bool m_hasMove;
                QPersistentModelIndex m_index;
        };
    }
}
