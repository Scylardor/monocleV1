#ifndef MOE_ASSERT_H_
#define MOE_ASSERT_H_

#include "moeLikely.h"

#ifndef MOE_SHIPPING
    #include "moeDebugger.h"
    #include "moeStringize.h"
    #include "moeDLLVisibility.h"

namespace moe
{
    // A dummy wrapper function whose only purpose is to return a false value to use within MOE_ENSURE.
    bool    MOE_DLL_API AssertErrorReturnFalse(const char* const file, int line, const char* msg);
}

    #define MOE_ASSERT(expr)                                                                            \
        {                                                                                               \
            if (MOE_UNLIKELY(!(expr)))                                                                  \
            {                                                                                           \
                moe::AssertErrorReturnFalse(__FILE__, __LINE__, MOE_STRINGIZE(expr) ": ASSERT FAILED!");\
            }                                                                                           \
        }

    // A-la-Unreal assertion you can use in an if. Avoids to assert + check the same condition just after
    #define MOE_ENSURE(expr) (MOE_LIKELY(expr) || moe::AssertErrorReturnFalse(__FILE__, __LINE__, MOE_STRINGIZE(expr) ": ENSURE FAILED!"))
#else
    #define MOE_ASSERT(cond) (void)0
    #define MOE_ENSURE(expr) (MOE_LIKELY(expr))
#endif // MOE_SHIPPING

#endif // MOE_ASSERT_H_
