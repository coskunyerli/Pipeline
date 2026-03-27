#pragma once
#include <QObject>
#include <QQmlEngine>
#include <QAbstractItemModel>
#include <data/contextmetadata.h>
namespace Pipeline::Runtime
{
    class NodeModel;
    class AppDataService;
    class NodeContextMetadata;
    class NodeModelService : public QObject
    {
            Q_OBJECT
            Q_PROPERTY(QAbstractItemModel* model READ model CONSTANT)

        public:
            explicit NodeModelService(QObject* parent = nullptr);
            //static NodeModelService* instance(QQmlEngine*, QJSEngine*);

            QAbstractItemModel* model() const;

            Q_INVOKABLE bool saveNode(const QString& description, const NodeContextMetadata& data);
            Q_INVOKABLE NodeContextMetadata loadNode(const QString& name);
            Q_INVOKABLE QStringList listNodes();

        private:


            void loadAllNodes();

        private:
            NodeModel* m_model = nullptr;
    };
}
