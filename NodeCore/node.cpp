#include "flownode.h"
#include "port.h"

namespace Pipeline
{
    namespace Core
    {
        struct Node::PrivateData
        {
            std::string m_name;
            std::vector<Port*> m_outPorts;
            std::vector<Port*> m_inPorts;
        };

        Node::Node()
            : m_x(0)
            , m_y(0)
            , m_parent(nullptr)
            , m_privateData(new PrivateData())

        {
        }

        Node::Node(const Node &other)
            : m_x(other.m_x)
            , m_y(other.m_y)
            , m_parent(other.m_parent)
            , m_privateData(new PrivateData(*other.m_privateData))
        {
        }

        Node::~Node()
        {
            delete m_privateData;
        }

        FlowNode* Node::parent() const
        {
            return m_parent;
        }

        void Node::setParent(FlowNode *parent)
        {
            m_parent  = parent;
        }

        std::string Node::getName() const
        {
            return m_privateData->m_name;
        }

        float Node::getX() const
        {
            return m_x;
        }

        float Node::getY() const
        {
            return m_y;
        }

        void Node::setX(float x)
        {
            m_x = x;
        }

        void Node::setY(float y)
        {
            m_y = y;
        }

        bool Node::addPort(Port *port, bool isIn)
        {
            auto *ports = isIn ? &m_privateData->m_inPorts : &m_privateData->m_outPorts;

            if (std::find(ports->begin(), ports->end(), port) == ports->end())
            {
                ports->push_back(port);
                return true;
            }

            return false;
        }

        Port* Node::getInPort(size_t index) const
        {
            if (index >= m_privateData->m_inPorts.size())
            {
                return nullptr;
            }

            return m_privateData->m_inPorts[index];
        }

        Port* Node::getOutPort(size_t index) const
        {
            if (index >= m_privateData->m_outPorts.size())
            {
                return nullptr;
            }

            return m_privateData->m_outPorts[index];
        }

        size_t Node::getInPortCount() const
        {
            return m_privateData->m_inPorts.size();
        }

        size_t Node::getOutPortCount() const
        {
            return m_privateData->m_outPorts.size();
        }

        size_t Node::findPortIndex(Port *port, bool& has, bool& isIn)
        {
            has = true;

            for (size_t i = 0; i < m_privateData->m_inPorts.size(); i++)
            {
                auto*p = m_privateData->m_inPorts[i];

                if (p == port)
                {
                    isIn = true;
                    return i;
                }
            }

            for (size_t i = 0; i < m_privateData->m_outPorts.size(); i++)
            {
                auto*p = m_privateData->m_outPorts[i];

                if (p == port)
                {
                    isIn = false;
                    return i;
                }
            }

            has = false;
            return 0;
        }

        size_t Node::findInConnectionCount() const
        {
            size_t total = 0;

            for (size_t i = 0; i < getInPortCount(); i++)
            {
                auto *port = getInPort(i);
                total += port->getConnectionCount();
            }

            return total;
        }

        size_t Node::findOutConnectionCount() const
        {
            size_t total = 0;

            for (size_t i = 0; i < getOutPortCount(); i++)
            {
                auto *port = getOutPort(i);
                total += port->getConnectionCount();
            }

            return total;
        }

        bool Node::removePort(Port *port)
        {
            auto inIt = std::find(m_privateData->m_inPorts.begin(), m_privateData->m_inPorts.end(), port);

            if (inIt != m_privateData->m_inPorts.end())
            {
                m_privateData->m_inPorts.erase(inIt);
                return true;
            }

            auto outIt = std::find(m_privateData->m_outPorts.begin(), m_privateData->m_outPorts.end(), port);

            if (outIt != m_privateData->m_outPorts.end())
            {
                m_privateData->m_outPorts.erase(outIt);
                return true;
            }

            return false;
        }

        Port* Node::removePort(size_t index, bool isIn)
        {
            auto *ports = isIn ? &m_privateData->m_inPorts : &m_privateData->m_outPorts;

            if (index >= ports->size())
            {
                return nullptr;
            }

            Port* port = ports->at(index);
            ports->erase(ports->begin() + index);
            return port;
        }

    }
}
