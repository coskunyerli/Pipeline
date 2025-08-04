#pragma once
#include <QIdentityProxyModel>
#include "nodeuimanager.h"
#include "portgraphviewmodel.h"
#include <QHash>
namespace Pipeline
{
    namespace UI
    {
        class NodeGraphViewModel : public QIdentityProxyModel
        {
                Q_OBJECT
                Q_PROPERTY(NodeIUManager* uiManager READ uiManager WRITE setUIManager NOTIFY uiManagerChanged)
            public:
                enum NodeModelRoles
                {
                    NodeWidth = 1000,
                    NodeHeight,
                    PortModel
                };
                explicit NodeGraphViewModel(QObject* parent = nullptr);
                Q_INVOKABLE bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
                QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override;

                NodeIUManager* uiManager()
                {
                    return m_uiManager;
                }
                void setUIManager(NodeIUManager *manager);

                PortGraphViewModel* inPortModel(const QModelIndex& nodeIndex) const;
                PortGraphViewModel* outPortModel(const QModelIndex& nodeIndex) const;

                QHash<int, QByteArray> roleNames() const override;
            signals:
                void uiManagerChanged();
            private:
                NodeIUManager* m_uiManager;
        };
    }
}
