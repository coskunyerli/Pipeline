#pragma once
#include <QGraphicsView>
#include <QAbstractItemModel>
namespace Pipeline
{
    namespace UI
    {
        class NodeGraphView : public QGraphicsView
        {
                Q_OBJECT
            public:
                NodeGraphView(QWidget * parent = nullptr);
                void setModel(QAbstractItemModel *model);
            private:
                void onCustomContextMenuRequested(const QPoint &pos);
                void initSignalsAndSlots();
            private:
                QAbstractItemModel* m_model;
        };

    }
}
