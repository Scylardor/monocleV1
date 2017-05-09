#include "LogUtils.h"

namespace moe
{
    namespace internals
    {
        static const char*  LogChannelStrings[] =
        {
            "Default",
            // ...
        };

        static const char*  LogSeverityStrings[] =
        {
            "INFO",
            "WARNING",
            "ERROR",
            "FATAL"
        };

        const char* GetChannelString(LogChannel chan)
        {
            // TODO assert chan < countof(LogChannelStrings)
            return LogChannelStrings[(int)chan];
        }

        const char* GetSeverityString(LogSeverity sev)
        {
            // TODO assert sev < countof(LogSeverityStrings)
            return LogSeverityStrings[(int)sev];
        }
    }
}