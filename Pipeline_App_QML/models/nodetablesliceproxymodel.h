#pragma once

#include <QAbstractProxyModel>
#include <QPersistentModelIndex>
namespace Pipeline::Runtime
{
    class NodeTableModel;
    class NodeTableSliceProxyModel : public QAbstractProxyModel
    {
            Q_OBJECT
            Q_PROPERTY(size_t rows READ rows WRITE setRows NOTIFY rowsChanged)
            Q_PROPERTY(size_t columns READ columns WRITE setColumns NOTIFY columnsChanged)
            Q_PROPERTY(QModelIndex currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
            Q_PROPERTY(QString currentIndexValue READ currentIndexValue WRITE setCurrentIndexValue NOTIFY currentIndexValueChanged)

        public:
            explicit NodeTableSliceProxyModel(QObject* parent = nullptr);
            Q_INVOKABLE void setCurrentIndex(const QModelIndex& index);

            QModelIndex currentIndex() const
            {
                return m_currentIndex;
            }
            QString currentIndexValue() const;
            void setCurrentIndexValue(const QString &value);
            Q_INVOKABLE bool setHeaderData(int section,
                                           Qt::Orientation orientation,
                                           const QVariant& value,
                                           int role = Qt::EditRole) override;

            // ----------------------------------------------------
            QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
            QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
            // ----------------------------------------------------

            QModelIndex index(int row, int column,
                              const QModelIndex& parent = QModelIndex()) const override;
            QModelIndex parent(const QModelIndex&) const override
            {
                return QModelIndex();
            }

            // ----------------------------------------------------

            int rowCount(const QModelIndex& parent = QModelIndex()) const override;
            int columnCount(const QModelIndex& parent = QModelIndex()) const override;
            QVariant data(const QModelIndex& index, int role) const override;
            bool setData(const QModelIndex& index,
                         const QVariant& value,
                         int role = Qt::EditRole) override;
            Qt::ItemFlags flags(const QModelIndex& index) const override;
            QVariant headerData(int section,
                                Qt::Orientation orientation,
                                int role) const override;


            void setSourceModel(QAbstractItemModel* model) override;
            size_t rows() const;
            void setRows(size_t newRows);

            size_t columns() const;
            void setColumns(size_t newColumns);

        signals:
            void currentIndexChanged();
            void currentIndexValueChanged();
            void rowsChanged();

            void columnsChanged();
        private:
            void onSourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                     const QList<int> &roles = QList<int>());
        private:
            QPersistentModelIndex m_currentIndex;
    };

}
