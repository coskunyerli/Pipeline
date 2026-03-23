#pragma once
#include <QObject>
namespace Pipeline::Runtime
{
    class ActorNode;
    class BaseDataContext;
    class BaseActorNodeDispatcher: public QObject
    {
            Q_OBJECT
        public:
            BaseActorNodeDispatcher(ActorNode *node);
            Q_INVOKABLE BaseDataContext* createDataContext(QObject*parent = nullptr);
            Q_INVOKABLE void saveContext(BaseDataContext* dataContext);

        private:
            ActorNode* m_relatedNode;
    };
}
