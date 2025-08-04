#pragma once
#include "Models/nodegraphtreemodel.h"
#include "actor.h"

namespace Pipeline
{
    namespace Runtime
    {
        class ActorNode: public UI::MNode
        {
            public:
                explicit ActorNode();
                explicit ActorNode(Thread::Actor* actor);
                void setActor(Thread::Actor* actor);
                void addNextNode(ActorNode* other);
                void removeNextNode(ActorNode* other);
                virtual QVariant behaviour(const Thread::BehaviourContext& behaviour) = 0;
                void run();
            private:
                Thread::Actor* m_actor;
        };
    }
}
