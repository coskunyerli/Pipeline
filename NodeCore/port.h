#pragma once
#include <vector>
#include "pipeline_core_dll.h"

namespace Pipeline
{
    namespace Core
    {
        class Connection;
        class Node;
        class PIPELINE_CORE_API Port
        {
            public:
                explicit Port(Node* node);
                virtual ~Port();
                virtual Connection* connect(Port *other);
                bool hasConnection() const;
                bool hasConnection(Port *other) const;
                bool hasConnection(Connection * connection) const;
                Connection* getConnection(size_t index) const;
                Connection* getConnection(Port *other) const;
                size_t findConnection(Connection *connection, bool& has) const;

                size_t getConnectionCount() const;
                Node* getOwnerNode() const;
                void addConnection(Connection* connection);
            private:
                Node* m_ownerNode;
                struct PrivateData;
                PrivateData* m_privateData;

        };
    }
}
