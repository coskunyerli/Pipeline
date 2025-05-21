#include "connection.h"

namespace Pipeline
{
    namespace Core
    {

        Connection::Connection(Port *in, Port *out)
            : m_inPort(in)
            , m_outPort(out)
        {
        }

    }
}
