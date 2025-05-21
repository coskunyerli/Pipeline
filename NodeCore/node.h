#include <vector>

namespace Pipeline
{
    namespace Core
    {
        class Port;
        class Node
        {
            public:
                explicit Node();
                virtual ~Node();
                virtual void run() = 0;
                virtual bool addPort(Port *port);
                virtual bool removePort(Port *port);
                virtual Port* removePort(size_t index);
            private:
                std::vector<Port*> m_ports;
        };
    }
}
