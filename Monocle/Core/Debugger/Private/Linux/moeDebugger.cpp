#ifndef MOE_SHIPPING

#include <cstdio>   // FILE* ops
#include <cstring>  // strstr
#include <cstdlib>  // atoi
#include "Core/moeDebugger.h"

// Linux version of Monocle debugger utilities

namespace moe
{
    // The idea on Linux is to read the status file of current process and see if a tracer PID is present in it (as GDB would write in it).
    // Credits to Sam Liao on http://stackoverflow.com/questions/3596781/how-to-detect-if-the-current-process-is-being-run-by-gdb
    // But using a fopen+read combo is guaranteed to be a performance hit, especially if called often...
    bool    IsDebuggerPresent()
    {
        std::FILE* statusFileHandle = std::fopen("/proc/self/status", "r");
        if (statusFileHandle == nullptr)
        {
            return false;
        }

        char buf[256]; // arbitrary size
        std::size_t numRead = std::fread(buf, sizeof(char), sizeof(buf), statusFileHandle);
        bool debuggerPresent = false;

        if (numRead > 0)
        {
            buf[numRead] = '\0';

            static const char TracerPIDHeader[] = "TracerPid:";
            const char * tracerPIdSection = std::strstr(buf, TracerPIDHeader);

            if (tracerPIdSection != nullptr)
            {
                int tracerPID = std::atoi(tracerPIdSection + sizeof(TracerPIDHeader) - 1);
                debuggerPresent = (tracerPID != 0);
            }
        }

        std::fclose(statusFileHandle);
        return debuggerPresent;
    }


    // Generates an interrupt 3 (commonly raises SIGTRAP caught by the debugger without crashing the program afterwards).
    void    DebugBreak()
    {
        if (IsDebuggerPresent())
        {
            __asm__("int $3");
        }
    }
}

#endif // MOE_SHIPPING
