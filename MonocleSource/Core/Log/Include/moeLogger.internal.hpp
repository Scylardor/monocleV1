namespace moe
{
    template <class FilterPolicy, class FormatPolicy, class WritePolicy>
    StdLogger<FilterPolicy, FormatPolicy, WritePolicy>::StdLogger(FilterPolicy&& filter, FormatPolicy&& formatter, WritePolicy&& writer) :
        m_filter(std::move(filter)),
        m_formatter(std::move(formatter)),
        m_writer(std::move(writer))
    {
    }

    template <class FilterPolicy, class FormatPolicy, class WritePolicy>
    void    StdLogger<FilterPolicy, FormatPolicy, WritePolicy>::ConsumeLogInfo(const LogInfo& logInfo)
    {
        if (m_filter.Filter(logInfo))
        {
            std::string buffer;
            m_formatter.Format(buffer, logInfo);
            m_writer.Write(buffer);
        }
    }
}