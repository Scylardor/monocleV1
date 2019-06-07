#include "Windowing/Windows/moeWin32Window.h"
#include "Windowing/moeWindowAttributes.h"
#include "Core/Preprocessor/moeAssert.h"
#include "Core/Log/moeLogMacros.h"
#include "Core/Misc/Windows/GetLastErrorAsString.h"

namespace moe
{
    const wchar_t* Win32Window::WINDOW_CLASS = L"Win32Window";

    Win32Window::Win32Window(const WindowAttributes& winAttr) :
        WindowBase<Win32Window>(winAttr)
    {
        m_handle = nullptr;
        MOE_DEBUG_ASSERT(InitializeWindow(winAttr) == true);
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
        windowClass.hIcon = LoadIcon(windowClass.hInstance, MAKEINTRESOURCE(winAttr.IconId)); // Loads user-provided icon if available

        if (!MOE_ASSERT(RegisterClassExW(&windowClass)))
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
        RECT windowRect = { 0, 0, (LONG)winAttr.Dims.Width, (LONG)winAttr.Dims.Height };
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

        if (!MOE_ASSERT(m_handle != nullptr))
        {
            MOE_ERROR(moe::ChanWindowing, "Win32 window creation failed. Last error: '%s'.", moe::GetLastErrorAsString());
            return false;
        }

		// Register a pointer to ourself in the user data so the message processing procedure knows which window to send messages to
		SetWindowLongPtr(m_handle, GWLP_USERDATA, (LONG_PTR)this);

        return true;
    }


	LRESULT Win32Window::ProcessWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		wchar_t buf[1024];

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

		case WM_SYSKEYDOWN:		// Is A Key Being Held Down?
		{
			swprintf_s(buf, L"WM_SYSKEYDOWN: 0x%x\n", wParam);
			OutputDebugString(buf);
			break;
		}

		case WM_SYSKEYUP:
		{
			swprintf_s(buf, L"WM_SYSKEYUP: 0x%x\n", wParam);
			OutputDebugString(buf);
			break;
		}

		case WM_KEYDOWN:		// Is A Key Being Held Down?
		{
			swprintf_s(buf, L"WM_KEYDOWN: 0x%x\n", wParam);
			OutputDebugString(buf);
			CHAR name[1024];
			UINT scanCode = MapVirtualKeyA(wParam, MAPVK_VK_TO_VSC);
			LONG lParamValue = (scanCode << 16);
			int result = GetKeyNameTextA(lParamValue, name, 1024);
			if (result > 0)
			{
				OutputDebugStringA(name);
				OutputDebugStringA("\n");
			}
			if (lParam & 0x01000000)
			{
				OutputDebugStringA("extended: yes");
			}
			else
			{
				OutputDebugStringA("extended: no");
			}
			OutputDebugStringA("\n");

			//keys[wParam] = TRUE;// If So, Mark It As TRUE
			return 0;		// Jump Back
		}

		case WM_CHAR:		// Is A Key Being Held Down?
		{
			swprintf_s(buf, L"WM_CHAR: 0x%x %c byte compo: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", wParam, wParam, ((char*)(&wParam))[0], ((char*)(&wParam))[1], ((char*)(&wParam))[2], ((char*)(&wParam))[3], ((char*)(&wParam))[4], ((char*)(&wParam))[5], ((char*)(&wParam))[6], ((char*)(&wParam))[7]);
			OutputDebugString(buf);
			SHORT keyScan = VkKeyScan(wParam);
			CHAR vkCode = (keyScan);
			ZeroMemory(buf, 32);
			swprintf_s(buf, L"VK: 0x%x\n", vkCode);
			OutputDebugString(buf);

			//keys[wParam] = TRUE;// If So, Mark It As TRUE
			return 0;		// Jump Back
		}

		case WM_KEYUP:		// Has A Key Been Released?
		{
			swprintf_s(buf, L"WM_KEYUP: 0x%x\n", wParam);
			OutputDebugString(buf);
		   // keys[wParam] = FALSE;// If So, Mark It As FALSE
		    return 0;			// Jump Back
		}

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


	void    Win32Window::DestroyWindow()
    {
        m_context.reset();
        DestroyWindowHandle();
    }

    void    Win32Window::DestroyWindowHandle()
    {
        ::DestroyWindow(GetHandle());
        UnregisterClass(Win32Window::WINDOW_CLASS, GetModuleHandleW(nullptr));
    }

    template <>
    void    Win32Window::CreateConcreteContext<moe::WGLContext>(const ContextDescriptor& contextDesc)
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
        if (!MOE_ASSERT(m_context != nullptr))
        {
            return; // Couldn't create a new context ?! Give up now
        }

        moe::WGLContext* wglContext = static_cast<moe::WGLContext*>(m_context.get());

        wglContext->CreateLegacyContext(myDC, contextDesc);
        if (!wglContext->LoadExtensions(myDC))
        {
            m_context.reset();
            return;
        }

        MOE_INFO(moe::ChanWindowing, "WGL extensions successfully loaded, recreating Win32 window...");
        ReleaseDC(GetHandle(), myDC);
        DestroyWindowHandle();
        if (!InitializeWindow(m_attribs))
        {
            return;
        }

        myDC = GetDC(GetHandle()); // The handle has been recreated, re-get the DC
        wglContext->CreateExtensibleContext(myDC, contextDesc);
        ReleaseDC(GetHandle(), myDC);
    }

    template <>
    void    Win32Window::CreateConcreteContext<moe::D3DContext_11_2>(const ContextDescriptor& contextDesc)
    {
        if (!Initialized())
        {
            MOE_ERROR(moe::ChanWindowing, "Win32 window cannot create a context because it isn't initialized!");
            return;
        }

        MOE_INFO(moe::ChanWindowing, "Creating D3D 11.2 context");
        m_context = std::make_unique<moe::D3DContext_11_2>(contextDesc, GetHandle());
    }

    template <>
    void    Win32Window::CreateConcreteContext<moe::D3DContext_11_1>(const ContextDescriptor& contextDesc)
    {
        if (!Initialized())
        {
            MOE_ERROR(moe::ChanWindowing, "Win32 window cannot create a context because it isn't initialized!");
            return;
        }

        MOE_INFO(moe::ChanWindowing, "Creating D3D 11.1 context");
        m_context = std::make_unique<moe::D3DContext_11_1>(contextDesc, GetHandle());
    }

    bool    Win32Window::Initialized() const
    {
        return m_handle != nullptr;
    }


    bool	Win32Window::ProcessWindowEvents()
    {
        MSG msg = { 0 };

		bool shouldStop = false;

		// Check to see if any messages are waiting in the queue
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Translate the message and dispatch it to the window message handling procedure
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If the message is WM_QUIT, exit the update loop.
		// PeekMessage will only ever return WM_QUIT once the message queue is empty, so if there's a WM_QUIT message, it will be the last one.
		// see https://docs.microsoft.com/fr-fr/windows/desktop/winmsg/about-messages-and-message-queues
		shouldStop = (msg.message == WM_QUIT);

		return (!shouldStop);
    }


    LRESULT CALLBACK Win32Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
		// Retrieve the window this message belongs too and send it the message
		LONG_PTR lpUserData = GetWindowLongPtr(hWnd, GWLP_USERDATA);
		Win32Window* myWindow = (Win32Window*)lpUserData;
		// Window procedure can be called before a window is bound to it
		if (myWindow == nullptr)
		{
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		else
		{
			return myWindow->ProcessWindowMessage(hWnd, msg, wParam, lParam);
		}
    }

}