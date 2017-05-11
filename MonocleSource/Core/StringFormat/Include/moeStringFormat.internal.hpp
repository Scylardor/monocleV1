#ifdef MOE_STD_SUPPORT
#include <string>
#endif // MOE_STD_SUPPORT

#include "moeAssert.h"

namespace moe
{
    // Under the hood, StringFormat calls sprintf functions.
    template <typename... Args>
    int SnPrintf(char * const buffer, std::size_t bufferSize, const char * const format, const Args& ... args)
    {
        int const result = snprintf(buffer, bufferSize, format, StringFormatArg(args)...);
        MOE_ASSERT(result != -1);
        return result;
    }

    template <typename... Args>
    int SnPrintf(wchar_t * const buffer, std::size_t bufferSize, const wchar_t * const format, const Args& ... args)
    {
        // Unlike snprintf, you need to pass nullptr as buffer to swprintf to get the formatted output size,
        // otherwise if it doesn't fit it returns -1. So we can try first but else we need to call it again to get the wanted size.
        // cf. http://www.cplusplus.com/reference/cwchar/swprintf/
        int result = swprintf(buffer, bufferSize, format, StringFormatArg(args)...);
        if (result == -1)
        {
            result = swprintf(nullptr, 0, format, StringFormatArg(args)...);
            MOE_ASSERT(result != -1);
        }
        return result;
    }

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
            MOE_ASSERT(SnPrintf(&fmtBuffer[0], formattedSize + 1, format, args...) == formattedSize);
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
        StringFormat(buffer, L"%ls", format);
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
