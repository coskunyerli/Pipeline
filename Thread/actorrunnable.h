#pragma once
#include <QRunnable>
#include "actor.h"
namespace Pipeline
{
    namespace Thread
    {
        class ActorRunnable : public QRunnable
        {
            public:
                ActorRunnable(Actor* actor, bool triggerNext = true);
                virtual void run();
            private:
                Actor* m_actor;
                bool m_triggerNext;
        };
    }
}

