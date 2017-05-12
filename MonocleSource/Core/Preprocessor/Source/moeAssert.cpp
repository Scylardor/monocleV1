#ifndef MOE_SHIPPING

#include "moeAssert.h"
#include "moeLoggerBase.h"
#include "moeLogMacros.h"

namespace moe
{
    bool    AssertErrorReturnFalse(const char* const file, int line, const char* msg)
    {
        MOE_DEFAULT_LOG_EXPAND(moe::ChanDebug, moe::SevError, file, line, msg);
        #ifndef MOE_PROFILE
            moe::DebugBreak();
        #endif
        return false;
    }
}

#endif // MOE_SHIPPING
