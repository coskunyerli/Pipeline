#pragma once
#include "Models/nodegraphtreemodel.h"
#include "actor.h"

class QObject;
namespace Pipeline
{
    namespace Runtime
    {
        class ActorNode: public UI::MNode
        {
            public:
                explicit ActorNode();
                explicit ActorNode(Thread::Actor* actor);
                ~ActorNode();
                void setActor(Thread::Actor* actor);
                void addNextNode(ActorNode* other);
                void removeNextNode(ActorNode* other);
                virtual QVariant behaviour(const Thread::BehaviourContext& behaviour) = 0;
                virtual void onFinished(const QVariant& result) = 0;
                virtual void onFailed(const QVariant& result) = 0;
                void run();
                void runStandalone();
                QObject* getDispatcher() const;
                virtual void setDispatcher(QObject*dispatcher);
            private:
                QObject* m_dispatcher;
                Thread::Actor* m_actor;

        };
    }
}
