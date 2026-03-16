#include "nodetablesliceproxymodel.h"
#include <constants.h>
namespace Pipeline::Runtime
{

    NodeTableSliceProxyModel::NodeTableSliceProxyModel(QObject *parent)
        : QAbstractProxyModel(parent)
    {
    }

    void NodeTableSliceProxyModel::setCurrentIndex(const QModelIndex &index)
    {
        if (m_currentIndex == index)
            return;

        beginResetModel();
        m_currentIndex = index;
        emit this->currentIndexChanged();
        this->currentIndexValueChanged();
        endResetModel();
    }

    QString NodeTableSliceProxyModel::currentIndexValue() const
    {
        if (!this->sourceModel())
        {
            return "";
        }

        return this->sourceModel()->data(this->m_currentIndex).toString();
    }

    void NodeTableSliceProxyModel::setCurrentIndexValue(const QString &value)
    {
        if (!this->sourceModel())
        {
            return;
        }

        if (this->currentIndexValue() == value)
        {
            return;
        }

        this->sourceModel()->setData(this->m_currentIndex, value);
        this->currentIndexValueChanged();
    }

    QModelIndex NodeTableSliceProxyModel::mapToSource(const QModelIndex &proxyIndex) const
    {
        if (!proxyIndex.isValid())
            return QModelIndex();

        return sourceModel()->index(
                   proxyIndex.row(),
                   proxyIndex.column(),
                   m_currentIndex
               );
    }

    QModelIndex NodeTableSliceProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
    {
        if (!sourceIndex.isValid())
            return QModelIndex();

        if (sourceIndex.parent() != m_currentIndex)
            return QModelIndex();

        return index(sourceIndex.row(), sourceIndex.column());
    }

    QModelIndex NodeTableSliceProxyModel::index(int row, int column, const QModelIndex &parent) const
    {
        if (parent.isValid())
            return QModelIndex();

        return createIndex(row, column);
    }

    int NodeTableSliceProxyModel::rowCount(const QModelIndex &parent) const
    {
        if (parent.isValid())
            return 0;

        if (!sourceModel())
            return 0;

        return sourceModel()->rowCount(m_currentIndex);
    }

    int NodeTableSliceProxyModel::columnCount(const QModelIndex &parent) const
    {
        if (!sourceModel())
            return 0;

        return sourceModel()->columnCount(m_currentIndex);
    }

    QVariant NodeTableSliceProxyModel::data(const QModelIndex &index, int role) const
    {
        return sourceModel()->data(mapToSource(index), role);
    }

    bool NodeTableSliceProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (!sourceModel())
            return false;

        QModelIndex sourceIndex = mapToSource(index);

        if (!sourceIndex.isValid())
            return false;

        return sourceModel()->setData(sourceIndex, value, role);
    }

    Qt::ItemFlags NodeTableSliceProxyModel::flags(const QModelIndex &index) const
    {
        if (!sourceModel())
            return Qt::NoItemFlags;

        return sourceModel()->flags(mapToSource(index));
    }

    QVariant NodeTableSliceProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (!sourceModel())
            return QVariant();

        if (orientation == Qt::Horizontal)
        {
            return this->sourceModel()->data(this->m_currentIndex, NodeTableRoles::HeaderData + section).toString();
        }
        else
        {
            return this->sourceModel()->headerData(section, orientation, role);
        }
    }

    bool NodeTableSliceProxyModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
    {
        if (!sourceModel())
            return false;

        if (orientation == Qt::Horizontal)
        {
            return this->sourceModel()->setData(this->m_currentIndex, value, NodeTableRoles::HeaderData + section);
        }
        else
        {
            return sourceModel()->setHeaderData(section, orientation, value, role);
        }
    }

    void NodeTableSliceProxyModel::setSourceModel(QAbstractItemModel *model)
    {
        if (sourceModel() == model)
            return;

        if (sourceModel())
        {
            disconnect(sourceModel(), nullptr, this, nullptr);
        }

        QAbstractProxyModel::setSourceModel(model);

        if (!model)
            return;

        // dataChanged forward
        connect(model, &QAbstractItemModel::dataChanged,
                this,
                [this](const QModelIndex & topLeft,
                       const QModelIndex & bottomRight,
                       const QVector<int>& roles)
        {
            QModelIndex p1 = mapFromSource(topLeft);
            QModelIndex p2 = mapFromSource(bottomRight);

            if (p1.isValid() && p2.isValid())
                emit dataChanged(p1, p2, roles);
        });
        // header değişimi forward
        connect(model, &QAbstractItemModel::headerDataChanged,
                this,
                &QAbstractItemModel::headerDataChanged);
        connect(model, &QAbstractItemModel::modelAboutToBeReset,
                this,
                [this]()
        {
            this->beginResetModel();
        });
        connect(model, &QAbstractItemModel::modelReset,
                this,
                [this]()
        {
            this->endResetModel();
        });
    }
}
