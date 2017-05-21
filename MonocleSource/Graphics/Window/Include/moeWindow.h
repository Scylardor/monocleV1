#ifndef MOE_WINDOW_H_
#define MOE_WINDOW_H_

#include "moeWindowAttributes.h"

// moe::WindowBase is a CRTP class used to instantiate platform-specific windows.
// It is the core of Monocle windowing system, and is based on the assumption that
// on a given OS, we can only build one type of window (e.g. Win32 on Windows, or X11
// on Linux; you can't e.g. create X11 windows on... Windows.
// So the advantage of this assumption is that windows type can be known at compile-time.
// This may not be so true with e.g. Wayland on Linux, and may need to be adapted in the future.
namespace moe
{
    // TODO: move this to a proper place (Core?)
    template<typename T>
    struct FalseType : std::false_type
    { };

    // Used by the WindowBase CRTP to find out of which type, which is defined by the CRTP base class (concrete window),
    // will be the window handle.
    template <typename T>
    struct WindowTraits
    {
        using HandleType = void;
        static_assert(FalseType<T>::value, "Define a WindowTraits with a HandleType to instantiate your window type");
    };



    template <class ConcreteWindow>
    class WindowBase
    {
    public:
        using Handle = typename WindowTraits<ConcreteWindow>::HandleType;

        void        InitializeWindow(const WindowAttributes& winAttr)
        {
            window().InitializeWindow(winAttr);
        }

        void        ProcessWindowEvents()
        {
            window().ProcessWindowEvents();
        }

        void        DestroyWindow();

        Handle      GetHandle()
        {
            return m_handle;
        }

    protected:
        ~WindowBase() {}

        Handle  m_handle;

    private:
        ConcreteWindow& window()
        {
            return static_cast<ConcreteWindow&>(*this);
        }
    };
}

#ifdef MOE_WINDOWS
#include "Windows/moeWin32Window.h"
#else if defined(MOE_LINUX)
#include "Linux/moeX11Window.h"
#endif // MOE_WINDOWS


#endif // MOE_WINDOW_H_