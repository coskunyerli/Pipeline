#include "port.h"
#include "connection.h"
#include "node.h"

namespace Pipeline
{
    namespace Core
    {
        struct Port::PrivateData
        {
            std::vector<Connection*> m_connections;
        };

        Port::Port(Node* node)
            : m_privateData(new PrivateData())
        {
            m_ownerNode = node;
        }

        Port::~Port()
        {
            delete m_privateData;
        }

        Connection* Port::connect(Port *other)
        {
            if (hasConnection(other))
            {
                return nullptr;
            }

            auto *connection = new Connection(other, this);
            addConnection(connection);
            other->addConnection(connection);
            return connection;
        }

        bool Port::hasConnection() const
        {
            return !this->m_privateData->m_connections.empty();
        }

        bool Port::hasConnection(Port *other) const
        {
            return getConnection(other) != nullptr;
        }

        Connection* Port::getConnection(size_t index) const
        {
            if (index >= m_privateData->m_connections.size())
            {
                return nullptr;
            }

            return m_privateData->m_connections[index];
        }

        Connection* Port::getConnection(Port *other) const
        {
            for (auto *connection : m_privateData->m_connections)
            {
                if (!connection)
                {
                    continue;
                }

                if ((connection->getInPort() == this && connection->getOutPort() == other) ||
                    (connection->getOutPort() == this && connection->getInPort() == other))
                {
                    if (other->hasConnection(connection))
                    {
                        return connection;
                    }
                }
            }

            return nullptr;
        }

        size_t Port::findConnection(Connection *connection, bool &has) const
        {
            has = false;

            for (size_t i = 0; i < m_privateData->m_connections.size(); i++)
            {
                auto *c = m_privateData->m_connections[i];

                if (c == connection)
                {
                    has = true;
                    return i;
                }
            }

            return false;
        }

        bool Port::hasConnection(Connection *connection) const
        {
            for (auto *c : m_privateData->m_connections)
            {
                if (c == connection)
                {
                    return true;
                }
            }

            return false;
        }

        size_t Port::getConnectionCount() const
        {
            return m_privateData->m_connections.size();
        }

        Node* Port::getOwnerNode() const
        {
            return m_ownerNode;
        }

        void Port::addConnection(Connection *connection)
        {
            m_privateData->m_connections.push_back(connection);
        }
    }
}
