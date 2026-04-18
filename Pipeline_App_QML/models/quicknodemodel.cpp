#include "quicknodemodel.h"
#include <data/contextmetadata.h>
#include <constants.h>
namespace Pipeline::Runtime
{

    QuickNodeModel::QuickNodeModel(QObject *parent)
        : QAbstractListModel(parent)
    {
    }

    int QuickNodeModel::rowCount(const QModelIndex &parent) const
    {
        return static_cast<int>(m_nodes.size());
    }

    QVariant QuickNodeModel::data(const QModelIndex &index, int role) const
    {
        if (!index.isValid())
        {
            return {};
        }

        auto &modelItem = m_nodes[index.row()];

        switch (role)
        {
            case Constants::NodeModelRoles::NodeNameRole:
                return modelItem.getMetadata().getName();

            case Constants::NodeModelRoles::NodeShortNameRole:
                return modelItem.getMetadata().getName().left(2);

            case Constants::NodeModelRoles::NodeDescriptionRole:
                return modelItem.getDescription();

            case Constants::NodeModelRoles::NodeTypeRole:
                return modelItem.getMetadata().getNodeType();

            default:
                return {};
        }
    }

    bool QuickNodeModel::setData(const QModelIndex &index, const QVariant &value, int role)
    {
        return false;
    }

    void QuickNodeModel::addNode(const QString &description, const NodeContextMetadata &metadata)
    {
        int row = static_cast<int>(m_nodes.size());
        beginInsertRows(QModelIndex(), row, row);

        QuickNodeModelItem item(description, metadata);
        m_nodes.push_back(std::move(item));

        endInsertRows();
    }

    // Remove node
    void QuickNodeModel::removeNode(const QModelIndex &index)
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

    NodeContextMetadata QuickNodeModel::getNodeContext(const QString &nodeName)
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

    QHash<int, QByteArray> QuickNodeModel::roleNames() const
    {
        return
        {
            {Constants::NodeModelRoles::NodeNameRole, "name"},
            {Constants::NodeModelRoles::NodeShortNameRole, "shortName"},
            {Constants::NodeModelRoles::NodeDescriptionRole, "description"},
            {Constants::NodeModelRoles::NodeTypeRole, "type"},
        };
    }

    QuickNodeModelItem::QuickNodeModelItem(const QString description, const NodeContextMetadata &metadata)
        : m_description(description)
        , m_metadata(metadata)
    {
    }

}
