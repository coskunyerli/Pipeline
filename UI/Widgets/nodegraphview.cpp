#include "nodegraphview.h"
#include <QMenu>
#include <QGraphicsScene>
#include "GraphicsItems/basenodegraphicsitem.h"
#include "GraphicsItems/portgraphicsitem.h"
namespace Pipeline
{
    namespace UI
    {
        NodeGraphView::NodeGraphView(QWidget *parent)
            : QGraphicsView(parent)
        {
            auto *scene = new QGraphicsScene(this);
            scene->setItemIndexMethod(QGraphicsScene::NoIndex);
            this->setScene(scene);
            setContextMenuPolicy(Qt::CustomContextMenu);
            initSignalsAndSlots();
        }

        void NodeGraphView::setModel(QAbstractItemModel *model)
        {
            // TODOJ burada connection işlerini yap UI işlerini yap
        }

        void NodeGraphView::onCustomContextMenuRequested(const QPoint &pos)
        {
            QMenu menu;
            auto *addAction = menu.addAction("Add Item");
            auto *action = menu.exec(mapToGlobal(pos));

            if (action == addAction)
            {
                auto *nodeGraphicsItem = new BaseNodeGraphicsItem();
                auto *portItem = new PortGraphicsItem(nodeGraphicsItem);
                portItem->moveBy(80, 0);
                this->scene()->addItem(nodeGraphicsItem);
            }
        }

        void NodeGraphView::initSignalsAndSlots()
        {
            connect(this, &NodeGraphView::customContextMenuRequested, this, &NodeGraphView::onCustomContextMenuRequested);
        }
    }
}
