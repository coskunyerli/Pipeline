#pragma once
#include <QIdentityProxyModel>
#include "nodeuimanager.h"
namespace Pipeline
{
    namespace UI
    {
        class ConnectionGraphViewModel : public QIdentityProxyModel
        {
                Q_OBJECT
                Q_PROPERTY(NodeIUManager* uiManager READ uiManager WRITE setUIManager NOTIFY uiManagerChanged)
            public:
                enum ConnectionRoles
                {
                    ConnectionInX = 1000,
                    ConnectionInY,
                    ConnectionOutX,
                    ConnectionOutY,

                };
                explicit ConnectionGraphViewModel(QObject* parent = nullptr);
                QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override;
                void setSourceModel(QAbstractItemModel *model) override;

                NodeIUManager* uiManager()
                {
                    return m_uiManager;
                }
                void setUIManager(NodeIUManager *manager);
                QHash<int, QByteArray> roleNames() const override;
            signals:
                void uiManagerChanged();
            private:
                void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int>& roles);
            private:
                NodeIUManager* m_uiManager;
        };
    }
}
