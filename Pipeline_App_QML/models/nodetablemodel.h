#pragma once
#include <QHash>
#include <QAbstractItemModel>
#include <data/hierarchicaltabledata.h>

namespace Pipeline
{
    namespace Runtime
    {
        class NodeTableModel : public QAbstractItemModel
        {
                Q_PROPERTY(size_t rows READ rows WRITE setRows NOTIFY rowsChanged)
                Q_PROPERTY(size_t columns READ columns WRITE setColumns NOTIFY columnsChanged)
                Q_OBJECT
            public:
                explicit NodeTableModel(QObject*parent = nullptr);
                virtual ~NodeTableModel() override;
                int rowCount(const QModelIndex &parent = QModelIndex()) const override;
                int columnCount(const QModelIndex &parent = QModelIndex()) const override;

                QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
                QModelIndex parent(const QModelIndex &child) const override;
                QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
                Q_INVOKABLE bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                                               int role = Qt::EditRole) override;
                Q_INVOKABLE bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
                Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation,
                                                int role = Qt::DisplayRole) const override;
                Q_INVOKABLE QModelIndex createCell(const QModelIndex&index);
                virtual QHash<int, QByteArray> roleNames() const;

            public:
                void setRoot(const std::shared_ptr<HierarchicalTableData>& root);
                int rows() const;
                int columns() const;
                void setRows(int rows);
                void setColumns(int columns);
                std::shared_ptr<HierarchicalTableData> getRoot() const
                {
                    return m_rootResult;
                }
            signals:
                void rowsChanged();
                void columnsChanged();
            private:
                HierarchicalTableData* getParentTableData(const QModelIndex& index) const;
                HierarchicalTableData* accessParentTableData(const QModelIndex& index);
            private:
                std::shared_ptr<HierarchicalTableData>  m_rootResult;
        };
    }
}
