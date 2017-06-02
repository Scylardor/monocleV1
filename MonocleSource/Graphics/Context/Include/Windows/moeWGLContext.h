#ifndef MOE_WGL_CONTEXT_H_
#define MOE_WGL_CONTEXT_H_

#include "Graphics/Context/Include/moeGraphicsContext.h"
#include "glad/glad_wgl.h" // HDC is provided through GLAD
#include <cinttypes>


// The monocle implementation of a WGL context for OpenGL on Windows.
namespace moe
{
    struct ContextDescriptor;

    class WGLContext : public GraphicsContext
    {
    public:
        enum class CurrentFlag
        {
            Current     = 1 << 0,
            NotCurrent  = 1 << 1
        };

        WGLContext();
        ~WGLContext();

        virtual void    OnThreadChange() override {}

        void    CreateLegacyContext(HDC deviceContextHandle, const ContextDescriptor& pfDesc, CurrentFlag flag = CurrentFlag::Current);
        void    CreateExtensibleContext(HDC deviceContextHandle, const ContextDescriptor& pfDesc, CurrentFlag flag = CurrentFlag::Current);

        void    LoadExtensions();

        void    SetCurrentState(HDC deviceContextHandle, CurrentFlag flag);

        static bool LoadExtensions(HDC deviceContextHandle);

    private:
        int     ChooseExtensiblePixelFormat(HDC deviceContextHandle, const int* pixelFormatList);

        HGLRC   m_glContext;
    };
}

#endif // MOE_WGL_CONTEXT_H_
