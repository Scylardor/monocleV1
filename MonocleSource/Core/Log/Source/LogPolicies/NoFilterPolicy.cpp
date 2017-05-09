#include "LogPolicies/NoFilterPolicy.h"

namespace moe
{
    bool    NoFilterPolicy::Filter(const LogInfo&)
    {
        return true;
    }
}
