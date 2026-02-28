#include "pythonnoderesult.h"

#include <stdexcept>

// -------------------------------------------------------
// Binary format layout:
//
//   [Header]
//     4 bytes  magic   "PNR1" = 0x504E5231
//     1 byte   version = 1
//
//   [Node]  (recursive)
//     1 byte   marker: 0=null, 1=present
//     --- if present ---
//     8 bytes  rowCount    (uint64, big-endian)
//     8 bytes  columnCount (uint64, big-endian)
//     4 bytes  valueType   (uint32, big-endian)
//     4 bytes  value string length
//     N bytes  value string data (UTF-8)
//     8 bytes  childCount  (uint64, big-endian)
//     [childCount × Node]  recursive children
// -------------------------------------------------------

namespace Pipeline
{
    namespace Runtime
    {

        namespace
        {

            // --- Write helpers (big-endian) ---

            void writeU8(std::vector<uint8_t>& buf, uint8_t val)
            {
                buf.push_back(val);
            }

            void writeU32(std::vector<uint8_t>& buf, uint32_t val)
            {
                buf.push_back(static_cast<uint8_t>((val >> 24) & 0xFF));
                buf.push_back(static_cast<uint8_t>((val >> 16) & 0xFF));
                buf.push_back(static_cast<uint8_t>((val >>  8) & 0xFF));
                buf.push_back(static_cast<uint8_t>((val) & 0xFF));
            }

            void writeU64(std::vector<uint8_t>& buf, uint64_t val)
            {
                for (int i = 56; i >= 0; i -= 8)
                    buf.push_back(static_cast<uint8_t>((val >> i) & 0xFF));
            }

            void writeString(std::vector<uint8_t>& buf, const std::string& str)
            {
                writeU32(buf, static_cast<uint32_t>(str.size()));
                buf.insert(buf.end(), str.begin(), str.end());
            }

            // --- Read helpers (big-endian) ---

            uint8_t readU8(const uint8_t* data, size_t size, size_t& offset)
            {
                if (offset + 1 > size)
                    throw std::runtime_error("PythonNodeResult::deserialize: buffer underflow (U8)");

                return data[offset++];
            }

            uint32_t readU32(const uint8_t* data, size_t size, size_t& offset)
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

            uint64_t readU64(const uint8_t* data, size_t size, size_t& offset)
            {
                if (offset + 8 > size)
                    throw std::runtime_error("PythonNodeResult::deserialize: buffer underflow (U64)");

                uint64_t val = 0;

                for (int i = 0; i < 8; i++)
                    val = (val << 8) | data[offset++];

                return val;
            }

            std::string readString(const uint8_t* data, size_t size, size_t& offset)
            {
                uint32_t len = readU32(data, size, offset);

                if (offset + len > size)
                    throw std::runtime_error("PythonNodeResult::deserialize: buffer underflow (string)");

                std::string str(reinterpret_cast<const char*>(data + offset), len);
                offset += len;
                return str;
            }

            // --- Recursive serialize ---

            void serializeNode(std::vector<uint8_t>& buf, const PythonNodeResult* node)
            {
                if (!node)
                {
                    writeU8(buf, 0);
                    return;
                }

                writeU8(buf, 1);
                size_t rows = node->getRowCount();
                size_t cols = node->getColumnCount();
                writeU64(buf, rows);
                writeU64(buf, cols);
                writeU32(buf, static_cast<uint32_t>(node->getValueType()));
                writeString(buf, node->getValue());
                size_t childCount = rows * cols;
                writeU64(buf, childCount);
                // Serialize each slot in row-major order
                auto* mutableNode = const_cast<PythonNodeResult*>(node);

                for (size_t r = 0; r < rows; r++)
                {
                    for (size_t c = 0; c < cols; c++)
                    {
                        serializeNode(buf, mutableNode->getCell(r, c));
                    }
                }
            }

            // --- Recursive deserialize ---

            PythonNodeResult* deserializeNode(const uint8_t* data, size_t size,
                                              size_t& offset, PythonNodeResult* parent)
            {
                uint8_t marker = readU8(data, size, offset);

                if (marker == 0)
                    return nullptr;

                auto* node = new PythonNodeResult(parent);
                uint64_t rows = readU64(data, size, offset);
                uint64_t cols = readU64(data, size, offset);
                auto valueType = static_cast<PythonNodeResult::ValueType>(readU32(data, size, offset));
                std::string value = readString(data, size, offset);
                node->setSize(static_cast<size_t>(rows), static_cast<size_t>(cols));
                node->setValueType(valueType);

                if (!value.empty())
                    node->setValue(value);

                uint64_t childCount = readU64(data, size, offset);

                for (uint64_t i = 0; i < childCount; i++)
                {
                    PythonNodeResult* child = deserializeNode(data, size, offset, node);

                    if (child)
                    {
                        size_t r = static_cast<size_t>(i / cols);
                        size_t c = static_cast<size_t>(i % cols);
                        node->setCell(r, c, child);
                    }
                }

                return node;
            }

        } // anonymous namespace


        PythonNodeResult::PythonNodeResult(PythonNodeResult* parent)
            : m_columnCount(0)
            , m_rowCount(0)
            , m_valueType(ValueType::None)
            , m_parent(parent)
        {
        }

        PythonNodeResult::~PythonNodeResult()
        {
            bool has;

            if (m_parent)
            {
                std::pair<size_t, size_t> cellIndex = m_parent->cellIndexOf(this, has);

                if (has)
                {
                    m_parent->removeCell(cellIndex.first, cellIndex.second);
                }
            }

            for (auto*child : m_data)
            {
                delete child;
            }

            m_data.clear();
        }

        void PythonNodeResult::setSize(size_t row, size_t col)
        {
            m_columnCount = col;
            m_rowCount = row;
            this->m_data.resize(row * col);
        }

        void PythonNodeResult::setValue(const std::string value)
        {
            this->m_value = value;
            this->m_valueType = static_cast<ValueType>(m_valueType | ValueType::Value);
        }

        PythonNodeResult* PythonNodeResult::getCell(size_t row, size_t column)
        {
            if (row >= this->m_rowCount || column >= this->m_columnCount)
            {
                return nullptr;
            }

            return m_data.at(this->mapFromCellIndex(row, column));
        }

        PythonNodeResult* PythonNodeResult::getOrCreateCell(size_t row, size_t column)
        {
            size_t index = this->mapFromCellIndex(row, column);

            if (index >= this->getChildCount())
            {
                return nullptr;
            }

            auto *cell = this->getCell(row, column);

            if (!cell)
            {
                cell = new PythonNodeResult();
                this->m_data[index] = cell;
            }

            return cell;
        }

        void PythonNodeResult::deleteCell(size_t row, size_t column)
        {
            auto * child = this->getCell(row, column);
            m_data.erase(m_data.begin() + this->mapFromCellIndex(row, column));

            if (child)
            {
                delete child;
            }
        }

        PythonNodeResult* PythonNodeResult::removeCell(size_t row, size_t column)
        {
            auto * child = this->getCell(row, column);
            m_data.erase(m_data.begin() + this->mapFromCellIndex(row, column));
            return child;
        }

        std::pair<size_t, size_t> PythonNodeResult::cellIndexOf(PythonNodeResult *child, bool &has) const
        {
            if (!child)
            {
                has = false;
                return std::make_pair(0, 0);
            }

            for (size_t i = 0; i < this->getChildCount(); i++)
            {
                if (this->m_data.at(i) == child)
                {
                    has = true;
                    return this->mapToCellIndex(i);
                }
            }

            has = false;
            return std::make_pair(0, 0);
        }

        std::vector<uint8_t> PythonNodeResult::serialize()
        {
            return PythonNodeResult::serialize(this);
        }

        std::vector<uint8_t> PythonNodeResult::serialize(const PythonNodeResult *node)
        {
            std::vector<uint8_t> buffer;
            buffer.reserve(4096);
            // Header
            writeU32(buffer, 0x504E5231); // "PNR1"
            writeU8(buffer, 1);            // version
            serializeNode(buffer, node);
            return buffer;
        }

        PythonNodeResult* PythonNodeResult::deserialize(const uint8_t* data, size_t size)
        {
            size_t offset = 0;
            uint32_t magic = readU32(data, size, offset);

            if (magic != 0x504E5231)
                throw std::runtime_error("PythonNodeResult::deserialize: invalid magic (expected PNR1)");

            uint8_t version = readU8(data, size, offset);

            if (version != 1)
                throw std::runtime_error("PythonNodeResult::deserialize: unsupported version");

            return deserializeNode(data, size, offset, nullptr);
        }

        PythonNodeResult* PythonNodeResult::deserialize(const std::vector<uint8_t>& buffer)
        {
            return deserialize(buffer.data(), buffer.size());
        }

        std::pair<size_t, size_t> PythonNodeResult::mapToCellIndex(size_t index) const
        {
            size_t row = index / m_columnCount;
            size_t column = index % m_columnCount;
            return std::make_pair(row, column);
        }

        size_t PythonNodeResult::mapFromCellIndex(size_t row, size_t column) const
        {
            return row * this->m_columnCount + column;
        }

        void PythonNodeResult::setCell(size_t row, size_t column, PythonNodeResult *child)
        {
            size_t index = mapFromCellIndex(row, column);

            if (index >= m_data.size())
                return;

            // Delete existing child at this slot if any
            if (m_data[index])
            {
                // Detach parent to avoid double-remove in child's destructor
                m_data[index]->m_parent = nullptr;
                delete m_data[index];
            }

            m_data[index] = child;

            if (child)
                child->m_parent = this;
        }

        void PythonNodeResult::setValueType(ValueType vt)
        {
            m_valueType = vt;
        }

    }
}
