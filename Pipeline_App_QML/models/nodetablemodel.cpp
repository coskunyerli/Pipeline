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
                parentTableData = parentTableData->getCell(parent.row(), parent.column()).get();
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
                parentTableData = parentTableData->getCell(parent.row(), parent.column()).get();
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
                parentTableData = parentTableData->getCell(parent.row(), parent.column()).get();
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
            if (!m_rootResult)
            {
                return {};
            }

            if (!index.isValid())
            {
                if (role == Qt::DisplayRole)
                {
                    return QString::fromStdString(this->m_rootResult->getValue());
                }
                else if (role >= NodeTableRoles::HeaderData && role < NodeTableRoles::HeaderDataEnd)
                {
                    int headerDataIndex = role - NodeTableRoles::HeaderData;
                    return QString::fromStdString(m_rootResult->getHeaderData(headerDataIndex));
                }

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
            else if (role == NodeTableRoles::CellName)
            {
                auto cell = parentTableData->getCell(index.row(), index.column());

                if (cell && !cell->getName().empty())
                {
                    return QString::fromStdString(cell->getName());
                }

                auto keyString = QString("(") + QString::number(index.row()) + ", " + this->data(index.parent(), NodeTableRoles::HeaderData + index.column()).toString() + ")";
                return keyString;
            }
            else if (role == NodeTableRoles::ChildCell)
            {
                auto cell = parentTableData->getCell(index.row(), index.column());
                QVariant v = QVariant::fromValue<std::shared_ptr<HierarchicalTableData>>(cell);
                return v;
            }

            else if (role >= NodeTableRoles::HeaderData && role < NodeTableRoles::HeaderDataEnd)
            {
                auto cell = parentTableData->getCell(index.row(), index.column());
                int headerDataIndex = role - NodeTableRoles::HeaderData;
                return QString::fromStdString(cell->getHeaderData(headerDataIndex));
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
                            return this->setData(QModelIndex(), value, NodeTableRoles::HeaderData + section);;
                        }
                }
            }

            return false;
        }

        bool NodeTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
        {
            if (role == Qt::EditRole)
            {
                role = Qt::DisplayRole;
            }

            if (!index.isValid())
            {
                switch (role)
                {
                    case  Qt::DisplayRole:
                        {
                            this->m_rootResult->setValue(value.toString().toStdString());
                            this->dataChanged(QModelIndex(), QModelIndex(), {role});
                            return true;
                        }

                    case NodeTableRoles::Rows:
                        {
                            m_rootResult->setSize(value.toInt(), m_rootResult->getColumnCount());
                            emit dataChanged(index, index, {role});
                            return true;
                        }

                    case NodeTableRoles::Columns:
                        {
                            m_rootResult->setSize(m_rootResult->getRowCount(), value.toInt());
                            emit dataChanged(index, index, {role});
                            return true;
                        }
                }

                if (role >= NodeTableRoles::HeaderData && role < NodeTableRoles::HeaderDataEnd)
                {
                    int headerDataIndex = role - NodeTableRoles::HeaderData;
                    m_rootResult->setHeaderData(headerDataIndex, value.toString().toStdString());
                    emit this->headerDataChanged(Qt::Horizontal, headerDataIndex, headerDataIndex + 1);
                    return true;
                }

                return false;
            }

            auto *parentTableData = this->getParentTableData(index);

            if (!parentTableData)
            {
                return false;
            }

            switch (role)
            {
                case Qt::DisplayRole:
                    {
                        parentTableData->setCellValue(index.row(), index.column(), value.toString().toStdString());
                        emit dataChanged(index, index, {role});
                        return true;
                    }

                case NodeTableRoles::ChildCell:
                    {
                        parentTableData->setCell(index.row(), index.column(), value.value<std::shared_ptr<HierarchicalTableData>>());
                        emit dataChanged(index, index, {role});
                        return true;
                    }

                case NodeTableRoles::CellName:
                    {
                        auto cell = parentTableData->getCell(index.row(), index.column());
                        if(cell)
                        {
                            cell->setName(value.toString().toStdString());
                            emit dataChanged(index, index, {role});
                            return true;
                        }
                    }
                case NodeTableRoles::Rows:
                    {
                        auto cell = parentTableData->getCell(index.row(), index.column());
                        if(cell)
                        {
                            cell->setSize(value.toInt(), cell->getColumnCount());
                            emit dataChanged(index, index, {role});
                            return true;
                        }
                    }
                case NodeTableRoles::Columns:
                    {
                        auto cell = parentTableData->getCell(index.row(), index.column());
                        if(cell)
                        {
                            cell->setSize(cell->getRowCount(), value.toInt());
                            emit dataChanged(index, index, {role});
                            return true;
                        }
                    }
                default:
                    break;
            }

            if (role >= NodeTableRoles::HeaderData && role < NodeTableRoles::HeaderDataEnd)
            {
                int headerDataIndex = role - NodeTableRoles::HeaderData;
                auto cell = parentTableData->getCell(index.row(), index.column());

                if (!cell)
                {
                    return false;
                }

                cell->setHeaderData(headerDataIndex, value.toString().toStdString());
                emit this->headerDataChanged(Qt::Horizontal, headerDataIndex, headerDataIndex + 1);
                return true;
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
            roleNames[NodeTableRoles::CellName] = "cellName";
            return roleNames;
        }

        void NodeTableModel::setRoot(const std::shared_ptr<HierarchicalTableData>& root)
        {
            this->beginResetModel();
            m_rootResult = root;
            this->endResetModel();
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
