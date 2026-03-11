#include "stringhelper.h"

namespace Pipeline::Runtime
{

    std::string StringHelper::indexToString(int index)
    {
        std::string result;

        while (index >= 0)
        {
            int remainder = index % 26;
            result = char('A' + remainder) + result;
            index = index / 26 - 1;
        }

        return result;
    }

}
