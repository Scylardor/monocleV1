#ifndef MOE_ASSERT_H_
#define MOE_ASSERT_H_

#include "Core/Preprocessor/moeLikely.h"

// Monocle Assert
//---------------

// A debug assert used to break if the expression inside is false.
// Warning: this is used to DEBUG, which means it is STRIPPED in Shipping code.
// Thus, one needs to put extra care to put only additional checking code in the assert,
// leaving expressions we want to run in Shipping outside.

#ifndef MOE_SHIPPING
    #include "Core/Debugger/moeDebugger.h"
    #include "Core/Preprocessor/moeStringize.h"
    #include "Core/Preprocessor/moeDLLVisibility.h"

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
    #define MOE_ASSERT(expr) (void)0
    #define MOE_ENSURE(expr) (MOE_LIKELY(expr))
#endif // MOE_SHIPPING

#endif // MOE_ASSERT_H_
