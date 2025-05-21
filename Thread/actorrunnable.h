#pragma once
#include <QRunnable>
#include "actor.h"
namespace Pipeline
{
    class ActorRunnable : public QRunnable
    {
        public:
            ActorRunnable(Actor* actor);
            virtual void run();
        private:
            Actor* m_actor;
    };
}

