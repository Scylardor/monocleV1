#include "Graphics/Window/Include/Windows/moeWin32Window.h"
#include "Core/Preprocessor/Include/moeAssert.h"
#include "Core/Log/Include/moeLogMacros.h"
#include "Core/Misc/Include/moeAbort.h"
#include "Core/Misc/Include/Windows/GetLastErrorAsString.h"
#include "Core/StringFormat/Include/moeStringFormat.h"

#include "glad/glad_wgl.h"

#include <string>

namespace moe
{
    const wchar_t* Win32Window::WINDOW_CLASS = L"Win32Window";

    Win32Window::Win32Window(const WindowAttributes& winAttr) :
        WindowBase<Win32Window>()
    {
        m_handle = nullptr;
        InitializeWindow(winAttr);
    }

    Win32Window::~Win32Window()
    {
        DestroyWindow();
    }

    bool Win32Window::InitializeWindow(const WindowAttributes & winAttr)
    {
        // Window initialization is just registering the window class and creating the window handle.
        if (!RegisterWindow(winAttr))
        {
            return false;
        }

        if (!CreateWindowHandle(winAttr))
        {
            return false;
        }

        return true;
    }

    bool Win32Window::RegisterWindow(const WindowAttributes& winAttr)
    {
        WNDCLASSEXW windowClass;
        ZeroMemory(&windowClass, sizeof(windowClass));

        windowClass.cbSize = sizeof(windowClass);
        // NB: I set CS_OWNDC only for "you never know" legacy reasons.
        // AFAICT it *used* to be mandatory, but not anymore on NT systems.
        // cf. https://www.khronos.org/opengl/wiki/Platform_specifics:_Windows "What should I do before the window is created?"
        windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        windowClass.lpfnWndProc = Win32Window::WndProc;
        windowClass.hInstance = GetModuleHandleW(nullptr);
        windowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
        windowClass.lpszClassName = Win32Window::WINDOW_CLASS;
        windowClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0)); // Paint it black

        // No user-provided icon found, load default icon
        windowClass.hIcon = static_cast<HICON>(LoadImageW(nullptr,
            IDI_APPLICATION, IMAGE_ICON,
            0, 0, LR_DEFAULTSIZE | LR_SHARED));

        // Load user-provided icon if available
        // TODO: test that someday
        //windowClass.hIcon = LoadImageW(GetModuleHandleW(NULL),
        //    L"MONOCLE_ICON", IMAGE_ICON,
        //    0, 0, LR_DEFAULTSIZE | LR_SHARED);
        //if (!windowClass.hIcon)
        //{

        //}

        if (!MOE_ENSURE(RegisterClassExW(&windowClass)))
        {
            MOE_ERROR(moe::ChanWindowing, "Win32 window class registration failed. Last error: '%s'.", moe::GetLastErrorAsString());
            return false;
        }

        return true;
    }

    bool   Win32Window::CreateWindowHandle(const WindowAttributes& winAttr)
    {
        DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
        DWORD extendedStyle = WS_EX_APPWINDOW; // Forces a top-level window onto the taskbar when the window is visible.
                                               // winAttr width/height actually describes wanted client area size, so for the client area to be of the wanted size,
                                               // we must adjust window size to get a window slightly larger than that
        RECT windowRect = { 0, 0, (LONG)winAttr.Width, (LONG)winAttr.Height };
        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE); // this rect now holds the "true" window size

        m_handle = CreateWindowExW(extendedStyle,
            Win32Window::WINDOW_CLASS,
            winAttr.Title,
            style,
            CW_USEDEFAULT, CW_USEDEFAULT, // x,y position
            windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, // window size
            nullptr, // No parent window
            nullptr, // No window menu
            GetModuleHandleW(nullptr), // instance handle
            nullptr); // no custom data needed

        if (!MOE_ENSURE(m_handle != nullptr))
        {
            MOE_ERROR(moe::ChanWindowing, "Win32 window creation failed. Last error: '%s'.", moe::GetLastErrorAsString());
            return false;
        }
        return true;
    }

    void    Win32Window::DestroyWindow()
    {
        m_context.reset();
        ::DestroyWindow(GetHandle());
        UnregisterClass(Win32Window::WINDOW_CLASS, GetModuleHandleW(nullptr));
    }



    template <>
    void    Win32Window::CreateConcreteContext<moe::WGLContext>(const WindowAttributes& winAttr)
    {
        if (!Initialized())
        {
            MOE_ERROR(moe::ChanWindowing, "Win32 window cannot create a context because it isn't initialized!");
            return;
        }

        // The main steps of creating a WGL context:
        // - First create a dummy WGL context to be able to load extensions
        // - Load the extensions
        // - Delete the context and the window, because Windows does not allow the user to change the pixel format of a window.
        //  You get to set it exactly once. Therefore, to use a different pixel format from the dummy context's one (e.g. for sRGB or multisampling),
        //  you must destroy the window entirely and recreate it.
        //  cf. https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context
        // - recreate the window
        // - setup a new WGL context using extensions, etc.
        // enjoy
        MOE_INFO(moe::ChanWindowing, "Creating dummy WGL context");
        CreateDummyWGLContext(winAttr);
        if (m_context == nullptr)
        {
            return; // Couldn't create a dummy context ?! Give up now
        }

        if (!LoadWGLExtensions()) // Failed to load extensions, trash dummy context and leave
        {
            m_context.reset();
            return;
        }
        MOE_INFO(moe::ChanWindowing, "WGL extensions successfully loaded");

        DestroyWindow();

        MOE_INFO(moe::ChanWindowing, "Recreating Win32 window...");
        if (!InitializeWindow(winAttr))
        {
            return;
        }

        CreateExtensibleWGLContext(winAttr);

    }
    void    Win32Window::CreateDummyWGLContext(const WindowAttributes& winAttr)
    {
        // The values set by this PFD won't really be used.
        // But let's try to keep them as close as possible as what we'd really want...
        PIXELFORMATDESCRIPTOR dummyPFD;
        ZeroMemory(&dummyPFD, sizeof(PIXELFORMATDESCRIPTOR));

        DWORD dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
        if (winAttr.BufferCount > 1)
        {
            dwFlags |= PFD_DOUBLEBUFFER;
        }
        dummyPFD.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        dummyPFD.nVersion = 1;
        dummyPFD.dwFlags = dwFlags;
        dummyPFD.iPixelType = PFD_TYPE_RGBA;
        dummyPFD.cColorBits = winAttr.BufferPixelFormat.ColorBits();
        dummyPFD.cDepthBits = winAttr.BufferPixelFormat.depth;
        dummyPFD.cStencilBits = winAttr.BufferPixelFormat.stencil;
        // Other fields aren't used or are deprecated

        HDC deviceContextHandle = GetDC(GetHandle());
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

        // Create our dummy context
        m_context = std::make_unique<WGLContext>(deviceContextHandle, WGLContext::Current);
    }

    void    Win32Window::CreateExtensibleWGLContext(const WindowAttributes& winAttr)
    {
        const int pfdAttributes[] =
        {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,

            WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB,     winAttr.BufferPixelFormat.ColorBits(),
            WGL_RED_BITS_ARB,       winAttr.BufferPixelFormat.r,
            WGL_GREEN_BITS_ARB,     winAttr.BufferPixelFormat.g,
            WGL_BLUE_BITS_ARB,      winAttr.BufferPixelFormat.b,
            WGL_ALPHA_BITS_ARB,     winAttr.BufferPixelFormat.a,
            WGL_DEPTH_BITS_ARB,     winAttr.BufferPixelFormat.depth,
            WGL_STENCIL_BITS_ARB,   winAttr.BufferPixelFormat.stencil,

            WGL_DOUBLE_BUFFER_ARB,  winAttr.BufferCount >= 2,
            WGL_SAMPLE_BUFFERS_ARB, winAttr.SamplesCount > 1 ? 1 : 0, // put a sample count of at least 2 to activate MSAA
            WGL_SAMPLES_ARB,        winAttr.SamplesCount, // Actual number of MSAA samples
            WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE, // TODO: configure if the user wants a SRGB capable framebuffer or not?
            0
        };

        int chosenPixelFormat = ChooseExtensiblePixelFormat(pfdAttributes);

        BOOL pfSet = SetPixelFormat(GetDC(GetHandle()), chosenPixelFormat, nullptr);
        if (!pfSet)
        {
            MOE_ERROR(moe::ChanWindowing, "Win32 window failed choosing an extensible pixel format! Last error: '%s'", GetLastErrorAsString());
            return;
        }

        // In case the user really doesn't care about the API version, asking for a 1.0 context is a convention to ask the most recent one.
        const int glContextAttributes[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, (winAttr.WantedAPI_MajorVersion != WindowAttributes::DEFAULT_API_MAJOR_VERSION ? winAttr.WantedAPI_MajorVersion : 1),
            WGL_CONTEXT_MINOR_VERSION_ARB, (winAttr.WantedAPI_MinorVersion != WindowAttributes::DEFAULT_API_MINOR_VERSION ? winAttr.WantedAPI_MinorVersion : 0),
            // TODO: manage compatibility profiles later...
            //WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };

        m_context = std::make_unique<WGLContext>(GetDC(GetHandle()), glContextAttributes, WGLContext::Current);
    }


#if !defined(MOE_DIAGNOSTIC) && !defined(MOE_DEBUG)
    int Win32Window::ChooseExtensiblePixelFormat(const int* pixelFormatList)
    {
        // Usually we can trust Windows to choose the pixel format closest to what we need
        int pixelFormatIdx;
        UINT numFormats;
        wglChoosePixelFormatARB(GetDC(GetHandle()), pixelFormatList, nullptr, 1, &pixelFormatIdx, &numFormats);
        return pixelFormatIdx;
    }
#else // Debug or diagnostic modes
    int Win32Window::ChooseExtensiblePixelFormat(const int* pixelFormatList)
    {
        // This debug version iterates upon all possible pixel formats before choosing one.
        HDC deviceContext = GetDC(GetHandle());
        int formatCount = DescribePixelFormat(deviceContext, 1, 0, nullptr);
        MOE_INFO(moe::ChanGraphics, "Enumerating properties from %d pixel formats:", formatCount);
        // NB: pixel format numbering starts at 1!
        for (int pfi = 1; pfi <= formatCount; pfi++)
        {
            // Useful information is quite limited with the PIXELFORMATDESCRIPTOR struct.
            PIXELFORMATDESCRIPTOR pfd;
            DescribePixelFormat(deviceContext, pfi, sizeof(pfd), &pfd);

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
        wglChoosePixelFormatARB(GetDC(GetHandle()), pixelFormatList, nullptr, 1, &pixelFormatIdx, &numFormats);
        return pixelFormatIdx;

    }
#endif // !defined(MOE_DIAGNOSTIC) && !defined(MOE_DEBUG)

    bool Win32Window::LoadWGLExtensions()
    {
        bool success = (gladLoadWGL(GetDC(GetHandle())) == 1);
        if (!success) {
            MOE_ERROR(moe::ChanGraphics, "WGL extensions loading failed!! Expect following procedures to fail miserably...");
        }
        return success;
    }

    void Win32Window::InitializeWGLContext(const WindowAttributes& winAttr)
    {
        //SetPixelFormat(hDC, pixelFormats[0], &pfd);

        //const int glContextAttributes[] =
        //{
        //    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        //    WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        //    //WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
        //    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        //    0
        //};

        //hGLRC = wglCreateContextAttribsARB(hDC, NULL, glContextAttributes);

        //wglMakeCurrent(hDC, hGLRC);

        //ShowWindow(hWnd, SW_SHOW);

        //return hWnd;

    }

    bool    Win32Window::Initialized() const
    {
        return m_handle != nullptr;
    }

    void Win32Window::ProcessWindowEvents()
    {
        MSG msg = { 0 };
        while (WM_QUIT != msg.message)
        {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    LRESULT CALLBACK Win32Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)			// Check For Windows Messages
        {
            //      case WM_ACTIVATE:	// Watch For Window Activate Message
            //      {
            //          if (!HIWORD(wParam))// Check Minimization State

            //              active = TRUE;// Program Is Active
            //      }
            //          else
            //          {
            //              active = FALSE;// Program Is No Longer Active
            //          }

            //          return 0;	// Return To The Message Loop
            //      }

                  //case WM_SYSCOMMAND:	// Intercept System Commands
            //      {
            //          switch (wParam)	// Check System Calls
            //          {
            //              // Screensaver Trying To Start?
            //          case SC_SCREENSAVE:
            //              // Monitor Trying To Enter Powersave?
            //          case SC_MONITORPOWER:
            //              return 0;	// Prevent From Happening
            //          }
            //          break;			// Exit
            //      }

        case WM_CREATE: // Window was created
        {
            MOE_INFO(moe::ChanDebug, "Win32 window creation was successful");
            return 0;
        }
        case WM_CLOSE:  // Did We Receive A Close Message?
        {
            MOE_INFO(moe::ChanDebug, "Win32 window close requested, sending QUIT message!");
            PostQuitMessage(0);
            return 0;
        }

        //      case WM_KEYDOWN:		// Is A Key Being Held Down?
        //      {
        //          keys[wParam] = TRUE;// If So, Mark It As TRUE
        //          return 0;		// Jump Back
        //      }

        //      case WM_KEYUP:		// Has A Key Been Released?
        //      {
        //          keys[wParam] = FALSE;// If So, Mark It As FALSE
        //          return 0;			// Jump Back
        //      }

        //      case WM_SIZE:		// Resize The Direct3D Window
        //      {
        //          // LoWord=Width, HiWord=Height
        //          ReSizeD3DScene(LOWORD(lParam), HIWORD(lParam));
        //          return 0;			// Jump Back
        //      }
          //}
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

}