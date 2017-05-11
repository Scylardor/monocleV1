#ifndef MOE_ASSERT_H_
#define MOE_ASSERT_H_

#ifndef MOE_SHIPPING
    #include "moeDebugger.h"
    #include "moeJoin.h"
    #include "moeDLLVisibility.h"

namespace moe
{
    // A dummy wrapper function whose only purpose is to return a false value to use within MOE_ENSURE.
    bool    MOE_DLL_API AssertErrorReturnFalse(const char* const file, int line, char* const msg);
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
    // ENSURE are purely debug checks. In shipping, we should be able to assume they'll ALWAYS be true.
    // Still looks a bit risky to me. We'll see
    #define MOE_ENSURE(expr) (true)
#endif // MOE_SHIPPING

#endif // MOE_ASSERT_H_
