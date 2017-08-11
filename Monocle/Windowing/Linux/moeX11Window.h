#ifndef MOE_X11_WINDOW_H_
#define MOE_X11_WINDOW_H_

// Monocle implementation of an X 11 window for Linux.
// ---------------------------------------------------

#include "Windowing/moeWindow.h"
#include "Graphics/Context/Linux/moeGLXContext.h"

namespace moe
{
    // needed in order to specialize WindowTraits with our not-yet-defined X11Window
    // We *must* define WindowTraits BEFORE the actual window class because
    // WindowBase inheritance will need traits to statically enforce window's properties.
    class X11Window;

    template<>
    struct WindowTraits<X11Window>
    {
        using HandleType = Window;
        using CompatibleContexts = moe::TypeList<moe::GLXContext>;
    };


    class X11Window : public WindowBase<X11Window>
    {
    public:
        MOE_DLL_API X11Window(const WindowAttributes& winAttr);
        MOE_DLL_API ~X11Window();

        MOE_DLL_API bool    IsInitialized() const;

    private:
        void    OpenDisplay();

        void    Initialize(const WindowAttributes& winAttr);

        // WindowBase methods
        // WindowBase needs to be friend because it's gonna call CreateConcreteContext
        friend WindowBase<X11Window>;
        template <class Context>
        void                CreateConcreteContext(const ContextDescriptor& contextDesc);


        // Data

        Display*            m_xDisplay;
    };

    // Need to specialize templates out of the class because GCC respects the standard
    template <>
    MOE_DLL_API void    X11Window::CreateConcreteContext<moe::GLXContext>(const ContextDescriptor& contextDesc);
}

#endif // MOE_X11_WINDOW_H_
