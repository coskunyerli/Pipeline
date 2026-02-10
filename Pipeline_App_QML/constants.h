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
            TableModel
        };
    }
}
