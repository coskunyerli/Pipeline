#pragma once
#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include "Models/nodegraphtreemodel.h"
#include <models/nodemodel.h>
#include <QPointF>

namespace Pipeline
{
    namespace Runtime
    {
        /*singleton.h*/
        class GraphModelService : public QObject
        {
                Q_OBJECT
                Q_PROPERTY(UI::NodeGraphTreeModel* model READ model WRITE setModel NOTIFY modelChanged)
                Q_PROPERTY(NodeModel* quickNodeModel READ quickNodeModel WRITE setQuickNodeModel NOTIFY quickNodeModelChanged)
            signals:
                void modelChanged();
                void quickNodeModelChanged();

            public:
                Q_INVOKABLE bool addNode(const QString&quickNodeName, const QPointF& pos);
                Q_INVOKABLE void start();
            public:
                explicit GraphModelService(QObject *parent = nullptr);
                virtual ~GraphModelService();
                UI::NodeGraphTreeModel* model() const;
                void setModel(UI::NodeGraphTreeModel* model);
                NodeModel *quickNodeModel() const;
                void setQuickNodeModel(NodeModel *newQuickNodeModel);

            private:
                void onRowsInserted(const QModelIndex &parent, int first, int last);
            private:
                UI::NodeGraphTreeModel* m_model;
                NodeModel *m_quickNodeModel = nullptr;
        };
    }
}
