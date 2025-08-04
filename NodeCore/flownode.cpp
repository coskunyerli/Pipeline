#include "flownode.h"
#include <vector>
namespace Pipeline
{
    namespace Core
    {
        struct FlowNode::PrivateData
        {
            std::vector<Node*> m_nodes;
        };

        FlowNode::FlowNode()
            : Node()
            , m_privateData(new PrivateData())
        {
        }

        FlowNode::~FlowNode()
        {
            for (auto *node : m_privateData->m_nodes)
            {
                delete node;
            }

            delete m_privateData;
        }

        Node* FlowNode::getNode(size_t index) const
        {
            if (index >= getChildNodeCount())
            {
                return nullptr;
            }

            return m_privateData->m_nodes[index];
        }

        size_t FlowNode::getChildNodeCount() const
        {
            return m_privateData->m_nodes.size();
        }

        bool FlowNode::addNode(Node *start)
        {
            if (!start)
            {
                return false;
            }

            if (std::find(m_privateData->m_nodes.begin(), m_privateData->m_nodes.end(), start) == m_privateData->m_nodes.end())
            {
                m_privateData->m_nodes.push_back(start);
                start->setParent(this);
                return true;
            }

            return false;
        }

        size_t FlowNode::indexOf(Node *child, bool &has)
        {
            auto it = std::find(m_privateData->m_nodes.begin(), m_privateData->m_nodes.end(), child);

            if (it != m_privateData->m_nodes.end())
            {
                has = true;
                return static_cast<size_t>(std::distance(m_privateData->m_nodes.begin(), it));
            }

            has = false;
            return 0;
        }

    }
}
