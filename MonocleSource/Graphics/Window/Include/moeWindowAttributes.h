#ifndef MOE_WINDOW_ATTRIBUTES_H_
#define MOE_WINDOW_ATTRIBUTES_H_

#include <cstdint> // std int types
#include "moeContextDescriptor.h"
#include "moeDLLVisibility.h"

// Window attributes is the platform-agnostic structure Monocle uses
// to query graphics context specifications to the underlying
// platform-specific OS windowing API.
// You can specify one in the Init() method of your moe::App specialization,
// or Monocle will pass a default one to its graphics context.
namespace moe
{
    struct MOE_DLL_API WindowAttributes
    {
        std::uint32_t       Width                   = DEFAULT_WIDTH;
        std::uint32_t       Height                  = DEFAULT_HEIGHT;
        const wchar_t*      Title                   = L"Monocle Window Application";
        ContextDescriptor   ContextDesc;  // Bits allocated to RGBA, Depth, Stencil etc.

        // DEFAULT_WIDTH * DEFAULT_HEIGHT = a 16:9 ratio that probably won't take full screen
        // TODO: maybe ask the system the ideal default size instead ?
        static const std::uint32_t  DEFAULT_WIDTH   =   1280;
        static const std::uint32_t  DEFAULT_HEIGHT  =   720;
    };
}

#endif // MOE_WINDOW_ATTRIBUTES_H_