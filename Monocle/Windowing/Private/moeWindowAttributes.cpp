#include "Windowing/moeWindowAttributes.h"

#include <limits>


namespace moe
{
    WindowAttributes::WindowAttributes(const Dimensions& dims, const wchar_t* windowTitle, int iconId) :
        Dims(dims),
        Title(windowTitle),
        IconId(iconId)
    {
    }

}
