#pragma once
#include "actornode.h"
namespace Pipeline
{
    namespace Runtime
    {
        class PythonFileActorNode : public ActorNode
        {
            public:
                explicit PythonFileActorNode();
                explicit PythonFileActorNode(QString script);
                void setFilename(const QString &filename);
                QVariant behaviour(const Thread::BehaviourContext& behaviour) override;
                QHash<int, QByteArray> roleNames() const override;
                bool setData(const QVariant &value, int role) override;
                virtual QVariant data(int role) const override;
            private:
                QString m_filename;
        };
    }
}
