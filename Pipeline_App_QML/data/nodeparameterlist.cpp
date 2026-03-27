#include "nodeparameterlist.h"
#include <helpers/serializehelper.h>
#include <stdexcept>

namespace Pipeline::Runtime
{

    NodeParameterList::NodeParameterList()
    {
    }

    void NodeParameterList::addParameter(const std::string &name, int type, const PVariant &value)
    {
        NodeParameter np;
        np.name = name;
        np.type = static_cast<ParamType>(type);
        np.value = value;
        m_params.emplace(name, np);
        m_paramsKeys.push_back(name);
    }

    int NodeParameterList::getParameterIndex(const std::string &name) const
    {
        for (int i = 0; i < static_cast<int>(m_params.size()); i++)
        {
            auto &paramName = m_paramsKeys[i];

            if (paramName == name)
            {
                return i;
            }
        }

        return -1;
    }

    std::string NodeParameterList::getParameterName(size_t index) const
    {
        if (index >= this->paramSize())
        {
            return "";
        }

        return m_paramsKeys[index];
    }

    void NodeParameterList::removeParameter(int index)
    {
        if (index < 0 || index >= m_paramsKeys.size()) return;

        auto name = getParameterName(index);
        m_params.erase(name);
        m_paramsKeys.erase(m_paramsKeys.begin() + index);
    }

    void NodeParameterList::removeParameter(const std::string &name)
    {
        auto it = m_params.find(name);

        if (it == this->m_params.end())
        {
            return;
        }

        int index = this->getParameterIndex(name);

        if (index != -1)
        {
            this->removeParameter(index);
        }
        else
        {
            m_params.erase(name);
        }
    }

    std::vector<uint8_t> NodeParameterList::serialize(std::vector<uint8_t>& buffer) const
    {
        // version
        uint32_t version = 1;
        SerializeHelper::writeU8(buffer, version);
        uint32_t count = static_cast<uint32_t>(m_paramsKeys.size());
        SerializeHelper::writeU32(buffer, count);

        for (const auto& key : m_paramsKeys)
        {
            const auto& p = m_params.at(key);
            SerializeHelper::writeString(buffer, key);
            int type = static_cast<int>(p.type);
            SerializeHelper::writeU8(buffer, type);

            switch (p.type)
            {
                case ParamType::Int:
                    {
                        int v = p.value.toInt();
                        SerializeHelper::writeU32(buffer, v);
                        break;
                    }

                case ParamType::Float:
                    {
                        float v = p.value.toFloat();
                        SerializeHelper::writeFloat(buffer, v);
                        break;
                    }

                case ParamType::Bool:
                    {
                        bool v = p.value.toBool();
                        SerializeHelper::writeU8(buffer, v);
                        break;
                    }

                case ParamType::String:
                case ParamType::Browse:
                    {
                        SerializeHelper::writeString(buffer, p.value.toString());
                        break;
                    }

                case ParamType::List:
                    {
                        // uint32_t listSize = static_cast<uint32_t>(p.listValues.size());
                        // SerializeHelper::writeU32(buffer, listSize);
                        // for (const auto& v : p.listValues)
                        // {
                        //     SerializeHelper::writeString(buffer, v.toString().toStdString());
                        // }
                        break;
                    }

                case ParamType::Table:
                    {
                        //uint32_t childCount = static_cast<uint32_t>(p.children.size());
                        //writeBinary(buffer, childCount);
                        // TODO: recursive serialize
                        break;
                    }
            }
        }

        return buffer;
    }

    NodeParameter& NodeParameterList::accessParameter(const std::string &name, bool &has)
    {
        auto it = m_params.find(name);

        if (it == m_params.end())
        {
            has = false;
            return m_invalid;
        }

        has = true;
        return it->second;
    }

    const NodeParameter& NodeParameterList::getParameter(size_t index, bool &has) const
    {
        if (index >= this->paramSize())
        {
            has = false;
            return m_invalid;
        }

        auto paramName = getParameterName(index);
        return this->getParameter(paramName, has);
    }

    const NodeParameter& NodeParameterList::getParameter(const std::string &name, bool &has) const
    {
        auto it = m_params.find(name);

        if (it == m_params.end())
        {
            has = false;
            return m_invalid;
        }

        has = true;
        return it->second;
    }

    bool NodeParameterList::updateParameterName(const std::string &parameterName, const std::string &newParameterName)
    {
        int index = this->getParameterIndex(parameterName);

        if (index == -1)
        {
            return false;
        }

        auto it = m_params.find(parameterName);

        if (it != m_params.end())
        {
            NodeParameter val = std::move(it->second);
            val.name = newParameterName;
            m_params.erase(it);
            m_params.emplace(newParameterName, std::move(val));
        }

        m_paramsKeys[index] = newParameterName;
        return true;
    }

    void NodeParameterList::deserialize(const uint8_t* buffer, size_t size)
    {
        size_t offset = 0;
        m_params.clear();
        m_paramsKeys.clear();

        try
        {
            uint32_t version =  SerializeHelper::readU8(buffer, size, offset);

            if (version != 1)
            {
                throw std::runtime_error("NodeParamListModel::deserialize: unsupported version");
            }

            uint32_t count = SerializeHelper::readU32(buffer, size, offset);

            for (uint32_t i = 0; i < count; ++i)
            {
                std::string key = SerializeHelper::readString(buffer, size, offset);
                int typeInt = SerializeHelper::readU8(buffer, size, offset);
                ParamType type = static_cast<ParamType>(typeInt);
                NodeParameter p;
                p.name = key;
                p.type = type;

                switch (type)
                {
                    case ParamType::Int:
                        {
                            int v = static_cast<int32_t>(SerializeHelper::readU32(buffer, size, offset));
                            p.value = v;
                            break;
                        }

                    case ParamType::Float:
                        {
                            float v = SerializeHelper::readFloat(buffer, size, offset);
                            p.value = v;
                            break;
                        }

                    case ParamType::Bool:
                        {
                            bool v = SerializeHelper::readU8(buffer, size, offset);
                            p.value = v;
                            break;
                        }

                    case ParamType::String:
                    case ParamType::Browse:
                        {
                            p.value = SerializeHelper::readString(buffer, size, offset);
                            break;
                        }

                    case ParamType::List:
                        {
                            // uint32_t listSize = SerializeHelper::readU32(buffer, size, offset);
                            // for (uint32_t j = 0; j < listSize; ++j)
                            // {
                            //     p.listValues.push_back(QString::fromStdString(SerializeHelper::readString(buffer, size, offset)));
                            // }
                            break;
                        }

                    case ParamType::Table:
                        {
                            //uint32_t childCount;
                            //readBinary(buffer, offset, childCount);
                            // TODO: recursive deserialize
                            break;
                        }
                }

                m_params.emplace(key, p);
                m_paramsKeys.push_back(key);
            }
        }
        catch (std::runtime_error &exception)
        {
        }
    }

    std::string NodeParameterList::paramTypeToString(ParamType t) const
    {
        switch (t)
        {
            case ParamType::Int:
                return "Int";

            case ParamType::Float:
                return "Float";

            case ParamType::Bool:
                return "Bool";

            case ParamType::String:
                return "String";

            case ParamType::List:
                return "List";

            case ParamType::Table:
                return "Table";

            case ParamType::Browse:
                return "Browse";
        }

        return "Unknown";
    }

}
