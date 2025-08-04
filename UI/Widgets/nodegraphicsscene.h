#pragma once
#include <QGraphicsScene>
#include "Models/nodegraphmodel.h"
#include "GraphicsItems/connectionpreviewgraphicsitem.h"

namespace Pipeline
{
    namespace UI
    {
        class NodeGraphicsScene : public QGraphicsScene
        {
                Q_OBJECT
            public:
                explicit NodeGraphicsScene(QObject*parent = nullptr);
                void setModel(NodeGraphModel* model);
                NodeGraphModel* model() const
                {
                    return m_model;
                }
            protected:
                void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
                void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
                void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
                void dropEvent(QGraphicsSceneDragDropEvent *event) override;
            private:
                void loadItemUsingModel();
                void onRowsInserted(const QModelIndex &parent, int first, int last);
                void onRowsRemoved(const QModelIndex &parent, int first, int last);
                void onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
                void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int>& roles = QVector<int>());
            private:
                NodeGraphModel* m_model;
                ConnectionPreviewGraphicsItem* m_previewItem;
        };
    }
}
