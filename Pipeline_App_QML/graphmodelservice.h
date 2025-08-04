#pragma once
#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include "Models/nodegraphtreemodel.h"

namespace Pipeline
{
    namespace Runtime
    {
        /*singleton.h*/
        class GraphModelService : public QObject
        {
                Q_OBJECT
                Q_PROPERTY(UI::NodeGraphTreeModel* model READ model WRITE setModel NOTIFY modelChanged)
            signals:
                void modelChanged();
            public:
                Q_INVOKABLE bool addNode(const QPointF& pos);
                Q_INVOKABLE void start();
            public:
                explicit GraphModelService(QObject *parent = nullptr);
                virtual ~GraphModelService();
                UI::NodeGraphTreeModel* model() const;
                void setModel(UI::NodeGraphTreeModel* model);
            private:
                void onRowsInserted(const QModelIndex &parent, int first, int last);
            private:
                UI::NodeGraphTreeModel* m_model;



        };
    }
}
