#include "Graphics/Window/Include/Windows/moeWin32Window.h"
#include "Core/Preprocessor/Include/moeAssert.h"
#include "Core/Log/Include/moeLogMacros.h"
#include "Core/Misc/Include/moeAbort.h"
#include "Core/Misc/Include/Windows/GetLastErrorAsString.h"
#include "glad/glad_wgl.h"

#include <string>

namespace moe
{
    Win32Window::Win32Window(const WindowAttributes& winAttr) :
        WindowBase<Win32Window>()
    {
        m_handle = nullptr;
        InitializeWindow(winAttr);
    }

    void Win32Window::InitializeWindow(const WindowAttributes & winAttr)
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
        windowClass.lpszClassName = L"Win32Window";
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
            const std::string error = moe::GetLastErrorAsString();
            MOE_FATAL(moe::ChanWindowing, "Failed Win32 window registration with error: '%s'. Aborting...", error);
            moe::Abort();
        }

        DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
        DWORD extendedStyle = WS_EX_APPWINDOW; // Forces a top - level window onto the taskbar when the window is visible.
        // winAttr width/height actually describes wanted client area size, so for the client area to be of the wanted size,
        // we must adjust window size to get a window slightly larger than that
        RECT windowRect = { 0, 0, (LONG)winAttr.Width, (LONG)winAttr.Height };
        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE); // this rect now holds the "true" window size

        m_handle = CreateWindowExW(extendedStyle,
            windowClass.lpszClassName,
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
            const std::string error = moe::GetLastErrorAsString();
            MOE_FATAL(moe::ChanWindowing, "Window creation failed with error: '%s'. Aborting...", error);
            moe::Abort();
        }
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


    void Win32Window::InitializeWGLContext(const moe::PixelFormat & pf)
    {
        if (!gladLoadWGL(GetDC(GetHandle()))) {
            printf("Something went wrong!\n");
            exit(-1);
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