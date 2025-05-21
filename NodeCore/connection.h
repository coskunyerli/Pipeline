#pragma once

namespace Pipeline
{
    namespace Core
    {
        class Port;
        class Connection
        {
            public:
                explicit Connection(Port* in, Port *out);
            private:
                Port* m_inPort;
                Port* m_outPort;
        };
    }
}
