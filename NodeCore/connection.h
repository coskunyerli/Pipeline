#pragma once
#include "pipeline_core_dll.h"

namespace Pipeline
{
    namespace Core
    {
        class Port;
        class PIPELINE_CORE_API Connection
        {
            public:
                explicit Connection(Port* in, Port *out);
                virtual ~Connection() {}
                Port* getInPort() const
                {
                    return m_inPort;
                }
                Port* getOutPort() const
                {
                    return m_outPort;
                }
            private:
                Port* m_inPort;
                Port* m_outPort;
        };
    }
}
