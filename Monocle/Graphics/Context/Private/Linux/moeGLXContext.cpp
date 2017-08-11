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
        MOE_INFO(moe::ChanGraphics, "Contxt created");
        // We need at least GLX 1.3 for FB configs
        int major, minor;
        if (!MOE_ASSERT(glXQueryVersion(display, &major, &minor)) || ((major == 1) && (minor < 3)) || (major < 1))
        {
            MOE_ERROR(moe::ChanGraphics, "GLX is too old! X11 window can't initialize.");
            return;
        }
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
        const bool msaaWanted = (ctxDesc.SamplesCount > 1);

        const int visualAttrs[] =
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
            GLX_DOUBLEBUFFER    ,   (ctxDesc.BuffersCount > 0),
            GLX_SAMPLE_BUFFERS  ,   msaaWanted,
            GLX_SAMPLES         ,   ctxDesc.SamplesCount,
            GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB, isSRGB_Color,
            None
        };

        // Then iterate on available FB configs to find a compatible visual
        XVisualInfo* myVisual = nullptr;

        int nbConfs = 0;
        GLXFBConfig* fbConfs = glXChooseFBConfig(display, DefaultScreen(display), visualAttrs, &nbConfs);

        for (int iConf = 0; iConf < nbConfs; ++iConf)
        {
            GLXFBConfig& curConf = fbConfs[iConf];
            int msaaOn = 0;
            int nbSamples = 0;
            int sRGBCapable = 0;
            glXGetFBConfigAttrib(display, curConf, GLX_SAMPLE_BUFFERS, &msaaOn);
            glXGetFBConfigAttrib(display, curConf, GLX_SAMPLES, &nbSamples);
            glXGetFBConfigAttrib(display, curConf, GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB, &sRGBCapable);

            if (msaaOn == msaaWanted && nbSamples == ctxDesc.SamplesCount && sRGBCapable == isSRGB_Color)
            {
                // we found a good one
                myVisual = glXGetVisualFromFBConfig(display, curConf);
                MOE_DEBUG_ASSERT(myVisual != nullptr);

                break;
            }
        }

        // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
        XFree(fbConfs);

        return myVisual;
    }
}