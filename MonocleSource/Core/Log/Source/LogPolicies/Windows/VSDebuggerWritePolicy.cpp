#include "LogPolicies/Windows/VSDebuggerWritePolicy.h"
#include <Windows.h>

namespace moe
{
    void    VSDebuggerWritePolicy::Write(const std::string& message)
    {
        OutputDebugStringA(message.c_str());

        // Ensure a newline feed is at the end to make sure messages are on their own line
        char lastChar = message[message.length() - 1];
        if (lastChar != '\n')
        {
            OutputDebugStringA("\n");
        }
    }
}
