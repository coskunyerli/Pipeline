#pragma once
namespace Pipeline::Runtime
{
    class ActorNode;
    class NodeContextMetadata;
    class NodeFactory
    {
        public:
            static ActorNode* create(const NodeContextMetadata &metadata);
    };
}
