#include "nodegraphicsscene.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QMimeData>
#include "GraphicsItems/basenodegraphicsitem.h"

namespace Pipeline
{
    namespace UI
    {
        NodeGraphicsScene::NodeGraphicsScene(QObject *parent)
            : QGraphicsScene(parent)
            , m_model(nullptr)
        {
        }

        void NodeGraphicsScene::setModel(NodeGraphModel *model)
        {
            if (m_model)
            {
                // disconnect all signals
                disconnect(m_model, &NodeGraphModel::rowsInserted, this, &NodeGraphicsScene::onRowsInserted);
                disconnect(m_model, &NodeGraphModel::rowsRemoved, this, &NodeGraphicsScene::onRowsRemoved);
                disconnect(m_model, &NodeGraphModel::rowsAboutToBeRemoved, this, &NodeGraphicsScene::onRowsAboutToBeRemoved);
                disconnect(m_model, &NodeGraphModel::dataChanged, this, &NodeGraphicsScene::onDataChanged);
            }

            m_model = model;

            // connect model signals
            if (m_model)
            {
                connect(m_model, &NodeGraphModel::rowsInserted, this, &NodeGraphicsScene::onRowsInserted);
                connect(m_model, &NodeGraphModel::rowsRemoved, this, &NodeGraphicsScene::onRowsRemoved);
                connect(m_model, &NodeGraphModel::rowsAboutToBeRemoved, this, &NodeGraphicsScene::onRowsAboutToBeRemoved);
                connect(m_model, &NodeGraphModel::dataChanged, this, &NodeGraphicsScene::onDataChanged);
            }

            loadItemUsingModel();
            // update UI from zero
        }

        void NodeGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
        {
            QGraphicsScene::dragEnterEvent(event);
            auto *mimeData = event->mimeData();
            bool ok;
            size_t portIndex = mimeData->property("nodePort").toLongLong(&ok);
            QPointF startPos = mimeData->property("startPos").toPointF();

            if (ok)
            {
                m_previewItem = new ConnectionPreviewGraphicsItem();
                m_previewItem->setStart(startPos);
                this->addItem(m_previewItem);
            }
            else
            {
                m_previewItem = nullptr;
            }

            event->acceptProposedAction();
            event->accept();
        }

        void NodeGraphicsScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
        {
            QGraphicsScene::dragLeaveEvent(event);

            if (m_previewItem)
            {
                removeItem(m_previewItem);
                m_previewItem = nullptr;
            }
        }

        void NodeGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
        {
            QGraphicsScene::dragMoveEvent(event);

            if (m_previewItem)
            {
                m_previewItem->setEnd(event->scenePos());
            }

            event->acceptProposedAction();
            event->accept();
        }

        void NodeGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event)
        {
            QGraphicsScene::dropEvent(event);

            if (m_previewItem)
            {
                removeItem(m_previewItem);
                m_previewItem = nullptr;
            }

            qDebug() << "drop;";
        }

        void NodeGraphicsScene::loadItemUsingModel()
        {
            clear();

            if (m_model)
            {
            }

            // draw nodes
            // draw connections
        }

        void NodeGraphicsScene::onRowsInserted(const QModelIndex &parent, int first, int last)
        {
            auto *model = this->model();

            if (!model)
            {
                return;
            }

            for (int i = first; i <= last; i++)
            {
                auto index = model->index(first, 0, parent);

                if (index.data(Roles::Type).toInt() == DataType::Node)
                {
                    auto *nodeGraphicsItem = new BaseNodeGraphicsItem(index);
                    nodeGraphicsItem->setPos(index.data(Roles::PosX).toFloat(), index.data(Roles::PosY).toFloat());
                    //this->addItem(nodeGraphicsItem);
                }
                else if (index.data(Roles::Type).toInt() == DataType::Connection)
                {
                    auto *nodeGraphicsItem = new BaseNodeGraphicsItem(index);
                    nodeGraphicsItem->setPos(index.data(Roles::PosX).toFloat(), index.data(Roles::PosY).toFloat());
                    //this->addItem(nodeGraphicsItem);
                }
            }
        }

        void NodeGraphicsScene::onRowsRemoved(const QModelIndex &parent, int first, int last)
        {
        }

        void NodeGraphicsScene::onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
        {
        }

        void NodeGraphicsScene::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int>& roles)
        {
        }
    }
}
