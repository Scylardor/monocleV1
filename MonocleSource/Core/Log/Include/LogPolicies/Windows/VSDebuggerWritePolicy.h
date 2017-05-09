#ifndef MOE_VS_DEBUGGER_WRITE_POLICY_H_
#define MOE_VS_DEBUGGER_WRITE_POLICY_H_

// The Visual Studio Debugger write policy, to write log messages inside the Output Window of VS.
// Currently works only with std::strings.

#include "moeDLLVisibility.h"

#ifdef MOE_STD_SUPPORT
#include <string>

namespace moe
{
    class MOE_DLL_API VSDebuggerWritePolicy
    {
    public:
        static void Write(const std::string& message);
    };
}

#endif // MOE_STD_SUPPORT

#endif // MOE_VS_DEBUGGER_WRITE_POLICY_H_