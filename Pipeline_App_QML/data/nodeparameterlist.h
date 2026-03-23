#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <data/nodeparameter.h>

namespace Pipeline::Runtime
{
    class NodeParameterList
    {
        public:
            explicit NodeParameterList();
            void addParameter(const std::string& name, int type, const PVariant& value = {});
            int getParameterIndex(const std::string &name) const;
            std::string getParameterName(size_t index) const;
            void removeParameter(int index);
            void removeParameter(const std::string& name);
            std::vector<uint8_t> serialize(std::vector<uint8_t>& output) const;
            NodeParameter& aceessParameter(const std::string& name, bool &has);

            const NodeParameter& getParameter(size_t index, bool &has) const ;
            const NodeParameter& getParameter(const std::string& name, bool &has) const;
            void deserialize(const uint8_t* data, size_t size);
            size_t paramSize() const
            {
                return m_params.size();
            }
            std::string paramTypeToString(ParamType t) const;

        private:
            std::unordered_map<std::string, NodeParameter> m_params;
            std::vector<std::string> m_paramsKeys;
            NodeParameter m_invalid;
    };
}
