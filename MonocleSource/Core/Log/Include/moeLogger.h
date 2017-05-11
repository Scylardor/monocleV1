#ifndef MOE_LOGGER_H_
#define MOE_LOGGER_H_

#include "moeDLLVisibility.h"
#include "moeLoggerBase.h"

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

#endif // MOE_LOGGER_H_