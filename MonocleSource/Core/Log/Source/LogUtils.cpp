#include "LogUtils.h"
#include "moeCountof.h"

namespace moe
{
    namespace internals
    {
        static const char*  LogChannelStrings[] =
        {
            "Default",
            "Debug",
            // ...
        };
        static_assert(moe::Countof(LogChannelStrings) == moe::LogChannel::_LogChannelMax_, "Each LogChannel value must have a matching string representation");

        static const char*  LogSeverityStrings[] =
        {
            "INFO",
            "WARNING",
            "ERROR",
            "FATAL"
        };
        static_assert(moe::Countof(LogSeverityStrings) == moe::LogSeverity::_LogSeverityMax_, "Each LogSeverity value must have a matching string representation");

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