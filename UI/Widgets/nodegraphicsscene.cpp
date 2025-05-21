#include "nodegraphicsscene.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

namespace Pipeline
{
    namespace UI
    {
        NodeGraphicsScene::NodeGraphicsScene(QObject *parent)
            : QGraphicsScene(parent)
        {
        }

        void NodeGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
        {
            QGraphicsScene::dragEnterEvent(event);
        }

        void NodeGraphicsScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
        {
            QGraphicsScene::dragLeaveEvent(event);
        }

        void NodeGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
        {
            QGraphicsScene::dragMoveEvent(event);
            qDebug() << event->scenePos();
        }
    }
}
