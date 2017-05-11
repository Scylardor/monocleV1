#include "LogPolicies/NoFormatPolicy.h"
#include "moeStringFormat.h"
#include "LogUtils.h"

namespace moe
{
    void NoFormatPolicy::Format(std::string& buffer, const LogInfo& logInfo)
    {
        StringFormat(buffer, "%s", logInfo.message);
    }
}