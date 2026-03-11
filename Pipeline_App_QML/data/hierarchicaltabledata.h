#pragma once
#include <string>
#include <vector>
#include <unordered_map>
namespace Pipeline
{
    namespace Runtime
    {
        class HierarchicalTableData
        {
            public:
                enum ValueType : int
                {
                    None = 0,
                    Value = 1,
                    Matrix = 2,
                    All = Value | Matrix
                };

                explicit HierarchicalTableData(HierarchicalTableData* parent = nullptr);
                virtual ~HierarchicalTableData();
                void setSize(size_t row, size_t col);
                size_t getColumnCount() const
                {
                    return m_columnCount;
                }
                size_t getRowCount() const
                {
                    return m_rowCount;
                }
                void setValue(const std::string value);
                std::string getValue() const
                {
                    return m_value;
                }
                ValueType getValueType() const
                {
                    return m_valueType;
                }
                size_t getChildCount() const
                {
                    return m_data.size();
                }
                std::string getHeaderData(int section) const;
                void setHeaderData(int section, const std::string& value);
                HierarchicalTableData* getCell(size_t row, size_t column);
                HierarchicalTableData* getOrCreateCell(size_t row, size_t column);
                void deleteCell(size_t row, size_t column);
                HierarchicalTableData* removeCell(size_t row, size_t column);
                HierarchicalTableData* getParent()
                {
                    return m_parent;
                }
                void setCell(size_t row, size_t column, HierarchicalTableData* child);
                void setValueType(ValueType vt);
                std::pair<size_t, size_t> cellIndexOf(HierarchicalTableData* child, bool &has) const;
                std::vector<uint8_t> serialize();
                HierarchicalTableData* copy() const;
                static std::vector<uint8_t> serialize(const HierarchicalTableData* node);
                static HierarchicalTableData* deserialize(const uint8_t* data, size_t size);
                static HierarchicalTableData* deserialize(const std::vector<uint8_t>& buffer);
            private:
                std::pair<size_t, size_t> mapToCellIndex(size_t index) const;
                size_t mapFromCellIndex(size_t row, size_t column) const;
                static void serializeNode(std::vector<uint8_t>& buf, const HierarchicalTableData* node);
                static HierarchicalTableData* deserializeNode(const uint8_t* data, size_t size,
                        size_t& offset, HierarchicalTableData* parent);
            private:
                std::string m_value;
                std::unordered_map<int, std::string> m_headerData;
                std::vector<HierarchicalTableData*> m_data;
                size_t m_columnCount;
                size_t m_rowCount;
                ValueType m_valueType;
                HierarchicalTableData* m_parent;
        };
    }
}

