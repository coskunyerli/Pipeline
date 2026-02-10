#pragma once
#include <QAbstractItemModel>
#include "data/pythonnoderesult.h"
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
                explicit NodeTableModel(QObject*parent=nullptr);
                virtual ~NodeTableModel() override;
                int rowCount(const QModelIndex &parent = QModelIndex()) const override;
                int columnCount(const QModelIndex &parent = QModelIndex()) const override;
                QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
                QModelIndex parent(const QModelIndex &child) const override;
                QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
                Q_INVOKABLE bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
            public:
                int rows() const;
                int columns() const;
                void setRows(int rows);
                void setColumns(int columns);
                PythonNodeResult* getRoot() const {return m_rootResult;}
            signals:
                void rowsChanged();
                void columnsChanged();
            private:
                PythonNodeResult* getResultNode(const QModelIndex& index) const;
            private:
                PythonNodeResult * m_rootResult;
        };
    }
}
