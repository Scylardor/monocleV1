#ifndef MOE_GRAPHICS_CONTEXT_H_
#define MOE_GRAPHICS_CONTEXT_H_

#include "Core/Preprocessor/moeDLLVisibility.h"

// The header for generic graphics contexts.
// It includes platform-specific ones as available contexts are not
// the same depending on the current OS.
namespace moe
{
    class MOE_DLL_API GraphicsContext
    {
    public:
        virtual ~GraphicsContext() {}

        virtual void    OnThreadChange() = 0;
    };
}

#endif // MOE_GRAPHICS_CONTEXT_H_