#pragma once
#include <vector>
#include <string>
namespace Pipeline::Runtime
{
    class SerializeHelper
    {
        public:
            static void writeU8(std::vector<uint8_t>& buf, uint8_t val);
            static void writeU32(std::vector<uint8_t>& buf, uint32_t val);
            static void writeU64(std::vector<uint8_t>& buf, uint64_t val);
            static void writeFloat(std::vector<uint8_t>& buf, float val);
            static void writeString(std::vector<uint8_t>& buf, const std::string& str);

            // --- Read helpers (big-endian) ---

            static uint8_t readU8(const uint8_t* data, size_t size, size_t& offset);
            static uint32_t readU32(const uint8_t* data, size_t size, size_t& offset);
            static uint64_t readU64(const uint8_t* data, size_t size, size_t& offset);
            static std::string readString(const uint8_t* data, size_t size, size_t& offset);
            static float readFloat(const uint8_t *data, size_t size, size_t &offset);
    };
}



// --- Write helpers (big-endian) ---

