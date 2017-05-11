#ifndef MOE_ASSERT_H_
#define MOE_ASSERT_H_

#ifndef MOE_SHIPPING
    #include "moeDebugger.h"
    #include "moeJoin.h"
    #include "moeLogMacros.h"

namespace moe
{
    // A dummy wrapper function whose only purpose is to return a false value to use within MOE_ENSURE.
    bool    AssertErrorReturnFalse(const char* const file, int line, char* const msg)
    {
        MOE_DEFAULT_LOG_EXPAND(moe::ChanDebug, moe::SevError, file, line, msg);
        moe::DebugBreak();
        return false;
    }
}

    #define MOE_ASSERT(expr)                                                                            \
        {                                                                                               \
            if (!(expr))                                                                                \
            {                                                                                           \
                moe::AssertErrorReturnFalse(__FILE__, __LINE__, MOE_JOIN(#expr, ": ASSERT FAILED!"));   \
            }                                                                                           \
        }

    // A-la-Unreal assertion you can use in an if. Avoids to assert + check the same condition just after
    #define MOE_ENSURE(expr) (expr || moe::AssertErrorReturnFalse(__FILE__, __LINE__, MOE_JOIN(#expr, ": ENSURE FAILED!")))
#else
    #define MOE_ASSERT(cond) (void)0
    #define MOE_ENSURE(expr) (expr) // TODO: branch prediction hint ?
#endif // MOE_SHIPPING

#endif // MOE_ASSERT_H_
