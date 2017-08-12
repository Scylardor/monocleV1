#include "Graphics/Context/Linux/moeGLXContext.h"

#include "Graphics/Context/moeContextDescriptor.h"
#include "Graphics/OpenGL/moeOpenGLUtils.h"

#include "Core/Preprocessor/moeAssert.h"
#include "Core/Log/moeLogMacros.h"

#include "glad/glad_glx.h"

namespace moe
{
    GLXContext::GLXContext(Display* display) :
        m_glContext(nullptr)
    {
        if (!gladLoadGLX(display, DefaultScreen(display)) || !m_autoGLLoader.IsLoaded())
        {
            MOE_ERROR(moe::ChanGraphics, "Glad failed to load GLX! Cannot initialize GLX context.");
            return;
        }

        // We need at least GLX 1.3 for FB configs
        int major, minor;
        if (!MOE_ASSERT(glXQueryVersion(display, &major, &minor)) || ((major == 1) && (minor < 3)) || (major < 1))
        {
            MOE_ERROR(moe::ChanGraphics, "Monocle GLX context requests at least GLX 1.3! Cannot initialize GLX context.");
            return;
        }

        MOE_INFO(moe::ChanDebug, "GLX context creation successful.");
    }



    XVisualInfo *   GLXContext::GetVisualFor(Display* display, const ContextDescriptor& ctxDesc)
    {
        // Use 8-bit per color RGBA by default.
        GLenum colorFormat = (ctxDesc.ColorFormat == ContextDescriptor::DEFAULT_GENERIC_FORMAT ? GL_RGBA8 : ctxDesc.ColorFormat);
        ColorBitCounter colorBitCount = DecomposeColorEnumRGBABits(colorFormat);
        bool isSRGB_Color = colorFormat == GL_SRGB8 || colorFormat == GL_SRGB8_ALPHA8; // ATM we only manage these two SRGB formats, favor explicit formats.

        // 24-bit depth and 8-bit stencil seems a sensible default value.
        GLenum depthStencilFormat = (ctxDesc.ColorFormat == ContextDescriptor::DEFAULT_GENERIC_FORMAT ? GL_DEPTH24_STENCIL8 : ctxDesc.DepthStencilFormat);
        DepthStencilBitCounter depthStencilBitCount = DecomposeDepthStencilEnumBits(depthStencilFormat);

        // First describe what we want
        const bool doubleBufferWanted = (ctxDesc.BuffersCount > 0);
        bool msaaWanted = (ctxDesc.SamplesCount > 1);

        MOE_INFO(moe::ChanDebug, "Searching visuals with R%d G%d B%d A%d color, D%d S%d depth/stencil, double buffered: %s, MSAA: %s, %d samples, SRGB: %s",
            colorBitCount.RedBits(), colorBitCount.GreenBits(), colorBitCount.BlueBits(), colorBitCount.AlphaBits(),
            depthStencilBitCount.DepthBits(), depthStencilBitCount.StencilBits(),
            (doubleBufferWanted ? "yes" : "no"),
            (msaaWanted ? "yes" : "no"),
            ctxDesc.SamplesCount,
            (isSRGB_Color ? "yes" : "no")
        );

        int visualAttrs[] =
        {
            GLX_X_RENDERABLE    ,   true,
            GLX_DRAWABLE_TYPE   ,   GLX_WINDOW_BIT,
            GLX_RENDER_TYPE     ,   GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE   ,   GLX_TRUE_COLOR,
            GLX_RED_SIZE        ,   colorBitCount.RedBits(),
            GLX_GREEN_SIZE      ,   colorBitCount.GreenBits(),
            GLX_BLUE_SIZE       ,   colorBitCount.BlueBits(),
            GLX_ALPHA_SIZE      ,   colorBitCount.AlphaBits(),
            GLX_DEPTH_SIZE      ,   depthStencilBitCount.DepthBits(),
            GLX_STENCIL_SIZE    ,   depthStencilBitCount.StencilBits(),
            GLX_DOUBLEBUFFER    ,   doubleBufferWanted,
            GLX_SAMPLE_BUFFERS  ,   msaaWanted,
            GLX_SAMPLES         ,   ctxDesc.SamplesCount,
            GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB, isSRGB_Color,
            None
        };

        // Then iterate on available FB configs to find a compatible visual
        // Opt for a "soft" policy on MSAA: if no MSAA FBConfig is found with desired samples, search general MSAA
        // and if nothing if found, search with no MSAA.
        XVisualInfo* myVisual = nullptr;

        int nbConfs = 0;
        GLXFBConfig* fbConfs = glXChooseFBConfig(display, DefaultScreen(display), visualAttrs, &nbConfs);
        MOE_INFO(moe::ChanDebug, "Searching visuals in %d confs", nbConfs);
        if (fbConfs == nullptr && msaaWanted)
        {
            MOE_WARNING(moe::ChanDebug, "No FBConfig with %dx MSAA. Searching with other MSAA values.", ctxDesc.SamplesCount);
            // visualAttrs[23] = 0;
            visualAttrs[25] = 0;
            fbConfs = glXChooseFBConfig(display, DefaultScreen(display), visualAttrs, &nbConfs);
            MOE_INFO(moe::ChanDebug, "Searching visuals in %d confs", nbConfs);
            if (fbConfs == nullptr)
            {
                MOE_WARNING(moe::ChanDebug, "No FBConfig with MSAA. Searching without MSAA.");
                visualAttrs[23] = 0;
                msaaWanted = false;
                fbConfs = glXChooseFBConfig(display, DefaultScreen(display), visualAttrs, &nbConfs);
                MOE_INFO(moe::ChanDebug, "Searching visuals in %d confs", nbConfs);
            }
        }

        int iBestConf = -1;
        for (int iConf = 0; iConf < nbConfs; ++iConf)
        {
            // We don't want a FB Config with accumulators because they're slower.
            // Usually, it suffices to check Red accumulator size.
            int accumRed;
            glXGetFBConfigAttrib(display, fbConfs[iConf], GLX_ACCUM_RED_SIZE, &accumRed);

            // Only select a FB config with accumulation if we have no other choice...
            if (!accumRed)
            {
                iBestConf = iConf;
                break; // it's OK
            }
            else if (iBestConf == -1)
            {
                iBestConf = iConf;
            }
        }

        MOE_DEBUG_ASSERT(iBestConf != -1);

        if (iBestConf != -1)
        {
            myVisual = glXGetVisualFromFBConfig(display, fbConfs[iBestConf]);
            MOE_DEBUG_ASSERT(myVisual != nullptr);
        }

        if (myVisual == nullptr)
        {
            MOE_ERROR(moe::ChanGraphics, "GLX context failed to retrieve a X Visual matching given context description!");
        }

        // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
        XFree(fbConfs);

        return myVisual;
    }
}