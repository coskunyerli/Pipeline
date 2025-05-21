#pragma once
#include <QGraphicsScene>

namespace Pipeline
{
    namespace UI
    {
        class NodeGraphicsScene : public QGraphicsScene
        {
                Q_OBJECT
            public:
                explicit NodeGraphicsScene(QObject*parent = nullptr);
            protected:
                void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
                void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
                void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;

        };
    }
}
