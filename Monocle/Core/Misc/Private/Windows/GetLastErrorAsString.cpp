#include <Windows.h>
#include "Core/Misc/Windows/GetLastErrorAsString.h"

namespace moe
{
    // Returns the last Win32 error, in string format. Returns an empty string if there is no error.
    std::string GetLastErrorAsString()
    {
        // Get the error message, if any.
        DWORD errorMessageID = ::GetLastError();
        if (errorMessageID == 0)
        {
            return std::string(); // No error message has been recorded
        }

        LPSTR messageBuffer = nullptr;
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

        std::string message(messageBuffer, size);
        LocalFree(messageBuffer);

        return message;
    }

}
