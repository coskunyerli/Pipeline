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
            m_rootResult = std::make_shared<HierarchicalTableData>();
            m_rootResult->setSize(10, 10);
        }

        NodeTableModel::~NodeTableModel()
        {
        }

        int NodeTableModel::rowCount(const QModelIndex &parent) const
        {
            HierarchicalTableData* parentTableData;

            if (!parent.isValid())
            {
                parentTableData = this->m_rootResult.get();
            }
            else
            {
                parentTableData = getParentTableData(parent);
                parentTableData = parentTableData->getCell(parent.row(),parent.column()).get();
            }

            if (parentTableData)
            {
                return static_cast<int>(parentTableData->getRowCount());
            }

            return 0;
        }

        int NodeTableModel::columnCount(const QModelIndex &parent) const
        {
            HierarchicalTableData* parentTableData;

            if (!parent.isValid())
            {
                parentTableData = this->m_rootResult.get();
            }
            else
            {
                parentTableData = getParentTableData(parent);
                parentTableData = parentTableData->getCell(parent.row(),parent.column()).get();
            }

            if (parentTableData)
            {
                return static_cast<int>(parentTableData->getColumnCount());
            }

            return 0;
        }

        QModelIndex NodeTableModel::index(int row, int column, const QModelIndex &parent) const
        {
            HierarchicalTableData* parentTableData = m_rootResult.get();

            if (parent.isValid())
            {
                parentTableData = getParentTableData(parent);
                parentTableData = parentTableData->getCell(parent.row(),parent.column()).get();
            }

            if (!parentTableData)
                return QModelIndex();

            return createIndex(row, column, parentTableData);
        }

        QModelIndex NodeTableModel::parent(const QModelIndex &child) const
        {
            if (!child.isValid())
                return QModelIndex();

            auto* parentTableData = getParentTableData(child);

            if (!parentTableData)
                return QModelIndex();

            auto* grandParentTableData = parentTableData->getParent();

            if (!grandParentTableData)
                return QModelIndex();

            bool has;
            auto pos = grandParentTableData->cellIndexOf(parentTableData, has);

            if (!has)
                return QModelIndex();

            return createIndex(
                static_cast<int>(pos.first),
                static_cast<int>(pos.second),
                grandParentTableData
                );
        }

        QVariant NodeTableModel::data(const QModelIndex &index, int role) const
        {
            if (!index.isValid())
            {
                return {};
            }

            HierarchicalTableData* parentTableData = getParentTableData(index);

            if (!parentTableData)
            {
                return {};
            }

            if (role == Qt::DisplayRole)
            {
                return QString::fromStdString(parentTableData->getCellValue(index.row(), index.column()));
            }
            else if (role == NodeTableRoles::HasTable)
            {
                return static_cast<HierarchicalTableData::ValueType>(parentTableData->getCellValueType(index.row(),
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

            auto *parentTableData = this->getParentTableData(index);

            if (!parentTableData)
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
                        parentTableData->setCellValue(index.row(), index.column(), value.toString().toStdString());
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

            auto* parentTableData = accessParentTableData(index);
            if (!parentTableData)
            {
                return QModelIndex();
            }

            auto child = parentTableData->getOrCreateCell(index.row(), index.column());
            child->setSize(10, 10);
            emit dataChanged(index, index, {NodeTableRoles::ChildCell, NodeTableRoles::HasTable});
            return this->index(index.row(), index.column(), index.parent());
        }

        QHash<int, QByteArray> NodeTableModel::roleNames() const
        {
            auto roleNames = QAbstractItemModel::roleNames();
            roleNames[NodeTableRoles::HasTable] = "hasTable";
            return roleNames;
        }

        void NodeTableModel::setRoot(const std::shared_ptr<HierarchicalTableData>& root)
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

            if (!m_rootResult)
            {
                return;
            }

            this->beginResetModel();
            this->m_rootResult->setSize(static_cast<size_t>(rows), static_cast<size_t>(this->columnCount()));
            this->endResetModel();
            emit this->rowsChanged();
        }

        void NodeTableModel::setColumns(int columns)
        {
            if (columns == columnCount())
            {
                return;
            }

            if (!m_rootResult)
            {
                return;
            }

            this->beginResetModel();
            this->m_rootResult->setSize(static_cast<size_t>(this->rowCount()), static_cast<size_t>(columns));
            this->endResetModel();
            emit this->columnsChanged();
        }

        HierarchicalTableData* NodeTableModel::getParentTableData(const QModelIndex &index) const
        {
            if (!index.isValid())
            {
                return m_rootResult.get();
            }

            return static_cast<HierarchicalTableData*>(index.internalPointer());
        }

        HierarchicalTableData* NodeTableModel::accessParentTableData(const QModelIndex &index)
        {
            if (!index.isValid())
            {
                return m_rootResult.get();
            }

            return static_cast<HierarchicalTableData*>(index.internalPointer());
        }

    };
};
