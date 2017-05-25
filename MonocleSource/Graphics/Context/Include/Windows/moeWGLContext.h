#ifndef MOE_WGL_CONTEXT_H_
#define MOE_WGL_CONTEXT_H_

#include "Graphics/Context/Include/moeGraphicsContext.h"
#include "glad/glad_wgl.h"

// The monocle implementation of a WGL context for OpenGL on Windows.
namespace moe
{
    class WGLContext : public GraphicsContext
    {
    public:
        enum CurrentFlag
        {
            Current     = 1 << 0,
            NotCurrent  = 1 << 1
        };

        WGLContext(HDC deviceContextHandle, CurrentFlag flag = Current);
        WGLContext(HDC deviceContextHandle, const int* wglAttributes, CurrentFlag flag = Current);
        ~WGLContext();

        virtual void    OnThreadChange() override {}

        void    SetCurrentState(HDC deviceContextHandle, CurrentFlag flag);

    private:
        HGLRC   m_glContext;
        bool    m_isCurrent;
    };
}

#endif // MOE_WGL_CONTEXT_H_
