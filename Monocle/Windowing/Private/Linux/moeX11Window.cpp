#include "Windowing/Linux/moeX11Window.h"

#include "Core/Preprocessor/moeAssert.h"
#include "Core/Log/moeLogMacros.h"
#include <cstdlib>  // getenv

#include "glad/glad_glx.h"


namespace moe
{

    X11Window::X11Window(const WindowAttributes& winAttr) :
        WindowBase<X11Window>(winAttr),
        m_xDisplay(nullptr)
    {
        OpenDisplay();
        MOE_DEBUG_ASSERT(m_xDisplay != nullptr);
    }

    X11Window::~X11Window()
    {
       // XCloseDisplay(m_xDisplay);
    }


    bool    X11Window::IsInitialized() const
    {
        return (m_xDisplay != nullptr);
    }


    void    X11Window::OpenDisplay()
    {
        // It is common usage in UNIX that one can use DISPLAY env variable to specify the X display to use.
        // If there isn't (null), will default to the first screen.
        m_xDisplay = XOpenDisplay(std::getenv("DISPLAY"));
    }


    void    X11Window::Initialize(const WindowAttributes& winAttr)
    {


    }




    template <>
    void    X11Window::CreateConcreteContext<moe::GLXContext>(const ContextDescriptor& contextDesc)
    {
        // First create the context
        MOE_DEBUG_ASSERT(m_xDisplay != nullptr);
        m_context = std::make_unique<moe::GLXContext>(m_xDisplay);
        MOE_DEBUG_ASSERT(m_context != nullptr);
        moe::GLXContext* const glxCtx = static_cast<moe::GLXContext*>(m_context.get());
    }
}
