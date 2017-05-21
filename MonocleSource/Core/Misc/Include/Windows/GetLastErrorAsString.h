#ifndef MOE_GET_LAST_ERROR_AS_STRING_H_
#define MOE_GET_LAST_ERROR_AS_STRING_H_

#include <string>

namespace moe
{
    //Returns the last Win32 error, in string format. Returns an empty string if there is no error.
    std::string GetLastErrorAsString();
}

#endif // MOE_GET_LAST_ERROR_AS_STRING_H_