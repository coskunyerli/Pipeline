#include "baseactornodedispatcher.h"
#include <nodes/actornode.h>
#include <contexts/basedatacontext.h>
namespace Pipeline::Runtime
{

    BaseActorNodeDispatcher::BaseActorNodeDispatcher(ActorNode *node)
        : QObject(nullptr)
        , m_relatedNode(node)
    {
    }

    BaseDataContext* BaseActorNodeDispatcher::createDataContext(QObject *parent)
    {
        if (!m_relatedNode)
        {
            return nullptr;
        }

        return m_relatedNode->createDataContext(parent);
    }

    void BaseActorNodeDispatcher::saveContext(BaseDataContext *dataContext)
    {
        if (!m_relatedNode)
        {
            return;
        }

        this->m_relatedNode->saveContext(dataContext);
    }

}
