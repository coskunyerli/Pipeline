#pragma once
#include <QAbstractListModel>
#include <data/contextmetadata.h>
#include <vector>

namespace Pipeline::Runtime
{
    class NodeModelItem
    {
        public:
            explicit NodeModelItem() = default;
            explicit NodeModelItem(const QString description, const NodeContextMetadata &metadata);
            QString getDescription()const
            {
                return m_description;
            }

            const NodeContextMetadata& getMetadata() const
            {
                return m_metadata;
            }
        private:
            QString m_description;
            NodeContextMetadata m_metadata;
    };

    class NodeModel : public QAbstractListModel
    {
            Q_OBJECT
        public:
            explicit NodeModel(QObject*parent = nullptr);
            Q_INVOKABLE int rowCount(const QModelIndex&parent = QModelIndex()) const override;
            Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
            Q_INVOKABLE virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
            Q_INVOKABLE void addNode(const QString &description, const NodeContextMetadata& metadata);
            Q_INVOKABLE void removeNode(const QModelIndex & index);
            Q_INVOKABLE NodeContextMetadata getNodeContext(const QString &nodeName);
            QHash<int, QByteArray> roleNames() const override;
        private:
            std::vector<NodeModelItem> m_nodes;
    };
}
