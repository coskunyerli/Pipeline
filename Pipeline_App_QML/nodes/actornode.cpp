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
        {
            m_actor = new Thread::Actor(QThreadPool::globalInstance(), nullptr);
            m_actor->setBehaviour(Thread::Behaviour([this](const Thread::BehaviourContext & context)
            {
                return this->behaviour(context);
            }));
            Thread::Actor::connect(m_actor, &Thread::Actor::failed, [](QVariant res){
                qDebug() << res;
            });
        }

        ActorNode::ActorNode(Thread::Actor *actor)
            : UI::MNode()
            , m_actor(actor)
        {
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

    }
}

