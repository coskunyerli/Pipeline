#pragma once
#include <QAbstractItemModel>
#include "flownode.h"
#include "port.h"
#include "connection.h"
#include "constants.h"

namespace Pipeline
{
    namespace UI
    {
        class ModelItemInterface
        {
            public:
                explicit ModelItemInterface(DataType type)
                    : m_type(type)
                {}

                virtual ~ModelItemInterface() {}
                virtual DataType getType() const
                {
                    return m_type;
                };
            protected:
                DataType m_type;

        };

        class MNode : public ModelItemInterface, public Core::Node
        {
            public:
                explicit MNode()
                    : ModelItemInterface(DataType::Node)
                    , Core::Node()

                {
                }
                virtual ~MNode()
                {
                }
                virtual QHash<int, QByteArray> roleNames() const
                {
                    return
                    {
                        {Roles::Name, "name"},
                        {Roles::DataIndex, "dataIndex"},
                        {Roles::Type, "type"},
                        {Roles::PosX, "posX"},
                        {Roles::PosY, "posY"},
                        {Roles::InConnectionCount, "inConnectionCount"},
                        {Roles::OutConnectionCount, "outConnectionCount"},
                        {Roles::ChildConnectionCount, "childConnectionCount"},
                        {Roles::InPortCount, "inPortCount"},
                        {Roles::OutPortCount, "outPortCount"},
                        {Roles::OutPortIndex, "outPortIndex"},
                        {Roles::InPortIndex, "inPortIndex"},
                        {Roles::InRelatedNode, "inRelatedNode"},
                        {Roles::OutRelatedNode, "outRelatedNode"},
                        {Roles::RelatedNode, "relatedNode"},
                        {Roles::ConnectionCount, "connectionCount"},
                        {Roles::HasConnection, "hasConnection"},
                    };
                }
                virtual bool setData(const QVariant &value, int role);
                virtual QVariant data(int role) const;

        };

        class MFlowNode : public ModelItemInterface, public Core::FlowNode
        {
            public:
                explicit MFlowNode()
                    : ModelItemInterface(DataType::Node)
                    , Core::FlowNode()
                {
                }
                virtual ~MFlowNode()
                {
                }
        };


        class MConnection : public ModelItemInterface, public Core::Connection
        {
            public:
                explicit MConnection(Core::Port* in, Core::Port *out)
                    : ModelItemInterface(DataType::Connection)
                    , Core::Connection(in, out)
                {
                }
                virtual ~MConnection()
                {
                }
        };

        class MPort : public ModelItemInterface, public Core::Port
        {
            public:
                explicit MPort(Core::Node* node)
                    : ModelItemInterface(DataType::Port)
                    , Core::Port(node)
                {
                }
                virtual ~MPort()
                {
                }

                Core::Connection* connect(Port *other) override;
        };
        class NodeGraphTreeModel : public QAbstractItemModel
        {
                Q_OBJECT
            public:
                explicit NodeGraphTreeModel(QObject *parent = nullptr);
                QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
                int rowCount(const QModelIndex &parent = QModelIndex()) const override;
                int columnCount(const QModelIndex &parent = QModelIndex()) const override;
                QHash<int, QByteArray> roleNames() const override;
                QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
                QModelIndex parent(const QModelIndex &child) const override;

                template <typename T>
                void registerNodeType()
                {
                    this->beginResetModel();
                    T t;
                    auto roles = t.roleNames();

                    for (auto &key : roles.keys())
                    {
                        m_roleNames[key] = roles[key];
                    }

                    this->endResetModel();
                }
                Q_INVOKABLE bool addConnection(const QModelIndex inPortIndex, const QModelIndex outPortIndex);
                Q_INVOKABLE bool addNode(MNode* node, const QModelIndex &parent = QModelIndex());
                Q_INVOKABLE bool addPort(MPort *port, const QModelIndex& index, bool isIn);
                Q_INVOKABLE bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
                template <typename T>
                T* getData(const QModelIndex &index) const
                {
                    ModelItemInterface *interface = m_rootNode;

                    if (!index.isValid())
                    {
                        interface = m_rootNode;
                    }
                    else
                    {
                        interface = static_cast<ModelItemInterface*>(index.internalPointer());
                    }

                    return dynamic_cast<T*>(interface);
                }

                ModelItemInterface* getData(const QModelIndex &index) const;

            private:
                MFlowNode* m_rootNode;
                QHash<int, QByteArray> m_roleNames;
        };
    }
}
