#include "LogPolicies/NoFormatPolicy.h"
#include "moeStringFormat.h"
#include "LogUtils.h"

namespace moe
{
    const std::string& NoFormatPolicy::Format(std::string&, const LogInfo& logInfo)
    {
        return logInfo.message;
    }
}