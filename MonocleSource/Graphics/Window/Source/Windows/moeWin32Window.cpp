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

        HDC myDC = GetDC(GetHandle());

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
        m_context = std::make_unique<moe::WGLContext>();
        if (!MOE_ENSURE(m_context != nullptr))
        {
            return; // Couldn't create a new context ?! Give up now
        }

        moe::WGLContext* wglContext = static_cast<moe::WGLContext*>(m_context.get());

        wglContext->CreateLegacyContext(myDC, winAttr.ContextDesc);
        if (!wglContext->LoadExtensions(myDC))
        {
            m_context.reset();
            return;
        }

        MOE_INFO(moe::ChanWindowing, "WGL extensions successfully loaded, recreating Win32 window...");
        ReleaseDC(GetHandle(), myDC);
        DestroyWindow();
        if (!InitializeWindow(winAttr))
        {
            return;
        }

        myDC = GetDC(GetHandle()); // The handle has been recreated, re-get the DC
        wglContext->CreateExtensibleContext(myDC, winAttr.ContextDesc);
        ReleaseDC(GetHandle(), myDC);
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