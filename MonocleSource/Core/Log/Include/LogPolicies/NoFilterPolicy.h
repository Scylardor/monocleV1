#ifndef MOE_NO_FILTER_POLICY_H_
#define MOE_NO_FILTER_POLICY_H_

// A logging filter-policy that, as its name says, doesn't filter anything.
#include "moeDLLVisibility.h"

#include "LogUtils.h"

namespace moe
{
    class MOE_DLL_API NoFilterPolicy
    {
    public:
		NoFilterPolicy() {}

        static bool Filter(const LogInfo& logInfo);
    };
}

#endif // MOE_NO_FILTER_POLICY_H_