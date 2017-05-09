#ifndef MOE_LOG_UTILS_H_
#define MOE_LOG_UTILS_H_

#ifdef MOE_STD_SUPPORT
#include <string>
#endif

// The Log Utilities are the small enums and data-holding structs that Log functions casually use.
// They're grouped in the same file since I can't think of how we could use one without the others.
namespace moe
{
    enum LogChannel
    {
        ChanDefault = 0,
        // ...
    };

    enum LogSeverity
    {
        SevInfo = 0,
        SevWarning,
        SevError,
        SevFatal
    };

#ifdef MOE_STD_SUPPORT
    struct LogInfo
    {
        const std::string&  message;
        const char*         file;
        int                 line;
        LogChannel          channel;
        LogSeverity         severity;
    };
#endif // MOE_STD_SUPPORT

    // In lack of a better alternative for enum-to-string conversion
    namespace internals
    {
        const char* GetChannelString(LogChannel chan);
        const char* GetSeverityString(LogSeverity sev);
    }
}

#endif // MOE_LOG_UTILS_H_