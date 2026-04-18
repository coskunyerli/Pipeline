#pragma once
#include "Models/constants.h"

namespace Pipeline
{
    namespace Runtime
    {
        namespace Constants
        {
            Q_NAMESPACE
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
            Q_ENUM_NS(NodeModelRoles)

            enum NodeRoles
            {
                PythonFileName = UI::Constants::Roles::PortConnectionIndexEnd + 100,
                PythonScript,
                PythonError,
                ActorAction,
                InputTableModel,
                NodeParameterListModel,
                OutputTableModel,
                NodeRunningState,
            };
            Q_ENUM_NS(NodeRoles)

            enum NodeTableRoles
            {
                HasTable = Qt::UserRole + 1,
                ChildCell,
                CellName,
                Rows,
                Columns,
                HeaderData = Qt::UserRole + 1000,
                HeaderDataEnd = HeaderData + 1000,
            };
            Q_ENUM_NS(NodeTableRoles)
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
            Q_ENUM_NS(ParameterRoles)
        }

    }
}
