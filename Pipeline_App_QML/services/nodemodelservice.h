#pragma once
#include <QObject>
#include <QQmlEngine>

namespace Pipeline::Runtime
{
    class NodeModel;
    class AppDataService;

    class NodeModelService : public QObject
    {
            Q_OBJECT
            Q_PROPERTY(QObject* model READ model CONSTANT)

        public:

            static NodeModelService* instance(QQmlEngine*, QJSEngine*);

            QObject* model() const;

            Q_INVOKABLE bool saveNode(const QString& name, const QByteArray& data);
            Q_INVOKABLE QByteArray loadNode(const QString& name);
            Q_INVOKABLE QStringList listNodes();

        private:

            explicit NodeModelService(QObject* parent = nullptr);
            void loadAllNodes();

        private:
            NodeModel* m_model = nullptr;
    };
}
