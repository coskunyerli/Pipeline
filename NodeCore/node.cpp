#include "node.h"
namespace Pipeline
{
    namespace Core
    {

        Node::Node()
        {
        }

        Node::~Node()
        {
        }

        bool Node::addPort(Port *port)
        {
            if (std::find(this->m_ports.begin(), this->m_ports.end(), port) == m_ports.end())
            {
                this->m_ports.push_back(port);
                return true;
            }

            return false;
        }

        bool Node::removePort(Port *port)
        {
            size_t index = 0;
            bool has = false;

            for (auto*p : m_ports)
            {
                if (p == port)
                {
                    has = true;
                    break;
                }

                index++;
            }

            if (has)
            {
                removePort(index);
            }

            return has;
        }

        Port* Node::removePort(size_t index)
        {
            if (index >= this->m_ports.size())
            {
                return nullptr;
            }

            Port* port = m_ports[index];
            this->m_ports.erase(this->m_ports.begin() + index);
            return port;
        }

    }
}
