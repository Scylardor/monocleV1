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


        void    MOE_DLL_API ProcessWindowEvents();

    private:
        // WindowBase needs to be friend because it's gonna call CreateConcreteContext
        friend WindowBase<Win32Window>;

        template <class Context>
        void    CreateConcreteContext(const moe::PixelFormat&);

        template <>
        void    CreateConcreteContext<moe::WGLContext>(const moe::PixelFormat& pf)
        {
            InitializeWGLContext(pf);
        }

        void    InitializeWindow(const WindowAttributes& winAttr);

        void    MOE_DLL_API InitializeWGLContext(const moe::PixelFormat& pf);

        static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };

    typedef Win32Window Window;
}

#endif // MOE_WIN32_WINDOW_H_
