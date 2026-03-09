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
                // onStarted is working before starting execution
                virtual void onStarted() = 0;
                virtual void onFinished(const QVariant& result) = 0;
                virtual void onFailed(const QVariant& result) = 0;
                virtual void run();
                virtual void runStandalone();
                QObject* getDispatcher() const;
                virtual void setDispatcher(QObject*dispatcher);
                Thread::BehaviourContext getContext();
            private:
                QObject* m_dispatcher;
                Thread::Actor* m_actor;

        };
    }
}
