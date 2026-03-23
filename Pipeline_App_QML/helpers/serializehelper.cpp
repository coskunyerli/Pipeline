#include "serializehelper.h"
#include <stdexcept>
namespace Pipeline::Runtime
{

    void SerializeHelper::writeU8(std::vector<uint8_t>& buf, uint8_t val)
    {
        buf.push_back(val);
    }

    void SerializeHelper::writeU32(std::vector<uint8_t>& buf, uint32_t val)
    {
        buf.push_back(static_cast<uint8_t>((val >> 24) & 0xFF));
        buf.push_back(static_cast<uint8_t>((val >> 16) & 0xFF));
        buf.push_back(static_cast<uint8_t>((val >>  8) & 0xFF));
        buf.push_back(static_cast<uint8_t>((val) & 0xFF));
    }

    void SerializeHelper::writeU64(std::vector<uint8_t>& buf, uint64_t val)
    {
        for (int i = 56; i >= 0; i -= 8)
            buf.push_back(static_cast<uint8_t>((val >> i) & 0xFF));
    }

    void SerializeHelper::writeFloat(std::vector<uint8_t> &buf, float val)
    {
        uint32_t u;
        std::memcpy(&u, &val, sizeof(float)); // bit copy

        buf.push_back((u >> 24) & 0xFF);
        buf.push_back((u >> 16) & 0xFF);
        buf.push_back((u >> 8) & 0xFF);
        buf.push_back(u & 0xFF);
    }
    void SerializeHelper::writeString(std::vector<uint8_t>& buf, const std::string &str)
    {
        writeU32(buf, static_cast<uint32_t>(str.size()));
        buf.insert(buf.end(), str.begin(), str.end());
    }

    uint8_t SerializeHelper::readU8(const uint8_t *data, size_t size, size_t &offset)
    {
        if (offset + 1 > size)
            throw std::runtime_error("PythonNodeResult::deserialize: buffer underflow (U8)");

        return data[offset++];
    }

    uint32_t SerializeHelper::readU32(const uint8_t *data, size_t size, size_t &offset)
    {
        if (offset + 4 > size)
            throw std::runtime_error("PythonNodeResult::deserialize: buffer underflow (U32)");

        uint32_t val = (static_cast<uint32_t>(data[offset])     << 24)
                       | (static_cast<uint32_t>(data[offset + 1]) << 16)
                       | (static_cast<uint32_t>(data[offset + 2]) <<  8)
                       | (static_cast<uint32_t>(data[offset + 3]));
        offset += 4;
        return val;
    }

    uint64_t SerializeHelper::readU64(const uint8_t *data, size_t size, size_t &offset)
    {
        if (offset + 8 > size)
            throw std::runtime_error("PythonNodeResult::deserialize: buffer underflow (U64)");

        uint64_t val = 0;

        for (int i = 0; i < 8; i++)
            val = (val << 8) | data[offset++];

        return val;
    }

    std::string SerializeHelper::readString(const uint8_t *data, size_t size, size_t &offset)
    {
        uint32_t len = readU32(data, size, offset);

        if (offset + len > size)
            throw std::runtime_error("PythonNodeResult::deserialize: buffer underflow (string)");

        std::string str(reinterpret_cast<const char*>(data + offset), len);
        offset += len;
        return str;
    }

    float SerializeHelper::readFloat(const uint8_t *data, size_t size, size_t &offset)
    {
        if (offset + 4 > size)
            throw std::runtime_error("Float Buffer overflow");

        uint32_t u = 0;
        u |= (data[offset++] << 24);
        u |= (data[offset++] << 16);
        u |= (data[offset++] << 8);
        u |= (data[offset++]);

        float val;
        std::memcpy(&val, &u, sizeof(float));

        return val;
    }

}
