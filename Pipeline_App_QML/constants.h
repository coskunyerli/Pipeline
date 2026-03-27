#pragma once
#include "Models/constants.h"

namespace Pipeline
{
    namespace Runtime
    {
        enum NodeTypes
        {
            PythonNode = 0,
        };
        enum NodeModelRoles
        {
            NodeNameRole,
            NodeShortNameRole,
            NodeDescriptionRole,
            NodeTypeRole,
        };

        enum NodeRoles
        {
            PythonFileName = UI::Roles::PortConnectionIndexEnd + 100,
            PythonScript,
            PythonError,
            ActorAction,
            InputTableModel,
            NodeParameterListModel,
            OutputTableModel,
            NodeRunningState,
        };

        enum NodeTableRoles
        {
            HasTable = Qt::UserRole + 1,
            ChildCell,
            CellName,
            HeaderData = Qt::UserRole + 1000,
            HeaderDataEnd = HeaderData + 1000,
        };
        // --------------------
        // Roles
        // --------------------
        enum ParameterRoles
        {
            NameRole = Qt::UserRole + 1,
            TypeRole,
            ValueRole,
            TypeStringRole,
            IsBrowseRole
        };
    }
}
