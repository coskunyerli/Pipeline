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
            auto *inPort2 = new UI::MPort(node);
            inPort->setName("In Port1");
            inPort2->setName("In Port2");
            outPort->setName("Output Port");
            node->addPort(inPort, true);
            node->addPort(inPort2, true);
            node->addPort(outPort, false);
        }

        return node;
    }
}
