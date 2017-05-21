#include "moeStringFormat.h"

namespace moe
{
    template <typename... Args>
    void    LoggerBase::Log(LogChannel channel, LogSeverity severity, const char* file, int line, const char* format, const Args&... args)
    {
        // Preformat the message once for all chained loggers, instead of letting them doing it multiple times
        std::string message;
        StringFormat(message, format, args...);

        // Start iterating on ourselves and stop when we're back on us (intrusive list is circular)
        IntrusiveListNode<LoggerBase>* loggerNode = this;
        do
        {
            LoggerBase* loggerOwner = loggerNode->Owner();
            if (MOE_ENSURE(loggerOwner != nullptr))
            {
                loggerOwner->LogFormatted(channel, severity, file, line, message);
                loggerNode = loggerNode->Next();
            }
        } while (loggerNode != this);
    }
}
