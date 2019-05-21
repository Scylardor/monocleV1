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


	WindowAttributes::WindowAttributes(Width_t w, Height_t h, const wchar_t* windowTitle, int iconId /*= DEFAULT_ID*/) :
		WindowAttributes(Dimensions(w, h), windowTitle, iconId)
	{
	}

}
