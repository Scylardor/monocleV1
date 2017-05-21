#ifndef MOE_WIN32_WINDOW_H_
#define MOE_WIN32_WINDOW_H_

#include <windows.h> // HWND
#include "moeWindow.h"

namespace moe
{
    class Win32Window;

    template<>
    struct WindowTraits<Win32Window>
    {
        using HandleType = HWND;
        using Type = Win32Window;
    };

    class Win32Window : public WindowBase<Win32Window>
    {
    public:
        Win32Window();

        void    InitializeWindow(const WindowAttributes& winAttr);
        void    ProcessWindowEvents();

        static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };

    typedef Win32Window Window;
}

#endif // MOE_WIN32_WINDOW_H_