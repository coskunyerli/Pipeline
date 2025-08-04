#pragma once
#include "node.h"
#include "pipeline_core_dll.h"
namespace Pipeline
{
    namespace Core
    {
        class PIPELINE_CORE_API FlowNode: public Node
        {
            public:
                explicit FlowNode();
                virtual ~FlowNode();
                Node* getNode(size_t index) const;
                size_t getChildNodeCount() const;
                // this is the start node of pipeline, all the rest is added automatically
                bool addNode(Node* start);
                size_t indexOf(Node* child, bool& has);

            private:
                struct PrivateData;
                PrivateData* m_privateData;
        };
    }
}
