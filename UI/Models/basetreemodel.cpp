#include "basetreemodel.h"


namespace Pipeline
{
    namespace UI
    {
        BaseTreeModel::BaseTreeModel(QObject *parent)

            : QAbstractItemModel(parent)
        {
            this->m_rootNode = new TreeItem();
        }

        BaseTreeModel::~BaseTreeModel()
        {
            delete m_rootNode;
        }

        int BaseTreeModel::rowCount(const QModelIndex &parent) const
        {
            TreeItem* node;

            if (!parent.isValid())
            {
                node = this->m_rootNode;
            }
            else
            {
                node = getTreeItem(parent);
            }

            if (node)
            {
                return static_cast<int>(node->getChildCount());
            }

            return 0;
        }

        int BaseTreeModel::columnCount(const QModelIndex &parent) const
        {
            return 1;
        }

        QModelIndex BaseTreeModel::index(int row, int column, const QModelIndex &parent) const
        {
            auto* parentNode = m_rootNode;

            if (parent.isValid())
            {
                parentNode = getTreeItem(parent);
            }

            if (!parentNode)
            {
                return QModelIndex();
            }

            return createIndex(row, column, parentNode->getChild(row));
        }

        QModelIndex BaseTreeModel::parent(const QModelIndex &child) const
        {
            if (!child.isValid())
            {
                return QModelIndex();
            }

            if (!child.parent().isValid())
            {
                return QModelIndex();
            }

            auto* parentNode = getTreeItem(child.parent());

            if (!parentNode)
            {
                return QModelIndex();
            }

            if (!parentNode->getParent())
            {
                return QModelIndex();
            }

            bool has;
            size_t parentIndex = parentNode->getParent()->indexOf(parentNode, has);

            if (has)
            {
                return createIndex(static_cast<int>(parentIndex), 0, parentNode);
            }

            return QModelIndex();
        }

        bool BaseTreeModel::addNodeItem(TreeItem *item, const QModelIndex &parent)
        {
            auto *parentNode = m_rootNode;

            if (parent.isValid())
            {
                parentNode = getTreeItem(parent);
            }

            if (!parentNode)
            {
                return false;
            }

            this->beginInsertRows(parent, rowCount(parent), rowCount(parent));
            parentNode->addChild(item);
            this->endInsertRows();
            return true;
        }

        TreeItem* BaseTreeModel::removeNodeItem(const QModelIndex &index)
        {
            if (!index.isValid())
            {
                return nullptr;
            }

            TreeItem* node = getTreeItem(index);

            if (!node)
            {
                return nullptr;
            }

            if (!node->getParent())
            {
                return nullptr;
            }

            return node->getParent()->removeChild(index.row());
        }

        bool BaseTreeModel::deleteNodeItem(const QModelIndex &index)
        {
            auto *node = removeNodeItem(index);

            if (node)
            {
                delete node;
                return true;
            }
            else
            {
                return false;
            }
        }

        TreeItem* BaseTreeModel::getTreeItem(const QModelIndex &index) const
        {
            if (!index.isValid())
            {
                return m_rootNode;
            }

            return static_cast<TreeItem*>(index.internalPointer());
        }
    }
}

