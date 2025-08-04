#pragma once
#include <QAbstractItemModel>
#include "treeitem.h"
namespace Pipeline
{
    namespace UI
    {
        class BaseTreeModel : public QAbstractItemModel
        {
                Q_OBJECT
            public:
                BaseTreeModel(QObject*parent = nullptr);
                virtual ~BaseTreeModel();
                int rowCount(const QModelIndex &parent = QModelIndex()) const override;
                int columnCount(const QModelIndex &parent = QModelIndex()) const override;
                QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
                QModelIndex parent(const QModelIndex &child) const override;
                bool addNodeItem(TreeItem* item, const QModelIndex& parent);
                TreeItem* removeNodeItem(const QModelIndex& index);
                bool deleteNodeItem(const QModelIndex& index);
            protected:
                TreeItem* getTreeItem(const QModelIndex& index) const;
                template <typename T>
                T getData(const QModelIndex &index) const
                {
                    TreeItem* treeItem = getTreeItem(index);

                    if (!treeItem)
                    {
                        return T();
                    }

                    return treeItem->getData<T>();
                }
            private:
                TreeItem* m_rootNode;
        };
    }
}
