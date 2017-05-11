#ifndef MOE_DEBUGGER_H_
#define MOE_DEBUGGER_H_

#include "moeDLLVisibility.h"

namespace moe
{
    bool    IsDebuggerPresent();

    void    MOE_DLL_API DebugBreak();
}

#endif // MOE_DEBUGGER_H_
