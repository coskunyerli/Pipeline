#pragma once

#include <QAbstractItemModel>

namespace Pipeline
{
    namespace UI
    {
        class NodeGraphModel : public QAbstractItemModel
        {
                Q_OBJECT
            public:
                NodeGraphModel(QObject*parent = nullptr);
        };
    }
}
