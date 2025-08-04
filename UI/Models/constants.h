#pragma once
#include <QAbstractItemModel>

namespace Pipeline
{
    namespace UI
    {
        enum DataType
        {
            None,
            Node,
            Port,
            Connection
        };

        enum ColumnNames
        {
            NodeColumn = 0,
            OutPortColumn = 1,
            InPortColumn = 2
        };
        enum Roles
        {
            // Generic Roles
            // Node roles
            Name = Qt::UserRole + 1,
            DataIndex,
            Type,
            PosX,
            PosY,
            InConnectionCount,
            OutConnectionCount,
            ChildConnectionCount,
            InPortCount,
            OutPortCount,
            // Connection Roles
            OutPortIndex,
            InPortIndex,
            InRelatedNode,
            InRelatedPort,
            OutRelatedPort,
            OutRelatedNode,
            // Port Rules
            RelatedNode,
            ConnectionCount,
            HasConnection,
            PortIsIn,
            // This is for get port index in node NodePortIndex is getting first port in given node NodePortIndex + 1 is sec port index. ports are flatten outport + inports.
            // first outports then inports
            NodePortIndex = PortIsIn + 50,
            NodePortIndexEnd = NodePortIndex + 50,
            // this is for get connection in port for example PortConnectionIndex is getting first connection in given port PortConnectionIndex + 1 is sec connection index
            PortConnectionIndex = NodePortIndexEnd + 1,
            PortConnectionIndexEnd = PortConnectionIndex + 50
        };

        enum ConnectionGraphRoles
        {
            OutRelatedNodePosX = Roles::PortConnectionIndexEnd + 1,
            OutRelatedNodePosY,
            InRelatedNodePosX,
            InRelatedNodePosY
        };
    }
}
