#pragma once
#include <string>
#include <vector>

namespace Pipeline
{
    namespace Runtime
    {        
        class PythonNodeResult
        {
        public:
            enum ValueType : int
            {
                None = 0,
                Value = 1,
                Matrix = 2,
                All = Value | Matrix
            };

            explicit PythonNodeResult(PythonNodeResult* parent = nullptr);
            virtual ~PythonNodeResult();
            void setSize(size_t row, size_t col);
            size_t getColumnCount() const { return m_columnCount; }
            size_t getRowCount() const { return m_rowCount; }
            void setValue(const std::string value);
            std::string getValue() const { return m_value; }
            ValueType getValueType() const { return m_valueType; }
            size_t getChildCount() const { return m_data.size(); }
            PythonNodeResult * getCell(size_t row, size_t column);
            PythonNodeResult * getOrCreateCell(size_t row, size_t column);
            void deleteCell(size_t row, size_t column);
            PythonNodeResult * removeCell(size_t row, size_t column);
            PythonNodeResult* getParent() { return m_parent; }
            std::pair<size_t, size_t> cellIndexOf(PythonNodeResult* child, bool &has) const;
        private:
            std::pair<size_t,size_t> mapToCellIndex(size_t index) const;
            size_t mapFromCellIndex(size_t row, size_t column) const;
        private:
            std::string m_value;
            std::vector<PythonNodeResult*> m_data;
            size_t m_columnCount;
            size_t m_rowCount;
            ValueType m_valueType;
            PythonNodeResult * m_parent;
        };
    }
}

