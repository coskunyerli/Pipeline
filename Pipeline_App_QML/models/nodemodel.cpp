#include "nodemodel.h"
#include <data/contextmetadata.h>
#include <constants.h>
namespace Pipeline::Runtime
{

    NodeModel::NodeModel(QObject *parent)
        : QAbstractListModel(parent)
    {
    }

    int NodeModel::rowCount(const QModelIndex &parent) const
    {
        return static_cast<int>(m_nodes.size());
    }

    QVariant NodeModel::data(const QModelIndex &index, int role) const
    {
        if (!index.isValid())
        {
            return {};
        }

        auto &modelItem = m_nodes[index.row()];

        switch (role)
        {
            case NodeModelRoles::NodeNameRole:
                return modelItem.getMetadata().getName();

            case NodeModelRoles::NodeShortNameRole:
                return modelItem.getMetadata().getName().left(2);

            case NodeModelRoles::NodeDescriptionRole:
                return modelItem.getDescription();

            case NodeModelRoles::NodeTypeRole:
                return modelItem.getMetadata().getNodeType();

            default:
                return {};
        }
    }

    bool NodeModel::setData(const QModelIndex &index, const QVariant &value, int role)
    {
        return false;
    }

    void NodeModel::addNode(const QString &description, const NodeContextMetadata &metadata)
    {
        int row = static_cast<int>(m_nodes.size());
        beginInsertRows(QModelIndex(), row, row);

        NodeModelItem item(description, metadata);
        m_nodes.push_back(std::move(item));

        endInsertRows();
    }

    // Remove node
    void NodeModel::removeNode(const QModelIndex &index)
    {
        if (!index.isValid())
            return;

        int row = index.row();
        if (row < 0 || row >= m_nodes.size())
            return;

        beginRemoveRows(QModelIndex(), row, row);
        m_nodes.erase(m_nodes.begin() + row);
        endRemoveRows();
    }

    NodeContextMetadata NodeModel::getNodeContext(const QString &nodeName)
    {
        for (auto i = 0; i < rowCount(); i++)
        {
            auto quickNode = m_nodes[i];
            auto&metadata = quickNode.getMetadata();
            if(metadata.getName() == nodeName)
            {
                return metadata;
            }
        }

        return NodeContextMetadata();
    }

    QHash<int, QByteArray> NodeModel::roleNames() const
    {
        return
        {
            {NodeModelRoles::NodeNameRole, "name"},
            {NodeModelRoles::NodeShortNameRole, "shortName"},
            {NodeModelRoles::NodeDescriptionRole, "description"},
            {NodeModelRoles::NodeTypeRole, "type"},
        };
    }

    NodeModelItem::NodeModelItem(const QString description, const NodeContextMetadata &metadata)
        : m_description(description)
        , m_metadata(metadata)
    {
    }

}
