#ifdef MOE_STD_SUPPORT
#include <string>
#endif // MOE_STD_SUPPORT

namespace moe
{
    // This function passes +1 to buffer sizes because SnPrintf assumes the reported buffer size includes space for the null terminator.
    template <class String, typename FormatChar, typename... Args>
    void StringFormat(String & buffer, const FormatChar * const format, const Args& ... args)
    {
        StringFormatBuffer<String> fmtBuffer(buffer);
        std::size_t origBufferSize = fmtBuffer.Size();
        // Assume the target buffer is large enough and resize only if necessary
        std::size_t const formattedSize = SnPrintf(&fmtBuffer[0], origBufferSize + 1, format, args...);
        if (formattedSize > origBufferSize)
        {
            fmtBuffer.Resize(formattedSize);
            SnPrintf(&fmtBuffer[0], formattedSize + 1, format, args...); // TODO: assert return == formattedSize
        }
        else if (formattedSize < origBufferSize)
        {
            fmtBuffer.Resize(formattedSize);
        }
    }
    
    // Specializations to protect against single format strings
    template <class String>
    void StringFormat(String & buffer, const char * const format)
    {
        StringFormat(buffer, "%s", format);
    }

    template <class String>
    void StringFormat(String & buffer, const wchar_t * const format)
    {
        StringFormat(buffer, "%ls", format);
    }


    // Under the hood, StringFormat calls sprintf functions.
    template <typename... Args>
    int SnPrintf(char * const buffer, std::size_t bufferSize, const char * const format, const Args& ... args)
    {
        int const result = snprintf(buffer, bufferSize, format, StringFormatArg(args)...);
        // TODO assert(-1 != result);
        return result;
    }

    template <typename... Args>
    int SnPrintf(wchar_t * const buffer, std::size_t bufferSize, const wchar_t * const format, const Args& ... args)
    {
        int const result = swprintf(buffer, bufferSize, format, StringFormatArg(args)...);
        // TODO assert(-1 != result);
        return result;
    }


#ifdef MOE_STD_SUPPORT
    // If std is supported, provide convenient specializations for StringFormat to work with std::string "out of the box".
    template <typename CharT>
    const CharT * StringFormatArg(const std::basic_string<CharT> & stringArg)
    {
        return stringArg.c_str();
    }

    template <typename CharT>
    class StringFormatBuffer<std::basic_string<CharT>>
    {
    public:
        StringFormatBuffer(std::basic_string<CharT>& buf) : m_buf(buf) {}

        std::size_t Size() const
        {
            return m_buf.size();
        }

        void    Resize(std::size_t newSize)
        {
            m_buf.resize(newSize);
        }

        CharT&  operator[](std::size_t pos)
        {
            return m_buf[pos];
        }

    private:
        std::basic_string<CharT>& m_buf;
    };
#endif // MOE_STD_SUPPORT

}

