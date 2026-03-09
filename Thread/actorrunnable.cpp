#include "actorrunnable.h"

namespace Pipeline
{
    namespace Thread
    {
        ActorRunnable::ActorRunnable(Actor *actor, bool triggerNext)
            : m_actor(actor)
            , m_triggerNext(triggerNext)
        {
        }

        void ActorRunnable::run()
        {
            if (m_actor)
            {
                try
                {
                    m_actor->m_state = Actor::PROGRASSING;
                    auto value = m_actor->getBehaviour().run(m_actor->getContext());
                    m_actor->m_value = value;
                    m_actor->m_state = Actor::FINISHED;
                    emit m_actor->finished(value);

                    if (m_triggerNext)
                    {
                        emit m_actor->nextActorRequested(value);
                    }
                }
                catch (std::exception &e)
                {
                    m_actor->m_state = Actor::FAILED;
                    emit m_actor->failed(e.what());
                }
                catch (...)
                {
                    m_actor->m_state = Actor::FAILED;
                    emit m_actor->failed("Unknow Exception");
                }
            }
        }
    }
}
