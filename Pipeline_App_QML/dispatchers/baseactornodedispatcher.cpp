#include "baseactornodedispatcher.h"
#include <nodes/actornode.h>
#include <contexts/basedatacontext.h>
#include <data/contextmetadata.h>
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

    NodeContextMetadata BaseActorNodeDispatcher::createMetadata()
    {
        if (!m_relatedNode)
        {
            return NodeContextMetadata();
        }

        return m_relatedNode->createMetadata();
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
