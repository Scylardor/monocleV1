#include "Graphics/Context/Include/Windows/moeWGLContext.h"
#include "Core/Preprocessor/Include/moeAssert.h"
#include "Core/Log/Include/moeLogMacros.h"
#include "Core/Misc/Include/Windows/GetLastErrorAsString.h"
#include "Graphics/Window/Include/moeContextDescriptor.h"

namespace moe
{

    WGLContext::WGLContext() :
        m_glContext(nullptr)
    {

    }

    WGLContext::~WGLContext()
    {
        // It is an error to delete an OpenGL rendering context that is the current context of another thread.
        // However, if a rendering context is the calling thread's current context, the wglDeleteContext function
        // changes the rendering context to being not current before deleting it.
        // cf. https://msdn.microsoft.com/en-us/library/windows/desktop/dd374381(v=vs.85).aspx
        wglDeleteContext(m_glContext);
    }

    // Usually this function's only purpose is to create a dummy context used to initialize WGL extensions
    // The created context will be then destroyed just after in order to create an extensible one.
    void WGLContext::CreateLegacyContext(HDC deviceContextHandle, const ContextDescriptor& pfDesc, CurrentFlag currentFlag)
    {
        PIXELFORMATDESCRIPTOR dummyPFD;
        ZeroMemory(&dummyPFD, sizeof(PIXELFORMATDESCRIPTOR));

        DWORD dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
        if (pfDesc.SamplesCount > 1)
        {
            dwFlags |= PFD_DOUBLEBUFFER;
        }
        dummyPFD.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        dummyPFD.nVersion = 1;
        dummyPFD.dwFlags = dwFlags;
        dummyPFD.iPixelType = PFD_TYPE_RGBA;
        dummyPFD.cColorBits = pfDesc.ColorBits();
        dummyPFD.cDepthBits = pfDesc.DepthBits;
        dummyPFD.cStencilBits = pfDesc.StencilBits;
        // Other fields aren't used or are deprecated

        int pixFormatIdx = ChoosePixelFormat(deviceContextHandle, &dummyPFD);
        if (!MOE_ENSURE(pixFormatIdx > 0))
        {
            MOE_ERROR(moe::ChanWindowing, "Win32 window failed choosing a dummy pixel format! Last error: '%s'", GetLastErrorAsString());
            return;
        }

        BOOL pfIsSet = SetPixelFormat(deviceContextHandle, pixFormatIdx, &dummyPFD);

        if (!MOE_ENSURE(pfIsSet))
        {
            MOE_ERROR(moe::ChanWindowing, "Unable to set window dummy pixel format. Last error: '%s'", GetLastErrorAsString());
            return;
        }

        m_glContext = wglCreateContext(deviceContextHandle);
        if (!MOE_ENSURE(m_glContext != nullptr))
        {
            MOE_ERROR(moe::ChanWindowing, "wglCreateContext failed! Last error: '%s'", GetLastErrorAsString());
            return;
        }

        SetCurrentState(deviceContextHandle, currentFlag);
    }

    void WGLContext::CreateExtensibleContext(HDC deviceContextHandle, const ContextDescriptor& pfDesc, CurrentFlag currentFlag)
    {
        const int pfdAttributes[] =
        {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,

            WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB,     pfDesc.ColorBits(),
            WGL_RED_BITS_ARB,       pfDesc.R,
            WGL_GREEN_BITS_ARB,     pfDesc.G,
            WGL_BLUE_BITS_ARB,      pfDesc.B,
            WGL_ALPHA_BITS_ARB,     pfDesc.A,
            WGL_DEPTH_BITS_ARB,     pfDesc.DepthBits,
            WGL_STENCIL_BITS_ARB,   pfDesc.StencilBits,

            WGL_DOUBLE_BUFFER_ARB,  (pfDesc.BuffersCount > 0),
            WGL_SAMPLE_BUFFERS_ARB, pfDesc.SamplesCount > 1 ? 1 : 0, // put a sample count of at least 2 to activate MSAA
            WGL_SAMPLES_ARB,        pfDesc.SamplesCount, // Actual number of MSAA samples
            WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE, // TODO: configure if the user wants a SRGB capable framebuffer or not?
            0
        };

        int chosenPixelFormat = ChooseExtensiblePixelFormat(deviceContextHandle, pfdAttributes);

        BOOL pfSet = SetPixelFormat(deviceContextHandle, chosenPixelFormat, nullptr);
        if (!pfSet)
        {
            MOE_ERROR(moe::ChanWindowing, "Win32 window failed choosing an extensible pixel format! Last error: '%s'", GetLastErrorAsString());
            return;
        }

        // Manage the "don't care" values of major/minorVer: ask for a 1.0 context, ie. a special value for "any version available".
        const int wglContextAttributes[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, (pfDesc.API_MajorVersion == ContextDescriptor::DEFAULT_API_MAJOR_VERSION ? 1 : pfDesc.API_MajorVersion),
            WGL_CONTEXT_MINOR_VERSION_ARB, (pfDesc.API_MinorVersion == ContextDescriptor::DEFAULT_API_MINOR_VERSION ? 0 : pfDesc.API_MinorVersion),
            // TODO: manage compatibility profiles later...
            //WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };

        m_glContext = wglCreateContextAttribsARB(deviceContextHandle, nullptr, wglContextAttributes);
        if (!MOE_ENSURE(m_glContext != nullptr))
        {
            MOE_ERROR(moe::ChanGraphics, "Unable to create extensible WGL context! Last error: '%s'", moe::GetLastErrorAsString());
            return;
        }

        SetCurrentState(deviceContextHandle, currentFlag);
    }

#if !defined(MOE_DIAGNOSTIC) && !defined(MOE_DEBUG)
    int Win32Window::ChooseExtensiblePixelFormat(HDC deviceContextHandle, const int* pixelFormatList)
    {
        // Usually we can trust Windows to choose the pixel format closest to what we need
        int pixelFormatIdx;
        UINT numFormats;
        wglChoosePixelFormatARB(deviceContextHandle, pixelFormatList, nullptr, 1, &pixelFormatIdx, &numFormats);
        return pixelFormatIdx;
    }
#else // Debug or diagnostic modes
    int WGLContext::ChooseExtensiblePixelFormat(HDC deviceContextHandle, const int* pixelFormatList)
    {
        // This debug version iterates upon all possible pixel formats before choosing one.
        int formatCount = DescribePixelFormat(deviceContextHandle, 1, 0, nullptr);
        MOE_INFO(moe::ChanGraphics, "Enumerating properties from %d pixel formats:", formatCount);
        // NB: pixel format numbering starts at 1!
        for (int pfi = 1; pfi <= formatCount; pfi++)
        {
            // Useful information is quite limited with the PIXELFORMATDESCRIPTOR struct.
            PIXELFORMATDESCRIPTOR pfd;
            DescribePixelFormat(deviceContextHandle, pfi, sizeof(pfd), &pfd);

            bool isRGBA = (pfd.iPixelType & PFD_TYPE_RGBA) == PFD_TYPE_RGBA;
            bool doubleBuffered = ((pfd.dwFlags & PFD_DOUBLEBUFFER) == PFD_DOUBLEBUFFER);
            MOE_INFO(moe::ChanGraphics,
                "Pixel format %i: { Pixel Type %s; components bits: R%dG%dB%dA%d, Depth %d, Stencil %d, double buffered: %s }",
                pfi,
                (isRGBA ? "RGBA " : "COLOR INDEX"),
                pfd.cRedBits, pfd.cGreenBits, pfd.cBlueBits, pfd.cAlphaBits, pfd.cDepthBits, pfd.cStencilBits,
                (doubleBuffered ? "Yes" : "No")
            );
        }

        int pixelFormatIdx;
        UINT numFormats;
        wglChoosePixelFormatARB(deviceContextHandle, pixelFormatList, nullptr, 1, &pixelFormatIdx, &numFormats);
        return pixelFormatIdx;
    }
#endif // !defined(MOE_DIAGNOSTIC) && !defined(MOE_DEBUG)

    void WGLContext::SetCurrentState(HDC deviceContextHandle, CurrentFlag flag)
    {
        if (flag == CurrentFlag::Current)
        {
            MOE_ASSERT(m_glContext != nullptr);
            wglMakeCurrent(deviceContextHandle, m_glContext);
        }
        else
        {
            wglMakeCurrent(nullptr, nullptr);
        }

    }

    bool WGLContext::LoadExtensions(HDC deviceContextHandle)
    {
        return MOE_ENSURE(gladLoadWGL(deviceContextHandle) != 0);
    }
}