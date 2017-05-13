#include "DefaultLogger.h"

namespace moe
{
    DefaultLogger&  GetDefaultLogger()
    {
        return DefaultLogger::Instance();
    }

    LoggerBase*  GetDefaultLoggerPtr()
    {
        return DefaultLogger::Instance().GetBasePtr();
    }
}