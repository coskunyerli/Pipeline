#include "actornode.h"
#include "actor.h"
#include <QDebug>
#include <data/hierarchicaltabledata.h>
#include <dispatchers/baseactornodedispatcher.h>
#include <constants.h>

namespace Pipeline
{
    namespace Runtime
    {

        ActorNode::ActorNode()
            : UI::MNode()
            , m_dispatcher(nullptr)
            , m_actor(nullptr)

        {
            m_actor = new Thread::Actor(QThreadPool::globalInstance(), nullptr);
            m_actor->setBehaviour(Thread::Behaviour([this](const Thread::BehaviourContext & context)
            {
                return this->behaviour(context);
            }));
            // here could be a problem about virtual function
            this->setDispatcher(new BaseActorNodeDispatcher(this));
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

        QHash<int, QByteArray> ActorNode::roleNames() const
        {
            auto roles = UI::MNode::roleNames();
            roles[NodeRoles::ActorAction] = "actorAction";
            return roles;
        }

        QVariant ActorNode::data(int role) const
        {
            if (role == NodeRoles::ActorAction)
            {
                QVariant v = QVariant::fromValue(static_cast<QObject*>(this->getDispatcher()));
                return v;
            }

            return UI::MNode::data(role);
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

        BaseActorNodeDispatcher* ActorNode::getDispatcher() const
        {
            return m_dispatcher;
        }

        void ActorNode::setDispatcher(BaseActorNodeDispatcher *dispatcher)
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

        std::shared_ptr<HierarchicalTableData> ActorNode::createInputDataFromContext(const QList<QVariant>& inputDataList)
        {
            auto parentInputData = std::make_shared<HierarchicalTableData>();
            parentInputData->setSize(10, 10);
            int count = 0;

            for (auto &variant : inputDataList)
            {
                if (variant.canConvert<std::shared_ptr<HierarchicalTableData>>())
                {
                    auto inputData = variant.value<std::shared_ptr<HierarchicalTableData>>();

                    if (inputData)
                    {
                        parentInputData->setCellValue(count, inputData->getValue());
                        parentInputData->setCell(count, inputData);
                        count++;
                    }
                }
            }

            if (count == 0)
            {
                return nullptr;
            }

            if (count == 1)
            {
                // if just one cell, dont put in parent node, not necessary
                auto childCell = parentInputData->removeCell(0, 0);
                return childCell;
            }

            return parentInputData;
        }

    }
}

