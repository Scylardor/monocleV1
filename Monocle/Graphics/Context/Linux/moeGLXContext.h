#ifndef MOE_GLX_CONTEXT_H_
#define MOE_GLX_CONTEXT_H_

// The Monocle implementation of a GLX context.
// --------------------------------------------

#include "Graphics/Context/moeGraphicsContext.h"
#include "Core/Preprocessor/moeDLLVisibility.h"
#include "Graphics/OpenGL/Linux/moeAutoGladGLXLoader.h"

#include "glad/glad_glx.h"


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
        ::GLXContext        m_glContext; // GLAD typedef. Actually a pointer.
        AutoGladGLXLoader   m_autoGLLoader; // The automatic GL loader to keep a dummy handle on libGL.
    };
}


#endif // MOE_GLX_CONTEXT_H_
