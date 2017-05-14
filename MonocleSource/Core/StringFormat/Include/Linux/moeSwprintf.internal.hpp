
// LINUX version 
namespace moe
{
    template <typename... Args>
    int SnPrintf(wchar_t * const buffer, std::size_t bufferSize, const wchar_t * const format, const Args& ... args)
    {
        static const int bufferSizeIncrement = 64;
        int result = std::swprintf(buffer, bufferSize, format, StringFormatArg(args)...);
        if (result == -1)
        {
            result = bufferSize + bufferSizeIncrement;
        }
        return result;
    }
}