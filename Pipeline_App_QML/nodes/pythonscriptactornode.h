#pragma once
#include "actornode.h"
namespace Pipeline
{
    namespace Runtime
    {
        class PythonScriptActorNode : public ActorNode
        {
            public:
                explicit PythonScriptActorNode();
                explicit PythonScriptActorNode(Thread::Actor* actor);
                explicit PythonScriptActorNode(QString script);
                explicit PythonScriptActorNode(QString script, Thread::Actor* actor);
                void setScript(const QString &script);
                QVariant behaviour(const Thread::BehaviourContext& behaviour) override;
                QHash<int, QByteArray> roleNames() const override;
                bool setData(const QVariant &value, int role) override;
                virtual QVariant data(int role) const override;
            private:
                QString m_pythonScript;
        };
    }
}
