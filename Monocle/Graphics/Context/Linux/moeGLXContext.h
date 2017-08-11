#ifndef MOE_GLX_CONTEXT_H_
#define MOE_GLX_CONTEXT_H_

// The Monocle implementation of a GLX context.
// --------------------------------------------

#include "Graphics/Context/moeGraphicsContext.h"
#include "Core/Preprocessor/moeDLLVisibility.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace moe
{
    class ContextDescriptor;

    class GLXContext : public GraphicsContext
    {
    public:
        MOE_DLL_API GLXContext(Display* display);

        MOE_DLL_API XVisualInfo *   GetVisualFor(Display* display, const ContextDescriptor& ctxDesc);

        virtual void    OnThreadChange() override {}

    private:
        void*    m_glContext; // GLAD uses a typedef with the same name ! Actually a pointer.

    };
}


#endif // MOE_GLX_CONTEXT_H_
