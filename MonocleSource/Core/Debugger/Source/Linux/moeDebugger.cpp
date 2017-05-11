#include <cstdio>   // FILE* ops
#include <cstring>  // strstr
#include <cstdlib>  // atoi
#include "moeDebugger.h"

// Linux version of Monocle debugger utilities

namespace moe
{
    // The idea on Linux is to read the status file of current process and see if a tracer PID is present in it (as GDB would write in it).
    // Credits to Sam Liao on http://stackoverflow.com/questions/3596781/how-to-detect-if-the-current-process-is-being-run-by-gdb
    // TODO: needs testing (at least on some major distro like ubuntu...)
    // and using a fopen+read combo is guaranteed to be a performance hit, especially if called often...
    bool    IsDebuggerPresent()
    {
        std::FILE* statusFileHandle = std::fopen("/proc/self/status", "r");
        if (statusFileHandle == nullptr)
        {
            return false;
        }

        char buf[1024]; // arbitrary size
        std::size_t numRead = std::fread(buf, sizeof(char), sizeof(buf), statusFileHandle);

        if (numRead > 0)
        {
            buf[numRead] = '\0';

            static const char TracerPIDHeader[] = "TracerPid:";
            const char * tracerPIdSection = std::strstr(buf, TracerPIDHeader);

            if (tracerPIdSection != nullptr)
            {
                int tracerPID = std::atoi(tracerPIdSection + sizeof(TracerPIDHeader) - 1);
                return (tracerPID != 0);
            }
        }

        return false;
    }


    // Generates an interrupt 3 (commonly raises SIGTRAP caught by the debugger without crashing the program afterwards).
    // TODO: needs testing
    void    DebugBreak()
    {
        if (IsDebuggerPresent())
        {
            __asm__("int $3");
        }
    }
}
