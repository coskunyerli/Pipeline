#include "models/nodetablemodel.h"
#include <helpers/stringhelper.h>
#include <constants.h>
namespace Pipeline
{
    namespace Runtime
    {

        NodeTableModel::NodeTableModel(QObject *parent)
            : QAbstractItemModel(parent)
        {
            m_rootResult = QSharedPointer<HierarchicalTableData>::create();
            m_rootResult->setSize(10, 10);
        }

        NodeTableModel::~NodeTableModel()
        {
        }

        int NodeTableModel::rowCount(const QModelIndex &parent) const
        {
            HierarchicalTableData* nodeResult;

            if (!parent.isValid())
            {
                nodeResult = this->m_rootResult.get();
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
            HierarchicalTableData* nodeResult;

            if (!parent.isValid())
            {
                nodeResult = this->m_rootResult.get();
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
            auto* parentNode = m_rootResult.get();

            if (parent.isValid())
            {
                parentNode = getResultNode(parent);
            }

            if (!parentNode)
            {
                return QModelIndex();
            }

            return createIndex(row, column, parentNode->getCell(row, column));
        }

        QModelIndex NodeTableModel::parent(const QModelIndex &child) const
        {
            if (!child.isValid())
            {
                return QModelIndex();
            }

            auto* childNode = getResultNode(child);

            if (!childNode)
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
            std::pair<size_t, size_t> parentCellIndex = parentNode->getParent()->cellIndexOf(parentNode, has);

            if (has)
            {
                return createIndex(static_cast<int>(parentCellIndex.first), static_cast<int>(parentCellIndex.second), parentNode);
            }

            return QModelIndex();
        }

        QVariant NodeTableModel::data(const QModelIndex &index, int role) const
        {
            if (!index.isValid())
            {
                return {};
            }

            HierarchicalTableData* resultNode = getResultNode(index.parent());

            if (!resultNode)
            {
                return {};
            }

            if (role == Qt::DisplayRole)
            {
                return QString::fromStdString(resultNode->getCellValue(index.row(), index.column()));
            }
            else if (role == NodeTableRoles::HasTable)
            {
                return static_cast<HierarchicalTableData::ValueType>(resultNode->getCellValueType(index.row(),
                        index.column()) & HierarchicalTableData::ValueType::Matrix) != HierarchicalTableData::ValueType::None;
            }

            return {};
        }

        bool NodeTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
        {
            if (role == Qt::EditRole)
            {
                role = Qt::DisplayRole;
            }

            if (orientation == Qt::Horizontal)
            {
                switch (role)
                {
                    case Qt::DisplayRole:
                        {
                            m_rootResult->setHeaderData(section, value.toString().toStdString());
                            emit this->headerDataChanged(orientation, section, section + 1);
                            return true;
                        }
                }
            }

            return false;
        }

        bool NodeTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
        {
            if (!index.isValid())
            {
                return false;
            }

            auto *resultNode = this->getResultNode(index.parent());

            if (!resultNode)
            {
                return false;
            }

            if (role == Qt::EditRole)
            {
                role = Qt::DisplayRole;
            }

            switch (role)
            {
                case Qt::DisplayRole:
                    {
                        resultNode->setCellValue(index.row(), index.column(), value.toString().toStdString());
                        emit dataChanged(index, index, {role});
                        return true;
                    }

                default:
                    break;
            }

            return false;
        }

        QVariant NodeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
        {
            if (orientation == Qt::Vertical)
            {
                return section;
            }
            else if (orientation == Qt::Horizontal)
            {
                return QString::fromStdString(m_rootResult->getHeaderData(section));
            }

            return QVariant();
        }

        QModelIndex NodeTableModel::createCell(const QModelIndex &index)
        {
            if (!index.isValid())
            {
                return QModelIndex();
            }

            auto parent = index.parent();
            auto* tableData = accessResultNode(parent);

            if (!tableData)
            {
                return QModelIndex();
            }

            auto child = tableData->getOrCreateCell(index.row(), index.column());
            child->setSize(10,10);
            emit dataChanged(index, index, {NodeTableRoles::ChildCell, NodeTableRoles::HasTable});
            return this->index(index.row(),index.column(), parent);
        }

        QHash<int, QByteArray> NodeTableModel::roleNames() const
        {
            auto roleNames = QAbstractItemModel::roleNames();
            roleNames[NodeTableRoles::HasTable] = "hasTable";
            return roleNames;
        }

        void NodeTableModel::setRoot(const QSharedPointer<HierarchicalTableData>& root)
        {
            this->beginResetModel();
            m_rootResult = root;
            emit this->columnsChanged();
            emit this->rowsChanged();
            this->endResetModel();
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
            if (rows == rowCount())
            {
                return;
            }

            this->beginResetModel();
            this->m_rootResult->setSize(rows, this->columnCount());
            this->endResetModel();
            emit this->rowsChanged();
        }

        void NodeTableModel::setColumns(int columns)
        {
            if (columns == columnCount())
            {
                return;
            }

            this->beginResetModel();
            this->m_rootResult->setSize(this->rowCount(), columns);
            this->endResetModel();
            emit this->columnsChanged();
        }

        HierarchicalTableData* NodeTableModel::getResultNode(const QModelIndex &index) const
        {
            if (!index.isValid())
            {
                return m_rootResult.get();
            }

            return static_cast<HierarchicalTableData*>(index.internalPointer());
        }

        HierarchicalTableData* NodeTableModel::accessResultNode(const QModelIndex &index)
        {
            if (!index.isValid())
            {
                return m_rootResult.get();
            }

            return static_cast<HierarchicalTableData*>(index.internalPointer());
        }

    };
};
