#include "Windowing/moeWindowAttributes.h"

namespace moe
{
    WindowAttributes::WindowAttributes(const Dimensions& dims, const wchar_t* windowTitle, int iconId) :
        Dims(dims),
        Title(windowTitle),
        IconId(iconId)
    {
    }

    const wchar_t*     WindowAttributes::DEFAULT_WINDOW_TITLE = L"Monocle Application";
}
