#ifndef MOE_WGL_CONTEXT_H_
#define MOE_WGL_CONTEXT_H_

#include "Graphics/Context/Include/moeGraphicsContext.h"

// GLAD includes some windows headers.
// Define this to avoid windows defining min/max() macros disrupting std::numeric_limits functions.
#define NOMINMAX

#include "glad/glad_wgl.h" // HDC is provided through GLAD


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

        void    SetCurrentState(HDC deviceContextHandle, CurrentFlag flag);


        static bool LoadExtensions(HDC deviceContextHandle);

    private:
        int     ChooseExtensiblePixelFormat(HDC deviceContextHandle, const int* pixelFormatList);

        HGLRC   m_glContext;
    };
}

#endif // MOE_WGL_CONTEXT_H_
