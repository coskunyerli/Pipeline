#include "port.h"
#include "connection.h"

namespace Pipeline
{
    namespace Core
    {

        Port::Port()
            : m_connection(nullptr)
        {
        }

        Connection* Port::connect(Port *other)
        {
            m_connection = new Connection(this, other);
            return m_connection;
        }

        bool Port::hasConnection() const
        {
            return this->m_connection != nullptr;
        }

    }
}
