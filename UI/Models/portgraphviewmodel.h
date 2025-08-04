#pragma once
#include <QAbstractProxyModel>
#include "nodeuimanager.h"

namespace Pipeline
{
    //TODOJ Port yazılacak ilk olarak bu kaldı
    namespace UI
    {
        class PortGraphViewModel : public QAbstractProxyModel
        {
                Q_OBJECT
                Q_PROPERTY(NodeIUManager* uiManager READ uiManager WRITE setUIManager NOTIFY uiManagerChanged)
                Q_PROPERTY(QModelIndex nodeIndex READ nodeIndex WRITE setNodeIndex NOTIFY nodeIndexChanged)
                Q_PROPERTY(bool isInPort READ isInPort WRITE setIsInPort NOTIFY isInPortChanged)

            public:
                enum PortModelRoles
                {
                    PortX = 2000,
                    PortY,
                    PortWidth,
                    PortHeight
                };
                explicit PortGraphViewModel(QObject *parent = nullptr);
                QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
                QModelIndex parent(const QModelIndex &child) const override;
                QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override;

                QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
                QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
                int columnCount(const QModelIndex &parent = QModelIndex()) const override;
                int rowCount(const QModelIndex &parent = QModelIndex()) const override;
                void setSourceModel(QAbstractItemModel *model) override;
                QHash<int, QByteArray> roleNames() const override;

                void setNodeIndex(const QModelIndex& flowIndex);
                void setIsInPort(bool res);
                QModelIndex nodeIndex() const
                {
                    return m_nodeIndex;
                }

                NodeIUManager* uiManager()
                {
                    return m_uiManager;
                }

                bool isInPort() const
                {
                    return m_isInPort;
                }
                void setUIManager(NodeIUManager *manager);
            signals:
                void uiManagerChanged();
                void nodeIndexChanged();
                void isInPortChanged();
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
                NodeIUManager* m_uiManager;
                QPersistentModelIndex m_nodeIndex;
                bool m_isInPort;
        };
    }
}
