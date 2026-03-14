#include "actornode.h"
#include "actor.h"
#include <QDebug>
namespace Pipeline
{
    namespace Runtime
    {

        ActorNode::ActorNode()
            : UI::MNode()
            , m_actor(nullptr)
            , m_dispatcher(nullptr)
        {
            m_actor = new Thread::Actor(QThreadPool::globalInstance(), nullptr);
            m_actor->setBehaviour(Thread::Behaviour([this](const Thread::BehaviourContext & context)
            {
                return this->behaviour(context);
            }));
            this->setDispatcher(new QObject());
        }

        ActorNode::ActorNode(Thread::Actor *actor)
            : UI::MNode()
            , m_actor(actor)
        {
        }

        ActorNode::~ActorNode()
        {
            this->m_dispatcher->deleteLater();
        }

        void ActorNode::setActor(Thread::Actor *actor)
        {
            if (m_actor == actor)
            {
                return;
            }

            m_actor = actor;
        }

        void ActorNode::addNextNode(ActorNode* other)
        {
            if (!m_actor)
            {
                return;
            }

            m_actor->addNextActor(other->m_actor);
        }

        void ActorNode::removeNextNode(ActorNode *other)
        {
            if (!m_actor)
            {
                return;
            }

            m_actor->removeNextActor(other->m_actor);
        }

        void ActorNode::run()
        {
            if (!m_actor)
            {
                return;
            }

            m_actor->startRequest();
        }

        void ActorNode::runStandalone()
        {
            if (!m_actor)
            {
                return;
            }

            m_actor->startRequestStandalone();
        }

        Thread::Actor::ProgressState ActorNode::getState() const
        {
            return m_actor->getState();
        }

        QObject* ActorNode::getDispatcher() const
        {
            return m_dispatcher;
        }

        void ActorNode::setDispatcher(QObject *dispatcher)
        {
            if (m_dispatcher)
            {
                m_dispatcher->deleteLater();
                m_dispatcher = nullptr;
            }

            m_dispatcher = dispatcher;
            Thread::Actor::connect(m_actor, &Thread::Actor::started, m_dispatcher, [this]()
            {
                this->onStarted();
            });
            Thread::Actor::connect(m_actor, &Thread::Actor::failed, m_dispatcher, [this](QVariant res)
            {
                qDebug() << res;
                this->onFailed(res);
            });
            Thread::Actor::connect(m_actor, &Thread::Actor::finished, m_dispatcher, [this](QVariant res)
            {
                this->onFinished(res);
            });
        }

        Thread::BehaviourContext ActorNode::getContext()
        {
            if (!m_actor)
            {
                return {};
            }

            return m_actor->getContext();
        }

    }
}

