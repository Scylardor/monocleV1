#ifndef MOE_WINDOW_ATTRIBUTES_H_
#define MOE_WINDOW_ATTRIBUTES_H_

#include <cstdint> // std int types
#include "Core/Preprocessor/moeDLLVisibility.h"
#include "Graphics/Context/moeContextDescriptor.h"
#include "Core/Misc/moeNamedType.h"

// Window attributes is the platform-agnostic structure Monocle uses
// to query graphics context specifications to the underlying
// platform-specific OS windowing API.
// You can specify one in the Init() method of your moe::App specialization,
// or Monocle will pass a default one to its graphics context.
namespace moe
{
    struct WidthParam {};
    struct HeightParam {};
    using Width_t = NamedType<std::uint32_t, WidthParam>;
    using Height_t = NamedType<std::uint32_t, HeightParam>;

    struct MOE_DLL_API WindowAttributes
    {
        struct Dimensions
        {
            Width_t   Width;
            Height_t  Height;
        };

        WindowAttributes(const Dimensions& dims, const wchar_t* windowTitle = DEFAULT_WINDOW_TITLE, int iconId = DEFAULT_ID);

        Dimensions          Dims;
        const wchar_t*      Title;
        int                 IconId;

        static const wchar_t*   DEFAULT_WINDOW_TITLE;
        static const int        DEFAULT_ID = -1; // Invalid ID

    };
}

#endif // MOE_WINDOW_ATTRIBUTES_H_