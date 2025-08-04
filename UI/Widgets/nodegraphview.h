#pragma once
#include <QGraphicsView>
#include "nodegraphicsscene.h"
namespace Pipeline
{
    namespace UI
    {
        class NodeGraphView : public QGraphicsView
        {
                Q_OBJECT
            public:
                NodeGraphView(QWidget * parent = nullptr);
                NodeGraphicsScene* graphScene() const;

            private:
                void onCustomContextMenuRequested(const QPoint &pos);
                void initSignalsAndSlots();
        };

    }
}
