#include "LogPolicies/DebuggerFormatPolicy.h"
#include "moeStringFormat.h"
#include "LogUtils.h"

namespace moe
{
    void    DebuggerFormatPolicy::Format(std::string& buf, const LogInfo& logInfo)
    {
        StringFormat(buf, "%s(%d): [%s] (%s) %s",
            logInfo.file,
            logInfo.line,
            internals::GetChannelString(logInfo.channel),
            internals::GetSeverityString(logInfo.severity),
            logInfo.message);
    }
}