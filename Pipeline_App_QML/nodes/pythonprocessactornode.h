#pragma once
#include "actornode.h"
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
            private:
                QString m_filename;
                QSharedMemory *m_memory;
        };
    }
}
