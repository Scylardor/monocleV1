#ifndef MOE_LOGGER_H_
#define MOE_LOGGER_H_

#include "moeDLLVisibility.h"
#include "moeLoggerBase.h"
#include "DefaultLogger.h"

namespace moe
{
    // A logger implementation that uses std::string as string buffer.
    // It is modulable with three orthogonal policies: the filter, the formatter and the writer.
#ifdef MOE_STD_SUPPORT
    template <class FilterPolicy, class FormatPolicy, class WritePolicy>
    class StdLogger : public LoggerBase
    {
    public:
        StdLogger(
            FilterPolicy&& filter       = FilterPolicy(),
            FormatPolicy&& formatter    = FormatPolicy(),
            WritePolicy&& writer        = WritePolicy());

        FilterPolicy&   GetFilterPolicy() { return m_filter; }
        FormatPolicy&   GetFormatPolicy() { return m_formatter; }
        WritePolicy&    GetWritePolicy()  { return m_writer; }

    protected:
        virtual void    ConsumeLogInfo(const LogInfo& logInfo) override;

	private:
		FilterPolicy    m_filter;
		FormatPolicy    m_formatter;
		WritePolicy     m_writer;
    };
}

#include "moeLogger.internal.hpp"
#endif // MOE_STD_SUPPORT


// The macros commonly used for logging.
// Each macro comes in two flavors: with or without a specified Logger. Without a specified Logger, we use the static default logger.
#define MOE_LOG_TO_LOGGER(logger, channel, severity, file, line, format, ...) \
    logger.Log(channel, severity, file, line, format, __VA_ARGS__)

#define MOE_LOG_EXPAND(logger, channel, severity, format, ...) \
    MOE_LOG_TO_LOGGER(logger, channel, severity, __FILE__, __LINE__, format, __VA_ARGS__)


#define MOE_LOG_DEFAULT(channel, severity, format, ...)                                                     \
    do {                                                                                                    \
        moe::LoggerBase* logger = moe::defaultLogger.Get();                                                 \
        if (logger != nullptr)                                                                              \
        {                                                                                                   \
            MOE_LOG_TO_LOGGER((*logger), channel, severity, __FILE__, __LINE__, format, __VA_ARGS__);       \
        }                                                                                                   \
    } while (0)


#define MOE_INFO(channel, format, ...) \
    MOE_LOG_DEFAULT(channel, moe::SevInfo, format, __VA_ARGS__)

#define MOE_LOGGER_INFO(logger, channel, format, ...) \
    MOE_LOG_EXPAND(logger, channel, moe::SevInfo, format, __VA_ARGS__)


#define MOE_WARNING(channel, format, ...) \
    MOE_LOG_DEFAULT(channel, moe::SevWarning, format, __VA_ARGS__)    

#define MOE_LOGGER_WARNING(logger, channel, format, ...) \
    MOE_LOG_EXPAND(logger, channel, moe::SevWarning, format, __VA_ARGS__)


#define MOE_ERROR(channel, format, ...) \
    MOE_LOG_DEFAULT(channel, moe::SevError, format, __VA_ARGS__)    

#define MOE_LOGGER_ERROR(logger, channel, format, ...) \
    MOE_LOG_EXPAND(logger, channel, moe::SevError, format, __VA_ARGS__)


#define MOE_FATAL(channel, format, ...) \
    MOE_LOG_DEFAULT(channel, moe::SevFatal, format, __VA_ARGS__)

#define MOE_LOGGER_FATAL(logger, channel, format, ...) \
    MOE_LOG_EXPAND(logger, channel, moe::SevFatal, format, __VA_ARGS__)

// MOE_LOG = MOE_INFO, basically
#define MOE_LOG(format, ...) MOE_INFO(moe::ChanDefault, format, __VA_ARGS__)


#endif // MOE_LOGGER_H_