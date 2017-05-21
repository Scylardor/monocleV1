#ifndef MOE_LOGGER_BASE_H_
#define MOE_LOGGER_BASE_H_

#ifdef MOE_STD_SUPPORT

#include <string>
#include "IntrusiveListNode.h"
#include "LogUtils.h"
#include "moeDLLVisibility.h"

namespace moe
{
    // The LoggerBase is a logger interface you can derive from to implement your own policy-based implementations.
    // Making it a virtual class so they can be linked by intrusive list nodes (not possible with templates)
    // Using STD support because the Log function uses std::string as a format buffer.
    class LoggerBase : public IntrusiveListNode<LoggerBase>
    {
    public:
        LoggerBase() : IntrusiveListNode<LoggerBase>(this) {}
        virtual ~LoggerBase() {}

        // A virtual function cannot be template so preformat the message and call the virtual function afterwards for further processing
        template <typename... Args>
        void    Log(LogChannel channel, LogSeverity severity, const char* file, int line, const char* format, const Args&... args);

        void    MOE_DLL_API LogFormatted(LogChannel channel, LogSeverity severity, const char* file, int line, const std::string& message);

    protected:
        virtual void	ConsumeLogInfo(const LogInfo& logInfo) = 0;
    };
}

#include "moeLoggerBase.internal.hpp"

#endif // MOE_STD_SUPPORT


#endif // MOE_LOGGER_BASE_H_