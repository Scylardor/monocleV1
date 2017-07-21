#include "Core/Log/DefaultLogger.h"

namespace moe
{
  DefaultLogger::DefaultLogger() :
    m_defaultLogger(nullptr)
  {
  }

    DefaultLogger&  GetDefaultLogger()
    {
        return DefaultLogger::Instance();
    }

    LoggerBase*  GetDefaultLoggerPtr()
    {
        return DefaultLogger::Instance().GetBasePtr();
    }

    void    ResetDefaultLogger()
    {
         DefaultLogger::Instance().Reset();
    }
}
