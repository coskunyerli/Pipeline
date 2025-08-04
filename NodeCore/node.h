#include <string>
#include "pipeline_core_dll.h"
namespace Pipeline
{
    namespace Core
    {
        class FlowNode;
        class Port;
        class PIPELINE_CORE_API Node
        {
            public:
                explicit Node();
                Node(const Node&other);
                virtual ~Node();
                virtual void run() {};
                FlowNode* parent() const;
                void setParent(FlowNode* parent);
                std::string getName() const;
                void setName(const std::string &name);
                float getX() const;
                float getY() const;
                void setX(float x);
                void setY(float y);
                virtual bool addPort(Port *port, bool isIn);
                virtual Port* getInPort(size_t index) const;
                virtual Port* getOutPort(size_t index) const;
                virtual size_t getInPortCount() const;
                virtual size_t getOutPortCount() const;
                size_t findPortIndex(Port* port, bool& has, bool& isIn);
                size_t findInConnectionCount() const;
                size_t findOutConnectionCount() const;
                virtual bool removePort(Port *port);
                virtual Port* removePort(size_t index, bool isIn);
            private:
                float m_x;
                float m_y;
                FlowNode* m_parent;
                struct PrivateData;
                PrivateData* m_privateData;
        };
    }
}
