#include "nodefactory.h"
#include <data/contextmetadata.h>
#include <nodes/pythonprocessactornode.h>
#include <constants.h>
#include <QJsonArray>
#include <QJsonObject>
namespace Pipeline::Runtime
{

    ActorNode* Pipeline::Runtime::NodeFactory::create(const NodeContextMetadata &metadata)
    {
        ActorNode *node = nullptr;

        if (metadata.getNodeType() == NodeTypes::PythonNode)
        {
            node = new PythonProcessActorNode();
            node->applyNodeContextMetadata(metadata);
            auto *outPort = new UI::MPort(node);
            auto *inPort = new UI::MPort(node);
            node->addPort(inPort, true);
            node->addPort(outPort, false);
        }

        return node;
    }
}
