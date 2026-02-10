#pragma once
#include "actornode.h"
#include "models/nodetablemodel.h"

class QSharedMemory;
namespace Pipeline
{
    namespace Runtime
    {

        class PythonFileActorNode : public ActorNode
        {
            public:
                explicit PythonFileActorNode();
                explicit PythonFileActorNode(QString script);
                ~PythonFileActorNode() override;
                void setFilename(const QString &filename);
                QVariant behaviour(const Thread::BehaviourContext& behaviour) override;
                QHash<int, QByteArray> roleNames() const override;
                bool setData(const QVariant &value, int role) override;
                virtual QVariant data(int role) const override;
            private:
                QString m_filename;
                NodeTableModel *m_inputDataTable;
                NodeTableModel *m_outputDataTable;
                QSharedMemory* m_memory;

        };
    }
}
