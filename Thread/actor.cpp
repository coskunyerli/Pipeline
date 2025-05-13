#include "actor.h"
#include <QThreadPool>
#include "actorrunnable.h"
namespace Pipeline
{
    Actor::Actor(QThreadPool *pool, QObject *parent)
    : QObject(parent)
    , m_threadPool(pool)
    , m_state(READY)
    , m_priority(0)
    {

    }

    Actor::Actor(QThreadPool *pool, int priority, QObject *parent)
    : QObject(parent)
    , m_threadPool(pool)
    , m_state(READY)
    , m_priority(priority)
    {

    }

    Actor::Actor(QObject *parent)
    : QObject(parent)
    , m_state(READY)
    , m_priority(0)
    {
        m_threadPool = QThreadPool::globalInstance();
    }

    void Actor::addNextActor(Actor *other)
    {
        if(other && !this->m_nextActors.contains(other))
        {
            this->m_nextActors.push_back(other);
            other->setDependentActor(this);
        }
    }

    BehaviourContext Actor::getContext() const
    {
        BehaviourContext context;
        for(auto* dependent : m_dependentActors)
        {
            context.m_variants.append(dependent->getValue());
        }
        return context;
    }

    void Actor::startRequest()
    {
        if(m_semaphore.available() == 0)
        {
            if(m_threadPool)
            {
                ActorRunnable *r = new ActorRunnable(this);
                m_threadPool->start(r, m_priority);
            }

        }
    }

    void Actor::reset()
    {
        m_semaphore.release(this->m_dependentActors.size() - m_semaphore.available());
        m_state = READY;
        for(auto *next: this->m_nextActors)
        {
            next->reset();
        }
    }

    void Actor::setDependentActor(Actor *other)
    {
        if(other && !m_dependentActors.contains(other))
        {
            m_dependentActors.push_back(other);
            m_semaphore.release(1);
            connect(other, &Actor::finished, this, &Actor::onDependentFinished);
        }
    }

    void Actor::onDependentFinished(QVariant /*value*/)
    {
        m_semaphore.acquire();
        startRequest();
    }

}
