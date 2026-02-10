#include "pythonnoderesult.h"

namespace Pipeline
{
    namespace Runtime
    {

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
            if(m_parent)
            {
                std::pair<size_t,size_t> cellIndex = m_parent->cellIndexOf(this,has);
                if(has)
                {
                    m_parent->removeCell(cellIndex.first,cellIndex.second);
                }
            }

            for(auto*child:m_data)
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
            this->m_valueType = ValueType(m_valueType & ValueType::Value);
        }

        PythonNodeResult *PythonNodeResult::getCell(size_t row, size_t column)
        {
            if(row >= this->m_rowCount || column >= this->m_columnCount)
            {
                return nullptr;
            }

            return m_data.at(this->mapFromCellIndex(row, column));
        }

        PythonNodeResult *PythonNodeResult::getOrCreateCell(size_t row, size_t column)
        {
            size_t index = this->mapFromCellIndex(row,column);
            if(index >= this->getChildCount())
            {
                return nullptr;
            }

            auto *cell = this->getCell(row, column);
            if(!cell)
            {
                cell = new PythonNodeResult();
                this->m_data[index] = cell;
            }
            return cell;
        }

        void PythonNodeResult::deleteCell(size_t row, size_t column)
        {
            auto * child = this->getCell(row,column);
            m_data.erase(m_data.begin() + this->mapFromCellIndex(row,column));
            if(child)
            {
                delete child;
            }
        }

        PythonNodeResult *PythonNodeResult::removeCell(size_t row, size_t column)
        {
            auto * child = this->getCell(row,column);
            m_data.erase(m_data.begin() + this->mapFromCellIndex(row,column));
            return child;
        }

        std::pair<size_t, size_t> PythonNodeResult::cellIndexOf(PythonNodeResult *child, bool &has) const
        {
            if(!child)
            {
                has = false;
                return std::make_pair(0,0);
            }
            for(size_t i = 0; i < this->getChildCount(); i++)
            {
                if(this->m_data.at(i) == child)
                {
                    has = true;
                    return this->mapToCellIndex(i);
                }
            }

            has = false;
            return std::make_pair(0,0);
        }

        std::pair<size_t, size_t> PythonNodeResult::mapToCellIndex(size_t index) const
        {
            size_t row = index / this->m_rowCount;
            size_t column = index - (row * this->m_rowCount);
            return std::make_pair(row, column);
        }

        size_t PythonNodeResult::mapFromCellIndex(size_t row, size_t column) const
        {
            return row * this->m_rowCount + column;
        }

    }
}
