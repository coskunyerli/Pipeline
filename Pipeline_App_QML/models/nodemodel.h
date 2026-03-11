#pragma once
#include <QAbstractListModel>
#include <vector>
namespace Pipeline::Runtime
{
    class NodeModel : public QAbstractListModel
    {
            Q_OBJECT
        public:
            explicit NodeModel(QObject*parent = nullptr);
            Q_INVOKABLE int rowCount(const QModelIndex&parent = QModelIndex()) const override;
            Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
            Q_INVOKABLE virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
        private:
            std::vector<int> m_nodes;
    };
}
