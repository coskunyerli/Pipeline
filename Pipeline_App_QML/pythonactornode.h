#pragma once
#include "actornode.h"
namespace Pipeline
{
    namespace Runtime
    {
        class PythonActorNode : public ActorNode
        {
            public:
                explicit PythonActorNode();
                explicit PythonActorNode(Thread::Actor* actor);
                explicit PythonActorNode(QString script);
                explicit PythonActorNode(QString script, Thread::Actor* actor);
                void setScript(const QString &script);
                QVariant behaviour(const Thread::BehaviourContext& behaviour) override;
                QHash<int, QByteArray> roleNames() const override;
                bool setData(const QVariant &value, int role);
            private:
                QString m_pythonScript;
        };
    }
}
