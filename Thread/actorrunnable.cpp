#include "actorrunnable.h"

namespace Pipeline
{

    ActorRunnable::ActorRunnable(Actor *actor)
        : m_actor(actor)
    {

    }

    void ActorRunnable::run()
    {
        if(m_actor)
        {
            try
            {
                m_actor->m_state = Actor::PROGRASSING;
                emit m_actor->started();
                auto value = m_actor->getBehaviour().run(m_actor->getContext());
                m_actor->m_value = value;
                m_actor->m_state = Actor::FINISHED;
                emit m_actor->finished(value);
            }
            catch(std::exception &e)
            {
                 m_actor->m_state = Actor::FAILED;
                 emit m_actor->failed(e.what());
            }
            catch(...)
            {
                m_actor->m_state = Actor::FAILED;
                emit m_actor->failed("Unknow Exception");
            }
        }
    }

}
