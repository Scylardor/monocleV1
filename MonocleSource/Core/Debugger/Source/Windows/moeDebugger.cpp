#ifndef MOE_SHIPPING

#include <Windows.h>
#include "moeDebugger.h"

// Windows version of Monocle debugger utilities
// (basically a wrapper of windows API)

namespace moe
{
    bool    IsDebuggerPresent()
    {
        // comparing to 0 to avoid warning C4800: 'BOOL' : forcing value to bool 'true' or 'false' (performance warning)
        return (::IsDebuggerPresent() != 0);
    }

    void    DebugBreak()
    {
        if (IsDebuggerPresent())
        {
            ::DebugBreak();
        }
    }
}

#endif // MOE_SHIPPING
