#pragma once
#include "Models/constants.h"

namespace Pipeline
{
    namespace Runtime
    {
        enum NodeRoles
        {
            PythonFileName = UI::Roles::PortConnectionIndexEnd + 100,
            PythonScript,
            PythonError,
            ActorAction,
            InputTableModel,
            InputParameterTableModel,
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
    }
}
