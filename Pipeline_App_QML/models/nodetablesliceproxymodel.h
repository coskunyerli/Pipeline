#pragma once

#include <QAbstractProxyModel>
#include <QPersistentModelIndex>
namespace Pipeline::Runtime
{
    class NodeTableSliceProxyModel : public QAbstractProxyModel
    {
            Q_OBJECT

        public:
            explicit NodeTableSliceProxyModel(QObject* parent = nullptr);
            Q_INVOKABLE void setCurrentIndex(const QModelIndex& index);

            Q_INVOKABLE QModelIndex currentIndex() const
            {
                return m_currentIndex;
            }
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

        private:
            QPersistentModelIndex m_currentIndex;
    };

}
