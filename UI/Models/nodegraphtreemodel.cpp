#include "nodegraphtreemodel.h"
#include <QDebug>
namespace
{
    size_t indexOfPort(Pipeline::Core::Node* node, Pipeline::Core::Port* port, bool isIn, bool &has)
    {
        if (!node || !port)
        {
            has = false;
            return 0;
        }

        size_t portCount = isIn ? node->getInPortCount() : node->getOutPortCount();

        for (size_t i = 0; i < portCount; i++)
        {
            if (isIn)
            {
                if (node->getInPort(i) == port)
                {
                    has = true;
                    return i;
                }
            }
            else
            {
                if (node->getOutPort(i) == port)
                {
                    has = true;
                    return i;
                }
            }
        }

        has = false;
        return 0;
    }
}
namespace Pipeline
{
    namespace UI
    {
        // Constants
        // Bit positions and masks
        // constexpr int HAS_PORT_SHIFT = 31;
        // constexpr int IS_IN_PORT_SHIFT = 30;
        // constexpr int OPERATION_SHIFT = 27;
        // constexpr int PORT_INDEX_SHIFT = 20;
        // constexpr int NODE_INDEX_SHIFT = 13;

        // constexpr int OPERATION_MASK = 0x7;      // 3 bits (0–7)
        // constexpr int PORT_INDEX_MASK = 0x7F;    // 7 bits (0–127)
        // constexpr int NODE_INDEX_MASK = 0x7F;    // 7 bits (0–127)
        // Encode role from portIndex and connectedNodeIndex
        NodeGraphTreeModel::NodeGraphTreeModel(QObject *parent)
            : QAbstractItemModel(parent)
            , m_roleNames({})
        {
            m_rootNode = new MFlowNode();
        }

        QVariant NodeGraphTreeModel::data(const QModelIndex &index, int role) const
        {
            if (role == Constants::Roles::DataIndex)
            {
                return index;
            }

            if (role == Constants::Roles::Type)
            {
                ModelItemInterface *modelItemInterface = getData(index);

                if (!modelItemInterface)
                {
                    return Constants::DataType::None;
                }

                return modelItemInterface->getType();
            }

            if (role >= Constants::Roles::NodePortIndex && role < Constants::Roles::NodePortIndexEnd)
            {
                if (index.data(Constants::Roles::Type) != Constants::DataType::Node)
                {
                    return {};
                }

                MNode* node = getData<MNode>(index);

                if (node)
                {
                    int portIndex = role - Constants::Roles::NodePortIndex;
                    auto outPortCount = index.data(Constants::Roles::OutPortCount).toInt();
                    bool isIn = false;

                    if (portIndex >= outPortCount)
                    {
                        portIndex = portIndex - outPortCount;
                        isIn = true;
                    }

                    auto column = isIn ? Constants::ColumnNames::InPortColumn : Constants::ColumnNames::OutPortColumn;
                    return this->index(portIndex, 0, index.siblingAtColumn(column));
                }
            }

            if (role >= Constants::Roles::PortConnectionIndex && role < Constants::Roles::PortConnectionIndexEnd)
            {
                if (index.data(Constants::Roles::Type) != Constants::DataType::Port)
                {
                    return {};
                }

                MPort* port = getData<MPort>(index);

                //TODOJ, bunu teest et mutlaka, bu yapılmalı çünkü inporttan connection indexlerine ulaşmam lazım ki node posx değiştiğinde bütün connectionlar datachanged ile
                // güncellenecek
                if (port)
                {
                    int connectionIndex = role - Constants::Roles::PortConnectionIndex;
                    bool has;
                    auto *connection = port->getConnection(connectionIndex);
                    auto *outPort = dynamic_cast<MPort*>(connection->getOutPort());

                    if (!outPort)
                    {
                        return QModelIndex();
                    }

                    auto outConnectionIndex = outPort->findConnection(connection, has);

                    if (!has)
                    {
                        return QModelIndex();
                    }

                    size_t portIndex = indexOfPort(outPort->getOwnerNode(), outPort, false, has);

                    if (!has)
                    {
                        return QModelIndex();
                    }

                    auto portModelIndex = createIndex(static_cast<int>(portIndex), 0, outPort);
                    return this->index(static_cast<int>(outConnectionIndex), 0, portModelIndex);
                }
            }

            switch (role)
            {
                case Constants::Roles::ChildConnectionCount:
                    {
                        size_t total = 0;
                        int rows = rowCount(index);

                        for (int i = 0; i < rows; i++)
                        {
                            total += this->index(i, 0, index).data(Constants::Roles::OutConnectionCount).toLongLong();
                        }

                        return total;
                    }

                default:
                    break;
            }

            if (index.data(Constants::Roles::Type) == Constants::DataType::Node)
            {
                MNode* node = getData<MNode>(index);
                return node->data(role);
            }
            else
            {
                switch (role)
                {
                    case Constants::Roles::OutPortIndex:
                    case Constants::Roles::InPortIndex:
                    case Constants::Roles::InRelatedNode:
                    case Constants::Roles::InRelatedPort:
                    case Constants::Roles::OutRelatedNode:
                    case Constants::Roles::OutRelatedPort:
                        {
                            // For Connection Role names
                            auto* connection = getData<MConnection>(index);

                            if (index.data(Constants::Roles::Type) == Constants::DataType::Connection && connection)
                            {
                                switch (role)
                                {
                                    case Constants::Roles::OutPortIndex:
                                        {
                                            auto *outPort = connection->getOutPort();

                                            if (outPort)
                                            {
                                                return index.parent();
                                            }

                                            return QModelIndex();
                                        }

                                    case Constants::Roles::InPortIndex:
                                        {
                                            Core::Port *inPort = connection->getInPort();

                                            if (inPort && inPort->getOwnerNode())
                                            {
                                                bool has;
                                                size_t portIndex = indexOfPort(inPort->getOwnerNode(), inPort, true, has);

                                                if (has)
                                                {
                                                    return createIndex(static_cast<int>(portIndex), 0, dynamic_cast<MPort*>(inPort));
                                                }
                                            }

                                            return QModelIndex();
                                        }

                                    case Constants::Roles::InRelatedPort:
                                        {
                                            if (auto *inPort = connection->getInPort())
                                            {
                                                if (MNode *ownerNode = dynamic_cast<MNode*>(inPort->getOwnerNode()))
                                                {
                                                    for (int p = 0; p < ownerNode->getInPortCount(); p++)
                                                    {
                                                        if (inPort == ownerNode->getInPort(p))
                                                        {
                                                            return createIndex(static_cast<int>(p), 0, inPort);
                                                        }
                                                    }
                                                }
                                            }

                                            return QModelIndex();
                                        }

                                    case Constants::Roles::InRelatedNode:
                                        {
                                            if (auto *inPort = connection->getInPort())
                                            {
                                                if (MNode *ownerNode = dynamic_cast<MNode*>(inPort->getOwnerNode()))
                                                {
                                                    bool has;
                                                    size_t nodeIndex = ownerNode->parent()->indexOf(ownerNode, has);

                                                    if (has)
                                                    {
                                                        return createIndex(static_cast<int>(nodeIndex), 0, ownerNode);
                                                    }
                                                }
                                            }

                                            return QModelIndex();
                                        }

                                    case Constants::Roles::OutRelatedNode:
                                        {
                                            return index.parent().parent().siblingAtColumn(Constants::ColumnNames::NodeColumn);
                                        }

                                    case Constants::Roles::OutRelatedPort:
                                        {
                                            return index.parent();
                                        }
                                }
                            }

                            break;
                        }
                    case Constants::Roles::PortName:
                    case Constants::Roles::RelatedNode:
                    case Constants::Roles::ConnectionCount:
                    case Constants::Roles::PortIsIn:
                    case Constants::Roles::HasConnection:
                        {
                            // for port role names
                            auto* port = getData<MPort>(index);

                            if (index.data(Constants::Roles::Type) == Constants::DataType::Port && port)
                            {
                                switch (role)
                                {
                                    case Constants::Roles::RelatedNode:
                                        {
                                            return index.parent().siblingAtColumn(Constants::ColumnNames::NodeColumn);
                                        }

                                    case Constants::Roles::ConnectionCount:
                                        {
                                            return port->getConnectionCount();
                                        }

                                    case Constants::Roles::PortIsIn:
                                        {
                                            return index.parent().column() == Constants::ColumnNames::InPortColumn ? true : false;
                                        }

                                    case Constants::Roles::PortName:
                                        {
                                            return QString::fromStdString(port->getName());
                                        }

                                    case Constants::Roles::HasConnection:
                                        {
                                            return port->getConnectionCount() > 0;
                                        }
                                }
                            }

                            break;
                        }
                }
            }

            return {};
        }

        int NodeGraphTreeModel::rowCount(const QModelIndex &parent) const
        {
            switch (data(parent, Constants::Roles::Type).toInt())
            {
                case Constants::DataType::Node:
                    {
                        if (parent.column() == Constants::ColumnNames::OutPortColumn)
                        {
                            return data(parent, Constants::Roles::OutPortCount).toInt();
                        }
                        else if (parent.column() == Constants::ColumnNames::InPortColumn)
                        {
                            return data(parent, Constants::Roles::InPortCount).toInt();
                        }
                        else
                        {
                            if (auto* flowNode = dynamic_cast<MFlowNode*>(getData(parent)))
                            {
                                return static_cast<int>(flowNode->getChildNodeCount());
                            }

                            return 0;
                        }
                    }

                case Constants::DataType::Port:
                    {
                        if (auto *port = getData<MPort>(parent))
                        {
                            return static_cast<int>(port->getConnectionCount());
                        }

                        return 0;
                    }

                default:
                    return 0;
            }
        }

        int NodeGraphTreeModel::columnCount(const QModelIndex &parent) const
        {
            // TODOj parent node could be ports or could be port or could be Node,
            // if parent node is node Node column count should be 3 one is for child nodes, one is out port one is in port
            // otherwise should be 1
            if (data(parent, Constants::Roles::Type).toInt() == Constants::DataType::Node)
            {
                return int(3);
            }

            return int(1);
        }

        QHash<int, QByteArray> NodeGraphTreeModel::roleNames() const
        {
            return m_roleNames;
        }

        QModelIndex NodeGraphTreeModel::index(int row, int column, const QModelIndex &parent) const
        {
            switch (data(parent, Constants::Roles::Type).toInt())
            {
                case Constants::DataType::Node:
                    {
                        if (parent.column() == Constants::ColumnNames::OutPortColumn)
                        {
                            if (MNode *node = getData<MNode>(parent))
                            {
                                if (row < node->getOutPortCount())
                                    return createIndex(row, column, dynamic_cast<MPort*>(node->getOutPort(row)));
                            }
                        }
                        else if (parent.column() == Constants::ColumnNames::InPortColumn)
                        {
                            if (MNode *node = getData<MNode>(parent))
                            {
                                if (row < node->getInPortCount())
                                    return createIndex(row, column, dynamic_cast<MPort*>(node->getInPort(row)));
                            }
                        }
                        else
                        {
                            if (MFlowNode* flowNode = getData<MFlowNode>(parent))
                            {
                                if (row < flowNode->getChildNodeCount())
                                    return createIndex(row, column, dynamic_cast<MNode*>(flowNode->getNode(row)));
                            }
                        }

                        return QModelIndex();
                    }

                case Constants::DataType::Port:
                    {
                        if (parent.data(Constants::Roles::PortIsIn).toBool())
                        {
                            return QModelIndex();
                        }

                        if (auto *port = getData<MPort>(parent))
                        {
                            Core::Connection *connection = port->getConnection(row);

                            if (connection)
                                return createIndex(row, column,  dynamic_cast<MConnection*>(connection));
                        }

                        return QModelIndex();
                    }

                default:
                    return QModelIndex();
            }
        }

        QModelIndex NodeGraphTreeModel::parent(const QModelIndex &child) const
        {
            int type = data(child, Constants::Roles::Type).toInt();

            switch (type)
            {
                case Constants::DataType::Node:
                    {
                        if (MNode *node = getData<MNode>(child))
                        {
                            if (!node->parent())
                            {
                                return QModelIndex();
                            }

                            MFlowNode* parentNode = dynamic_cast<MFlowNode*>(node->parent());

                            if (!parentNode->parent())
                            {
                                return QModelIndex();
                            }

                            bool has;
                            size_t parentIndex = parentNode->parent()->indexOf(parentNode, has);

                            if (!has)
                            {
                                return QModelIndex();
                            }

                            return createIndex(static_cast<int>(parentIndex), 0, parentNode);
                        }
                        else
                        {
                            return QModelIndex();
                        }
                    }

                case Constants::DataType::Port:
                    {
                        if (Core::Port *port = getData<MPort>(child))
                        {
                            MNode *ownerNode = dynamic_cast<MNode*>(port->getOwnerNode());

                            if (!ownerNode)
                            {
                                return QModelIndex();
                            }

                            if (!ownerNode->parent())
                            {
                                return QModelIndex();
                            }

                            bool has;
                            size_t nodeIndex = ownerNode->parent()->indexOf(ownerNode, has);

                            if (!has)
                            {
                                return QModelIndex();
                            }

                            indexOfPort(ownerNode, port, true, has);

                            if (has)
                            {
                                return createIndex(static_cast<int>(nodeIndex), Constants::ColumnNames::InPortColumn, ownerNode);
                            }

                            indexOfPort(ownerNode, port, false, has);

                            if (has)
                            {
                                return createIndex(static_cast<int>(nodeIndex), Constants::ColumnNames::OutPortColumn, ownerNode);
                            }

                            return QModelIndex();
                        }

                        return QModelIndex();
                    }

                case Constants::DataType::Connection:
                    {
                        if (auto *connection = getData<MConnection>(child))
                        {
                            Core::Port *port = connection->getOutPort();

                            if (!port)
                            {
                                return QModelIndex();
                            }

                            auto *ownerNode = port->getOwnerNode();

                            if (!ownerNode)
                            {
                                return QModelIndex();
                            }

                            bool has;
                            size_t portIndex = indexOfPort(ownerNode, port, false, has);

                            if (!has)
                            {
                                return QModelIndex();
                            }

                            return createIndex(static_cast<int>(portIndex), 0, dynamic_cast<MPort*>(port));
                        }

                        return QModelIndex();
                    }

                default:
                    return QModelIndex();
            }
        }

        bool NodeGraphTreeModel::addConnection(const QModelIndex inPortIndex, const QModelIndex outPortIndex)
        {
            if (inPortIndex.data(Constants::Roles::Type) != Constants::DataType::Port || outPortIndex.data(Constants::Roles::Type) != Constants::DataType::Port)
            {
                return false;
            }

            auto* inPort = getData<MPort>(inPortIndex);
            auto* outPort = getData<MPort>(outPortIndex);

            if (!inPort || !outPort)
            {
                return false;
            }

            this->beginInsertRows(outPortIndex, outPortIndex.data(Constants::Roles::ConnectionCount).toInt(), outPortIndex.data(Constants::Roles::ConnectionCount).toInt());
            outPort->connect(inPort);
            this->endInsertRows();
            auto inPortRelatedNode = inPortIndex.data(Constants::Roles::RelatedNode).toModelIndex();
            auto outPortRelatedNode = outPortIndex.data(Constants::Roles::RelatedNode).toModelIndex();
            auto* inPortNode = getData<MNode>(inPortRelatedNode);
            auto* outPortNode = getData<MNode>(outPortRelatedNode);

            if (inPortNode)
            {
                inPortNode->inConnectionChanged(inPort, outPort);
            }

            if (outPortNode)
            {
                outPortNode->outConnectionChanged(outPort, inPort);
            }

            emit dataChanged(inPortIndex, inPortIndex, {Constants::Roles::HasConnection});
            emit dataChanged(outPortIndex, outPortIndex, {Constants::Roles::HasConnection});
            emit dataChanged(inPortRelatedNode, inPortRelatedNode, {Constants::Roles::InConnectionCount, Constants::Roles::ChildConnectionCount});
            emit dataChanged(outPortRelatedNode, outPortRelatedNode, {Constants::Roles::OutConnectionCount, Constants::Roles::ChildConnectionCount});
            return true;
        }

        bool NodeGraphTreeModel::addNode(MNode* node, const QModelIndex &parent)
        {
            if (!node)
            {
                return false;
            }

            if (MFlowNode* flowNode = getData<MFlowNode>(parent))
            {
                int count = static_cast<int>(flowNode->getChildNodeCount());
                this->beginInsertRows(parent, count, count);
                flowNode->addNode(node);
                node->notifyChanged =
                    [node, this](QList<int> roles)
                {
                    QModelIndex idx = getIndexFromItem(node);

                    if (idx.isValid())
                    {
                        emit dataChanged(idx, idx, roles);
                    }
                };
                this->endInsertRows();
                return true;
            }

            return false;
        }

        bool NodeGraphTreeModel::addPort(MPort *port, const QModelIndex &index, bool isIn)
        {
            if (index.data(Constants::Roles::Type) != Constants::DataType::Node)
            {
                return false;
            }

            MNode *node = getData<MNode>(index);

            if (!node)
            {
                return false;
            }

            int columnIndex = isIn ? Constants::ColumnNames::InPortColumn : Constants::ColumnNames::OutPortColumn;
            size_t portCount = isIn ? node->getInPortCount() : node->getOutPortCount();
            this->beginInsertRows(index.siblingAtColumn(columnIndex), static_cast<int>(portCount), static_cast<int>(portCount));
            node->addPort(port, isIn);
            this->endInsertRows();
            emit this->dataChanged(index.siblingAtColumn(columnIndex), index.siblingAtColumn(columnIndex), {isIn ? Constants::Roles::InPortCount : Constants::Roles::OutPortCount});
            return true;
        }

        bool NodeGraphTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
        {
            switch (data(index, Constants::Roles::Type).toInt())
            {
                case Constants::DataType::Node:
                    {
                        MNode* node = getData<MNode>(index);
                        auto roles = node->roleNames();

                        if (roles.contains(role))
                        {
                            bool res = node->setData(value, role);

                            if (res)
                            {
                                emit dataChanged(index, index, {role});
                            }

                            return res;
                        }
                    }
            }

            return false;
        }

        ModelItemInterface* NodeGraphTreeModel::getData(const QModelIndex &index) const
        {
            if (!index.isValid())
            {
                return m_rootNode;
            }

            return static_cast<ModelItemInterface*>(index.internalPointer());
        }

        QModelIndex NodeGraphTreeModel::getIndexFromItem(MNode *node)
        {
            if (MFlowNode* flowNode = dynamic_cast<MFlowNode*>(node->parent()))
            {
                bool has;
                size_t row = flowNode->indexOf(node, has);

                if (has && row < flowNode->getChildNodeCount())
                    return createIndex(static_cast<int>(row), 0, node);
            }

            return QModelIndex();
        }

        Core::Connection* MPort::connect(Port *other)
        {
            if (hasConnection(other))
            {
                return nullptr;
            }

            auto *connection = new MConnection(other, this);
            addConnection(connection);
            other->addConnection(connection);
            return connection;
        }

        bool MNode::setData(const QVariant &value, int role, bool emitSignal)
        {
            bool res;
            bool result = false;

            switch (role)
            {
                case Constants::Roles::PosX:
                    {
                        int posX = value.toInt(&res);
                        this->setX(posX);
                        result = true;
                        break;
                    }

                case Constants::Roles::PosY:
                    {
                        int posY = value.toInt(&res);
                        this->setY(posY);
                        result = true;
                        break;
                    }

                case Constants::Roles::Name:
                    {
                        QString name = value.toString();
                        this->setName(name.toStdString());
                        result = true;
                        break;
                    }
            }

            if (result && emitSignal)
            {
                notifyChanged({role});
            }

            return result;
        }

        QVariant MNode::data(int role) const
        {
            switch (role)
            {
                case Qt::DisplayRole:
                case Constants::Roles::Name:
                    return QString::fromStdString(this->getName());

                case Constants::Roles::InConnectionCount:
                    {
                        return this->findInConnectionCount();
                    }

                case Constants::Roles::OutConnectionCount:
                    {
                        return this->findOutConnectionCount();
                    }

                case Constants::Roles::InPortCount:
                    {
                        return this->getInPortCount();
                    }

                case Constants::Roles::OutPortCount:
                    {
                        return this->getOutPortCount();
                    }

                case Constants::Roles::PosX:
                    {
                        return this->getX();
                    }

                case Constants::Roles::PosY:
                    {
                        return this->getY();
                    }
            }

            return {};
        }

    }
}

