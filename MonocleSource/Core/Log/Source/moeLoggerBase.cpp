#include "moeLoggerBase.h"

namespace moe
{
    void    LoggerBase::LogFormatted(LogChannel channel, LogSeverity severity, const char* file, int line, const std::string& message)
    {
        LogInfo logInfo = { message, file, line, channel, severity };
        ConsumeLogInfo(logInfo);
    }
}
