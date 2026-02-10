#include "models/nodetablemodel.h"

namespace Pipeline
{
    namespace Runtime
    {

        NodeTableModel::NodeTableModel(QObject *parent)
        : QAbstractItemModel(parent)
        {
            m_rootResult = new PythonNodeResult();
            m_rootResult->setSize(10,10);
        }

        NodeTableModel::~NodeTableModel()
        {
             delete m_rootResult;
        }

        int NodeTableModel::rowCount(const QModelIndex &parent) const
        {
            PythonNodeResult* nodeResult;

            if (!parent.isValid())
            {
                nodeResult = this->m_rootResult;
            }
            else
            {
                nodeResult = getResultNode(parent);
            }

            if (nodeResult)
            {
                return static_cast<int>(nodeResult->getRowCount());
            }

            return 0;
        }

        int NodeTableModel::columnCount(const QModelIndex &parent) const
        {
            PythonNodeResult* nodeResult;

            if (!parent.isValid())
            {
                nodeResult = this->m_rootResult;
            }
            else
            {
                nodeResult = getResultNode(parent);
            }

            if (nodeResult)
            {
                return static_cast<int>(nodeResult->getColumnCount());
            }

            return 0;
        }

        QModelIndex NodeTableModel::index(int row, int column, const QModelIndex &parent) const
        {
            auto* parentNode = m_rootResult;

            if (parent.isValid())
            {
                parentNode = getResultNode(parent);
            }

            if (!parentNode)
            {
                return QModelIndex();
            }

            return createIndex(row, column, parentNode->getCell(row,column));
        }

        QModelIndex NodeTableModel::parent(const QModelIndex &child) const
        {
            if (!child.isValid())
            {
                return QModelIndex();
            }
            auto* childNode = getResultNode(child);
            if(!childNode)
            {
                return QModelIndex();
            }

            auto* parentNode = childNode->getParent();

            if (!parentNode)
            {
                return QModelIndex();
            }

            if (!parentNode->getParent())
            {
                return QModelIndex();
            }

            bool has;
            std::pair<size_t,size_t> parentCellIndex = parentNode->getParent()->cellIndexOf(parentNode, has);

            if (has)
            {
                return createIndex(static_cast<int>(parentCellIndex.first), static_cast<int>(parentCellIndex.second), parentNode);
            }

            return QModelIndex();
        }

        QVariant NodeTableModel::data(const QModelIndex &index, int role) const
        {
            if(!index.isValid())
            {
                return {};
            }

            PythonNodeResult* resultNode = getResultNode(index);
            if(!resultNode)
            {
                return {};
            }

            if(role == Qt::DisplayRole)
            {
                return QString::fromStdString(resultNode->getValue());
            }

            return {};
        }

        bool NodeTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
        {
            if(!index.isValid())
            {
                return false;
            }

            auto *resultNode = this->getResultNode(index.parent());
            if(!resultNode)
            {
                return false;
            }

            if(role == Qt::EditRole)
            {
                role = Qt::DisplayRole;
            }

            switch (role) {
            case Qt::DisplayRole:
            {
                auto *cellResultNode = resultNode->getOrCreateCell(index.row(),index.column());
                if(cellResultNode != nullptr)
                {
                    cellResultNode->setValue(value.toString().toStdString());
                }
                return true;
            }
            default:
                break;
            }
            return false;
        }

        int NodeTableModel::rows() const
        {
            return this->rowCount();
        }

        int NodeTableModel::columns() const
        {
            return this->columnCount();
        }

        void NodeTableModel::setRows(int rows)
        {
            if(rows == rowCount())
            {
                return;
            }

            this->beginResetModel();
            this->m_rootResult->setSize(rows,this->columnCount());
            this->endResetModel();
            emit this->rowsChanged();
        }

        void NodeTableModel::setColumns(int columns)
        {
            if(columns == columnCount())
            {
                return;
            }
            this->beginResetModel();
            this->m_rootResult->setSize(this->rowCount(), columns);
            this->endResetModel();
            emit this->columnsChanged();
        }

        PythonNodeResult *NodeTableModel::getResultNode(const QModelIndex &index) const
        {
            if (!index.isValid())
            {
                return m_rootResult;
            }

            return static_cast<PythonNodeResult*>(index.internalPointer());
        }

    };
};
