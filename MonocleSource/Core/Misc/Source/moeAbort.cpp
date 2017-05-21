#include "moeAbort.h"

#include <cstdlib>

namespace moe
{
    void Abort()
    {
        std::abort();
    }
}