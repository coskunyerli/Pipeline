#include "nodeparamlistmodel.h"
#include <helpers/serializehelper.h>

namespace Pipeline::Runtime
{

    NodeParamListModel::NodeParamListModel(QObject *parent)
        : QAbstractListModel(parent)
    {}

    int NodeParamListModel::rowCount(const QModelIndex &parent) const
    {
        if (parent.isValid()) return 0;

        return static_cast<int>(m_nodeParamList.paramSize());
    }

    QVariant NodeParamListModel::data(const QModelIndex &index, int role) const
    {
        if (!index.isValid()) return {};

        auto paramName = this->m_nodeParamList.getParameterName(index.row());

        return this->data(QString::fromStdString(paramName), role);
    }

    QVariant NodeParamListModel::data(const QString &name, int role) const
    {
        bool has;
        auto&p = this->m_nodeParamList.getParameter(name.toStdString(), has);

        if (!has)
        {
            return {};
        }

        if(role == Qt::DisplayRole)
        {
            role = ParameterRoles::ValueRole;
        }

        switch (role)
        {
            case ParameterRoles::NameRole:
                return QString::fromStdString(p.name);

            case ParameterRoles::TypeRole:
                return static_cast<int>(p.type);

            case ParameterRoles::TypeStringRole:
                return QString::fromStdString(this->m_nodeParamList.paramTypeToString(p.type));

            case IsBrowseRole:
                return p.type == ParamType::Browse;

            case ParameterRoles::ValueRole:
                {
                    if (p.type == ParamType::List)
                    {
                        QStringList list;

                        // TODOJ for (const auto& v : p.listValues)
                        //     list << v.toString();

                        return list.join(", ");
                    }
                    else if (p.type == ParamType::Table)
                    {
                        return QString("Table (%1)").arg(0/* TODOJ p.children.size()*/);
                    }
                    else
                    {
                        return toQVariant(p.value);
                    }
                }
        }

        return {};
    }

    bool NodeParamListModel::setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (!index.isValid()) return false;

        if (role == Qt::EditRole || role == Qt::DisplayRole)
        {
            role = ParameterRoles::ValueRole;
        }

        auto paramName = this->m_nodeParamList.getParameterName(index.row());
        return this->setData(QString::fromStdString(paramName), value, role);
    }

    bool NodeParamListModel::setData(const QString &name, const QVariant &value, int role)
    {
        bool has;
        auto&p = this->m_nodeParamList.aceessParameter(name.toStdString(), has);

        switch (role)
        {
            case ParameterRoles::NameRole:
                p.name = value.toString().toStdString();
                break;

            // case ParameterRoles::TypeRole:
            //     p.type = static_cast<ParamType>(value.toInt());
            //     break;

            case ParameterRoles::ValueRole:
                {
                    if (p.type == ParamType::List)
                    {
                        // TODOJ p.listValues.clear();
                        // QStringList parts = value.toString().split(",", Qt::SkipEmptyParts);

                        // for (auto& s : parts)
                        //     p.listValues.push_back(s.trimmed());
                    }
                    else if (p.type == ParamType::Table)
                    {
                        return false; // table doğrudan edit edilmez
                    }
                    else
                    {
                        p.value = fromQVariant(value);
                    }

                    break;
                }
        }

        int i = this->m_nodeParamList.getParameterIndex(name.toStdString());
        QModelIndex index = this->index(i, 0);
        emit dataChanged(index, index, {role});
        return true;
    }

    Qt::ItemFlags NodeParamListModel::flags(const QModelIndex &index) const
    {
        if (!index.isValid()) return Qt::NoItemFlags;

        return Qt::ItemIsEnabled |
               Qt::ItemIsSelectable |
               Qt::ItemIsEditable;
    }

    QHash<int, QByteArray> NodeParamListModel::roleNames() const
    {
        return
        {
            {ParameterRoles::NameRole, "name"},
            {ParameterRoles::TypeRole, "type"},
            {ParameterRoles::ValueRole, "value"},
            {ParameterRoles::TypeStringRole, "typeString"},
            {IsBrowseRole, "isBrowse"}
        };
    }

    void NodeParamListModel::addParameter(const QString &name, int type, const QVariant &value)
    {
        if (this->m_nodeParamList.getParameterIndex(name.toStdString()) != -1)
        {
            return;
        }

        beginInsertRows(QModelIndex(), static_cast<int>(this->m_nodeParamList.paramSize()), static_cast<int>(this->m_nodeParamList.paramSize()));
        this->m_nodeParamList.addParameter(name.toStdString(), type, fromQVariant(value));
        endInsertRows();
    }

    void NodeParamListModel::removeParameter(int index)
    {
        if (index >= this->m_nodeParamList.paramSize()) return;

        this->beginRemoveRows(QModelIndex(), index, index);
        this->m_nodeParamList.removeParameter(index);
        this->endRemoveRows();
    }

    void NodeParamListModel::removeParameter(const QString &name)
    {
        int index = this->m_nodeParamList.getParameterIndex(name.toStdString());
        if (index == -1)
        {
            return;
        }

        this->beginRemoveRows(QModelIndex(), index, index);
        this->m_nodeParamList.removeParameter(name.toStdString());
        this->endRemoveRows();
    }

    bool NodeParamListModel::getEditable() const
    {
        return m_editable;
    }

    void NodeParamListModel::setEditable(bool newEditable)
    {
        if (m_editable == newEditable)
            return;

        m_editable = newEditable;
        emit editableChanged();
    }

    void NodeParamListModel::serialize(std::vector<uint8_t>& buffer) const
    {
        m_nodeParamList.serialize(buffer);
    }


    void NodeParamListModel::deserialize(const uint8_t* buffer, size_t size)
    {
        m_nodeParamList.deserialize(buffer, size);
    }


}
