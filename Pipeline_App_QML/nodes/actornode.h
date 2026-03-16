#pragma once
#include "Models/nodegraphtreemodel.h"
#include "actor.h"

class QObject;
namespace Pipeline
{
    namespace Runtime
    {
        class HierarchicalTableData;
    class BaseActorNodeDispatcher;
        class BaseDataContext;
        class ActorNode: public UI::MNode
        {
            public:
                explicit ActorNode();
                explicit ActorNode(Thread::Actor* actor);
                ~ActorNode();
                void setActor(Thread::Actor* actor);
                void addNextNode(ActorNode* other);
                void removeNextNode(ActorNode* other);
                QHash<int, QByteArray> roleNames() const override;
                QVariant data(int role) const override;
                virtual QVariant behaviour(const Thread::BehaviourContext& behaviour) = 0;
                // onStarted is working before starting execution
                virtual void onStarted() = 0;
                virtual void onFinished(const QVariant& result) = 0;
                virtual void onFailed(const QVariant& result) = 0;
                virtual void run();
                virtual void runStandalone();
                Thread::Actor::ProgressState getState() const;
                BaseActorNodeDispatcher* getDispatcher() const;
                virtual void setDispatcher(BaseActorNodeDispatcher*dispatcher);
                Thread::BehaviourContext getContext();
                std::shared_ptr<HierarchicalTableData> createInputDataFromContext(const QList<QVariant>& inputDataList);
                virtual BaseDataContext* createDataContext(QObject *parent = nullptr) = 0;
                virtual void saveContext(BaseDataContext* dataContext) = 0;
            private:
                BaseActorNodeDispatcher* m_dispatcher;
                Thread::Actor* m_actor;

        };
    }
}
