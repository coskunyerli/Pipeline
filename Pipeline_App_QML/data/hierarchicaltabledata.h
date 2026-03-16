#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
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
                std::string getValue() const;
                ValueType getValueType() const;
                size_t getChildCount() const
                {
                    return m_values.size();
                }
                std::string getHeaderData(int section) const;
                void setHeaderData(int section, const std::string& value);
                std::shared_ptr<HierarchicalTableData> getCell(size_t row, size_t column) const;
                std::string getCellValue(size_t row, size_t column) const;
                ValueType getCellValueType(size_t row, size_t column) const;
                std::shared_ptr<HierarchicalTableData> getOrCreateCell(size_t row, size_t column);
                std::shared_ptr<HierarchicalTableData> removeCell(size_t row, size_t column);
                HierarchicalTableData* getParent()
                {
                    return m_parent;
                }
                void setCell(size_t row, size_t column,  std::shared_ptr<HierarchicalTableData> child);
                void setCell(size_t index,  std::shared_ptr<HierarchicalTableData> child);
                void setCellValue(size_t row, size_t col, const std::string& v);
                std::pair<size_t, size_t> cellIndexOf(const std::shared_ptr<HierarchicalTableData>& child, bool &has) const;
                std::pair<size_t, size_t> cellIndexOf(const HierarchicalTableData* child, bool &has) const;
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


                struct CellKey
                {
                    size_t row;
                    size_t column;

                    bool operator==(const CellKey& other) const
                    {
                        return row == other.row && column == other.column;
                    }
                };

                struct CellKeyHash
                {
                    std::size_t operator()(const CellKey& k) const noexcept
                    {
                        return std::hash<size_t>()(k.row) ^ (std::hash<size_t>()(k.column) << 1);
                    }
                };

            private:
                std::unordered_map<int, std::string> m_headerData;
                std::string m_value;
                std::vector<std::string> m_values;
                std::unordered_map<CellKey, std::shared_ptr<HierarchicalTableData>, CellKeyHash> m_tables;
                size_t m_columnCount;
                size_t m_rowCount;
                HierarchicalTableData* m_parent;
        };
    }
}

