#ifndef MOE_WIN32_WINDOW_H_
#define MOE_WIN32_WINDOW_H_

#include "moeDLLVisibility.h"
#include "moeWindow.h"
#include "Graphics/Context/Include/Windows/moeWGLContext.h"
#include <windows.h> // HWND

namespace moe
{
    // needed in order to specialize WindowTraits with our not-yet-defined Win32Window
    // We *must* define WindowTraits BEFORE the actual window class because
    // WindowBase inheritance will need traits to statically enforce window's properties.
    class Win32Window;

    template<>
    struct WindowTraits<Win32Window>
    {
        using HandleType = HWND;
        using CompatibleContexts = moe::TypeList<moe::WGLContext>;
    };

    class Win32Window : public WindowBase<Win32Window>
    {
    public:
        MOE_DLL_API Win32Window(const WindowAttributes& winAttr);
        MOE_DLL_API ~Win32Window();


        void    MOE_DLL_API ProcessWindowEvents();

        bool    Initialized() const;

    private:
        // WindowBase needs to be friend because it's gonna call CreateConcreteContext
        friend WindowBase<Win32Window>;

        template <class Context>
        void                CreateConcreteContext(const WindowAttributes& winAttr);
        template <>
        MOE_DLL_API void    CreateConcreteContext<moe::WGLContext>(const WindowAttributes& winAttr);

        bool    InitializeWindow(const WindowAttributes& winAttr);
        bool    RegisterWindow(const WindowAttributes& winAttr);
        bool    CreateWindowHandle(const WindowAttributes& winAttr);
        void    DestroyWindow();

        static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        static const wchar_t*  WINDOW_CLASS;
    };

    // Define the Win32 window as being the default moe::Window.
    typedef Win32Window Window;
}

#endif // MOE_WIN32_WINDOW_H_
