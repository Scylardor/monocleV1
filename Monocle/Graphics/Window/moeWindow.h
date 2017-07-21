#ifndef MOE_WINDOW_H_
#define MOE_WINDOW_H_

#include "Core/Misc/moeTypeList.h"
#include "Core/Misc/moeFalse.h"
#include "Graphics/Context/moeGraphicsContext.h"
#include "moeWindowAttributes.h"
#include <memory> // unique_ptr

// moe::WindowBase is a CRTP class used to instantiate platform-specific windows.
// It is the core of Monocle windowing system, and is based on the assumption that
// on a given OS, we can only build one type of window (e.g. Win32 on Windows, or X11
// on Linux; you can't e.g. create X11 windows on... Windows.
// So the advantage of this assumption is that windows type can be known at compile-time.
// This may not be so true with e.g. Wayland on Linux, and may need to be adapted in the future.
namespace moe
{
    // To use a custom window with moe::WindowBase, you'll need to provide traits for it.
    // Required traits:
    // - HandleType: the type of the native handle of the window (e.g. HWND for Win32 windows)
    // - CompatibleContexts: a moe::TypeList of contexts that can be used with that window
    template <typename T>
    struct WindowTraits
    {
        using HandleType = void;
        using CompatibleContexts = moe::TypeList<moe::NullType>;
        static_assert(False<T>::value, "Define a WindowTraits specialization for your Window type to be able to inherit WindowBase");
    };



    template <class ConcreteWindow>
    class WindowBase
    {
    public:
        using Handle = typename WindowTraits<ConcreteWindow>::HandleType;

        bool    InitializeWindow(const WindowAttributes& winAttr)
        {
            return window().InitializeWindow(winAttr);
        }

        void    ProcessWindowEvents()
        {
            window().ProcessWindowEvents();
        }

        template <class ContextType>
        void    CreateContext(const WindowAttributes& winAttr)
        {
            static_assert(std::is_base_of<moe::GraphicsContext, ContextType>::value, "CreateContext only accepts classes derived from moe::GraphicsContext");
            static_assert(moe::TypeListIndexOf<WindowTraits<ConcreteWindow>::CompatibleContexts, ContextType>::value != -1,
                "This context type isn't supported by your Window type!");

            m_context = nullptr;
            window().CreateConcreteContext<ContextType>(winAttr);
        }

        void    DestroyWindow();

        Handle  GetHandle()
        {
            return m_handle;
        }

    protected:
        ~WindowBase() {}

        Handle  m_handle;
        std::unique_ptr<moe::GraphicsContext>   m_context;

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