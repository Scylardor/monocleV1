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
        const wchar_t*      Title   = L"Monocle Window Application";
        ContextDescriptor   ContextDesc;  // Bits allocated to RGBA, Depth, Stencil etc.
    };
}

#endif // MOE_WINDOW_ATTRIBUTES_H_