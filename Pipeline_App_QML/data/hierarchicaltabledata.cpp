#include "hierarchicaltabledata.h"

#include <stdexcept>

#include <helpers/stringhelper.h>

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
//     4 bytes  value string length
//     4 bytes  header size length
//     4 bytes  header column number
//     N bytes  header string data (UTF-8)
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

        } // anonymous namespace


        HierarchicalTableData::HierarchicalTableData(HierarchicalTableData* parent)
            : m_columnCount(0)
            , m_rowCount(0)
            , m_parent(parent)
        {
        }

        HierarchicalTableData::~HierarchicalTableData()
        {
            bool has;

            for(auto& childTable : m_tables)
            {
                childTable.second->m_parent = nullptr;
            }

            if (m_parent)
            {
                std::pair<size_t, size_t> cellIndex = m_parent->cellIndexOf(this, has);

                if (has)
                {
                    m_parent->removeCell(cellIndex.first, cellIndex.second);
                }
            }

            m_tables.clear();
        }

        void HierarchicalTableData::setSize(size_t row, size_t col)
        {
            m_columnCount = col;
            m_rowCount = row;
            this->m_values.resize(row * col);
        }

        void HierarchicalTableData::setValue(const std::string value)
        {
            if (!m_parent)
            {
                return;
            }

            bool has = false;
            auto index = m_parent->cellIndexOf(this, has);

            if (!has)
            {
                return;
            }

            m_parent->setCellValue(index.first, index.second, value);
        }

        std::string HierarchicalTableData::getValue() const
        {
            if (!m_parent)
            {
                return "";
            }

            bool has = false;
            auto index = m_parent->cellIndexOf(this, has);

            if (!has)
            {
                return "";
            }

            return m_parent->getCellValue(index.first, index.second);
        }

        HierarchicalTableData::ValueType HierarchicalTableData::getValueType() const
        {
            if (!m_parent)
            {
                return ValueType::None;
            }

            bool has = false;
            auto index = m_parent->cellIndexOf(this, has);

            if (!has)
            {
                return ValueType::None;
            }

            return this->getCellValueType(index.first, index.second);
        }

        std::string HierarchicalTableData::getHeaderData(int section) const
        {
            auto it = m_headerData.find(section);

            if (it != m_headerData.end())
            {
                return it->second;
            }

            return StringHelper::indexToString(section);
        }

        void HierarchicalTableData::setHeaderData(int section, const std::string& value)
        {
            m_headerData[section] = value;
        }

        std::shared_ptr<HierarchicalTableData> HierarchicalTableData::getCell(size_t row, size_t column) const
        {
            CellKey key = {row, column};
            auto it = m_tables.find(key);

            if (it == m_tables.end())
            {
                return nullptr;
            }

            return it->second;
        }

        std::string HierarchicalTableData::getCellValue(size_t row, size_t column) const
        {
            size_t index = this->mapFromCellIndex(row, column);

            if (index >= this->getChildCount())
            {
                return "";
            }

            return m_values[index];
        }

        HierarchicalTableData::ValueType HierarchicalTableData::getCellValueType(size_t row, size_t column) const
        {
            ValueType type = None;
            auto value = this->getCellValue(row, column);

            if (!value.empty())
            {
                type =  static_cast<ValueType>(type | ValueType::Value);
            }

            auto cell = this->getCell(row, column);

            if (cell)
            {
                type =  static_cast<ValueType>(type | ValueType::Matrix);
            }

            return type;
        }

        std::shared_ptr<HierarchicalTableData> HierarchicalTableData::getOrCreateCell(size_t row, size_t column)
        {
            auto cell = this->getCell(row, column);

            if (!cell)
            {
                this->setCell(row, column, new HierarchicalTableData());
                cell = this->getCell(row, column);
            }

            return cell;
        }

        std::shared_ptr<HierarchicalTableData> HierarchicalTableData::removeCell(size_t row, size_t column)
        {
            CellKey key = {row, column};
            auto it = m_tables.find(key);

            if (it == m_tables.end())
            {
                return nullptr;
            }

            auto removedCell = it->second;
            m_tables.erase(key);
            return removedCell;
        }

        std::pair<size_t, size_t> HierarchicalTableData::cellIndexOf(const std::shared_ptr<HierarchicalTableData>& child, bool &has) const
        {
            return this->cellIndexOf(child.get(), has);
        }

        std::vector<uint8_t> HierarchicalTableData::serialize()
        {
            return HierarchicalTableData::serialize(this);
        }

        HierarchicalTableData* HierarchicalTableData::copy() const
        {
            // TODOJ Bu daha hızlı yapılabilir, şimdilik bu böyle olsun
            auto data = HierarchicalTableData::serialize(this);
            return HierarchicalTableData::deserialize(data);
        }

        std::vector<uint8_t> HierarchicalTableData::serialize(const HierarchicalTableData *node)
        {
            std::vector<uint8_t> buffer;
            buffer.reserve(4096);
            // Header
            writeU32(buffer, 0x504E5231); // "PNR1"
            writeU8(buffer, 1);            // version
            serializeNode(buffer, node);
            return buffer;
        }

        HierarchicalTableData* HierarchicalTableData::deserialize(const uint8_t* data, size_t size)
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

        HierarchicalTableData* HierarchicalTableData::deserialize(const std::vector<uint8_t>& buffer)
        {
            return deserialize(buffer.data(), buffer.size());
        }

        std::pair<size_t, size_t> HierarchicalTableData::cellIndexOf(const HierarchicalTableData *child, bool &has) const
        {
            if (!child)
            {
                has = false;
                return std::make_pair(0, 0);
            }

            for (auto &pair : m_tables)
            {
                if (pair.second.get() == child)
                {
                    has = true;
                    return std::make_pair(pair.first.row, pair.first.column);
                }
            }

            has = false;
            return std::make_pair(0, 0);
        }

        std::pair<size_t, size_t> HierarchicalTableData::mapToCellIndex(size_t index) const
        {
            size_t row = index / m_columnCount;
            size_t column = index % m_columnCount;
            return std::make_pair(row, column);
        }

        size_t HierarchicalTableData::mapFromCellIndex(size_t row, size_t column) const
        {
            return row * this->m_columnCount + column;
        }

        void HierarchicalTableData::serializeNode(std::vector<uint8_t>& buf, const HierarchicalTableData *node)
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
            writeU32(buf, static_cast<uint32_t>(node->m_headerData.size()));

            for (const auto& pair : node->m_headerData)
            {
                writeU32(buf, static_cast<uint32_t>(pair.first));
                writeString(buf, pair.second);
            }

            // -------- VALUES (SPARSE) --------
            std::vector<std::pair<size_t, std::string>> nonEmptyValues;
            for (size_t i = 0; i < node->m_values.size(); i++)
            {
                if (!node->m_values[i].empty())
                    nonEmptyValues.emplace_back(i, node->m_values[i]);
            }
            writeU64(buf, nonEmptyValues.size());
            for (auto& pair : nonEmptyValues)
            {
                size_t row = pair.first / cols;
                size_t col = pair.first % cols;
                writeU64(buf, row);
                writeU64(buf, col);
                writeString(buf, pair.second);
            }
            // Serializer Child Table
            size_t childCount = node->m_tables.size();
            writeU64(buf, childCount);
            for (auto& pair : node->m_tables)
            {
                writeU64(buf, pair.first.row);
                writeU64(buf, pair.first.column);
                serializeNode(buf, pair.second.get());
            }
        }

        HierarchicalTableData* HierarchicalTableData::deserializeNode(const uint8_t* data, size_t size, size_t& offset, HierarchicalTableData *parent)
        {
            uint8_t marker = readU8(data, size, offset);
            if (marker == 0)
                return nullptr;
            auto* node = new HierarchicalTableData(parent);
            uint64_t rows = readU64(data, size, offset);
            uint64_t cols = readU64(data, size, offset);
            node->setSize(static_cast<size_t>(rows), static_cast<size_t>(cols));
            uint32_t headerCount = readU32(data, size, offset);

            for (uint32_t i = 0; i < headerCount; i++)
            {
                int key = static_cast<int>(readU32(data, size, offset));
                std::string value = readString(data, size, offset);
                node->setHeaderData(key, value);
            }

            uint64_t valueSize = readU64(data, size, offset);

            for (size_t i = 0; i < valueSize; i++)
            {
                uint64_t row = readU64(data, size, offset);
                uint64_t column = readU64(data, size, offset);
                std::string value = readString(data, size, offset);
                node->setCellValue(row, column, value);
            }

            uint64_t childCount = readU64(data, size, offset);

            for (uint64_t i = 0; i < childCount; i++)
            {
                HierarchicalTableData* child = deserializeNode(data, size, offset, node);

                if (child)
                {
                    uint64_t row = readU64(data, size, offset);
                    uint64_t column = readU64(data, size, offset);
                    node->setCell(row, column, child);
                }
            }

            return node;
        }

        void HierarchicalTableData::setCell(size_t row, size_t column, HierarchicalTableData *child)
        {
            CellKey key = {row, column};
            auto it = m_tables.find(key);

            if (it != m_tables.end())
            {
                it->second->m_parent = nullptr;
            }

            m_tables[key] = std::shared_ptr<HierarchicalTableData>(child);

            if (child)
                child->m_parent = this;
        }

        void HierarchicalTableData::setCellValue(size_t row, size_t col, const std::string &v)
        {
            size_t idx = mapFromCellIndex(row, col);

            if (idx >= this->getChildCount())
                return;

            m_values[idx] = v;
        }

    }
}
