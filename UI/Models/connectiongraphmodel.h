#pragma once
#include <QAbstractProxyModel>
#include "nodegraphtreemodel.h"

namespace Pipeline
{
    namespace UI
    {
        class ConnectionGraphModel : public QAbstractProxyModel
        {
                Q_OBJECT
            public:
                explicit ConnectionGraphModel(QObject *parent = nullptr);
                QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
                QModelIndex parent(const QModelIndex &child) const override;

                QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
                QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
                int columnCount(const QModelIndex &parent = QModelIndex()) const override;
                int rowCount(const QModelIndex &parent = QModelIndex()) const override;
                void setSourceModel(QAbstractItemModel *model) override;
                NodeGraphTreeModel* graphModel() const;
                void setFlowIndex(const QModelIndex& flowIndex);
                QModelIndex getFlowIndex() const
                {
                    return m_flowIndex;
                }

            private:
                void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int>& roles);
                //void onRowsAboutToBeInserted(const QModelIndex &parent, int first, int last);
                void onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
                void onRowsAboutToBeInserted(const QModelIndex &parent, int first, int last);
                void onRowsInserted(const QModelIndex &parent, int first, int last);
                void onRowsRemoved(const QModelIndex &parent, int first, int last);
                void onModelAboutToBeReset();
                void onModelReset();
                // std::pair<size_t, size_t> getRelatedOutPortInfoFromConnection(const QModelIndex& connectionIndex, bool& has) const;
                // QModelIndex getRelatedConnectionIndexFromPortAndConnection(const QModelIndex& nodeProxyIndex, size_t portIndex, size_t connectionIndex);
            private:
                QPersistentModelIndex m_flowIndex;
        };
    }
}
