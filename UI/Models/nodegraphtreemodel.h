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
        class MPort;
        class ModelItemInterface
        {
            public:
            explicit ModelItemInterface(Constants::DataType type)
                    : m_type(type)
                {}

                virtual ~ModelItemInterface() {}
                virtual Constants::DataType getType() const
                {
                    return m_type;
                };
            protected:
                Constants::DataType m_type;

        };

        class MNode : public ModelItemInterface, public Core::Node
        {
            public:
                explicit MNode()
                    : ModelItemInterface(Constants::DataType::Node)
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
                        {Constants::Roles::Name, "name"},
                        {Constants::Roles::DataIndex, "dataIndex"},
                        {Constants::Roles::Type, "type"},
                        {Constants::Roles::PosX, "posX"},
                        {Constants::Roles::PosY, "posY"},
                        {Constants::Roles::InConnectionCount, "inConnectionCount"},
                        {Constants::Roles::OutConnectionCount, "outConnectionCount"},
                        {Constants::Roles::ChildConnectionCount, "childConnectionCount"},
                        {Constants::Roles::InPortCount, "inPortCount"},
                        {Constants::Roles::OutPortCount, "outPortCount"},
                        {Constants::Roles::OutPortIndex, "outPortIndex"},
                        {Constants::Roles::InPortIndex, "inPortIndex"},
                        {Constants::Roles::InRelatedNode, "inRelatedNode"},
                        {Constants::Roles::OutRelatedNode, "outRelatedNode"},
                        {Constants::Roles::RelatedNode, "relatedNode"},
                        {Constants::Roles::ConnectionCount, "connectionCount"},
                        {Constants::Roles::HasConnection, "hasConnection"},
                    };
                }
                std::function<void(QList<int> roles)> notifyChanged;

                virtual bool setData(const QVariant &value, int role, bool emitSignal = false);
                virtual QVariant data(int role) const;
                virtual void inConnectionChanged(MPort *inPort, MPort* outPort) {}
                virtual void outConnectionChanged(MPort *outPort, MPort* inPort) {}

        };

        class MFlowNode : public ModelItemInterface, public Core::FlowNode
        {
            public:
                explicit MFlowNode()
                    : ModelItemInterface(Constants::DataType::Node)
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
                    : ModelItemInterface(Constants::DataType::Connection)
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
                explicit MPort()
                    : ModelItemInterface(Constants::DataType::Port)
                    , Core::Port(nullptr)
                {
                }
                explicit MPort(Core::Node* node)
                    : ModelItemInterface(Constants::DataType::Port)
                    , Core::Port(node)
                {
                }
                virtual ~MPort()
                {
                }

                virtual QHash<int, QByteArray> roleNames() const
                {
                    return
                    {
                        {Constants::Roles::PortName, "portName"},
                        {Constants::Roles::RelatedNode, "portRelatedNode"},
                        {Constants::Roles::ConnectionCount, "portConnectionCount"},
                        {Constants::Roles::HasConnection, "portHasConnection"},
                        {Constants::Roles::PortIsIn, "portIsIn"},
                    };
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
                QModelIndex getIndexFromItem(MNode* node);

            private:
                MFlowNode* m_rootNode;
                QHash<int, QByteArray> m_roleNames;
        };
    }
}
