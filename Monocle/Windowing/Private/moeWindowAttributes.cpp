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

    const wchar_t*                      WindowAttributes::DEFAULT_WINDOW_TITLE = L"Monocle Application";
    const WindowAttributes::Dimensions  WindowAttributes::DEFAULT_DIMENSIONS =
    {
        moe::Width_t(std::numeric_limits<std::uint32_t>::max()),
        moe::Height_t(std::numeric_limits<std::uint32_t>::max())
    };
}
