#ifndef MOE_GRAPHICS_CONTEXT_H_
#define MOE_GRAPHICS_CONTEXT_H_

// The header for generic graphics contexts.
// It includes platform-specific ones as available contexts are not
// the same depending on the current OS.
namespace moe
{
    class GraphicsContext
    {
    public:
        virtual void    OnThreadChange() = 0;
    };
}

#endif // MOE_GRAPHICS_CONTEXT_H_