#include "nodemodel.h"

namespace Pipeline::Runtime
{

    NodeModel::NodeModel(QObject *parent)
        : QAbstractListModel(parent)
    {
    }

    int NodeModel::rowCount(const QModelIndex &parent) const
    {
        return 0;
    }

    QVariant NodeModel::data(const QModelIndex &index, int role) const
    {
        return {};
    }

    bool NodeModel::setData(const QModelIndex &index, const QVariant &value, int role)
    {
        return false;
    }

}
