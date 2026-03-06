#pragma once
#include "actornode.h"
#include <models/nodetablemodel.h>
#include <QObject>

class QSharedMemory;
namespace Pipeline
{
    namespace Runtime
    {
        class PythonProcessActorNode : public ActorNode
        {
            public:
                explicit PythonProcessActorNode();
                ~PythonProcessActorNode() override;
                void setFilename(const QString &filename);
                QVariant behaviour(const Thread::BehaviourContext& behaviour) override;
                QHash<int, QByteArray> roleNames() const override;
                bool setData(const QVariant &value, int role) override;
                virtual QVariant data(int role) const override;
                void onFinished(const QVariant& result) override;
                void onFailed(const QVariant& result) override;
            private:
                QString m_filename;
                NodeTableModel* m_inputDataTable;
                NodeTableModel* m_outputDataTable;
                QString m_pythonError;
        };

        class PythonDispatcher : public QObject
        {
            Q_OBJECT
        public:
            PythonDispatcher(PythonProcessActorNode*node);
            Q_INVOKABLE void run();
        private:
            PythonProcessActorNode* m_pythonNode;
        };
    }
}
