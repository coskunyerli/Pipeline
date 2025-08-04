#include "nodegraphview.h"
#include <QMenu>
#include <QGraphicsScene>
namespace Pipeline
{
    namespace UI
    {
        NodeGraphView::NodeGraphView(QWidget *parent)
            : QGraphicsView(parent)
        {
            auto *scene = new NodeGraphicsScene(this);
            scene->setItemIndexMethod(QGraphicsScene::NoIndex);
            this->setScene(scene);
            setContextMenuPolicy(Qt::CustomContextMenu);
            initSignalsAndSlots();
        }

        NodeGraphicsScene* NodeGraphView::graphScene() const
        {
            return qobject_cast<NodeGraphicsScene*>(scene());
        }

        void NodeGraphView::onCustomContextMenuRequested(const QPoint &pos)
        {
            auto *graphScene = this->graphScene();

            if (!graphScene || !graphScene->model() || !graphScene->model()->graphModel())
            {
                return;
            }

            auto *graphModel = graphScene->model()->graphModel();
            QMenu menu;
            auto *addAction = menu.addAction("Add Item");
            auto *action = menu.exec(mapToGlobal(pos));

            if (action == addAction)
            {
                auto scenePos = mapToScene(pos);
                auto *node = new MNode();
                auto *port = new MPort(node);
                node->addPort(port, false);
                node->setX(scenePos.x());
                node->setY(scenePos.y());
                graphModel->addNode(node);
            }
        }

        void NodeGraphView::initSignalsAndSlots()
        {
            connect(this, &NodeGraphView::customContextMenuRequested, this, &NodeGraphView::onCustomContextMenuRequested);
        }
    }
}
