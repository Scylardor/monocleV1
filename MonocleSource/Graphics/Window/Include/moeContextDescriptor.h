#ifndef MOE_CONTEXT_DESCRIPTOR_H_
#define MOE_CONTEXT_DESCRIPTOR_H_

#include "moeDLLVisibility.h"
#include <cstdint>

// A data holder that describes all we need to know to allocate
// a graphic context:
// - the bits per color / per pixel we want to allocate for pixel data in our window
// - depth and stencil buffer bpp
// - number of samples to use for MSAA
// - double-buffering enabled or not.
// It will then be used and converted by the platform-specific
// layer to express our choice with the appropriate API.
namespace moe
{
    struct MOE_DLL_API ContextDescriptor
    {
        // Since OpenGL has an extension to enable adaptive V-Sync (which offers better performance in case of FPS drops),
        // a boolean flag is not enough.
        // The only sensible use to the "v-sync enabled but not adaptive" would be for profiling reasons, though.
        // There's no such configuration on D3D contexts. They shall fallback on "regular" v-sync if they see this flag set.
        enum class VSyncFlag : std::uint8_t
        {
            VSF_Disabled =              1 << 0,
            VSF_Enabled =               1 << 1,
            VSF_Enabled_NotAdaptive =  (1 << 2) | VSF_Enabled
        };

        // Fully-qualifying every parameter constructor. Delegates every other ctor
        ContextDescriptor(
            std::uint8_t rBits, std::uint8_t gBits, std::uint8_t bBits, std::uint8_t aBits,
            std::uint8_t depthBits, std::uint8_t stencilBits,
            std::uint8_t numSamples,
            std::int8_t apiMajorVersion, std::int8_t apiMinorVersion,
            std::uint8_t numBuffers, VSyncFlag useVSync,
            std::uint32_t width, std::uint32_t height) :
            R(rBits),
            G(gBits),
            B(bBits),
            A(aBits),
            DepthBits(depthBits),
            StencilBits(stencilBits),
            SamplesCount(numSamples),
            API_MajorVersion(apiMajorVersion),
            API_MinorVersion(apiMinorVersion),
            BuffersCount(numBuffers),
            VSyncState(useVSync),
            ViewportWidth(width),
            ViewportHeight(height)
        {
        }

        // Version in which you specify the same number of bits for all R, G, B and A color channels.
        ContextDescriptor(
            std::uint8_t rgbaBits = DEFAULT_BITS_PER_COLOR,
            std::uint8_t depthBits = DEFAULT_DEPTH_BITS, std::uint8_t stencilBits = DEFAULT_STENCIL_BITS,
            std::uint8_t numSamples = DEFAULT_SAMPLES_COUNT,
            std::int8_t apiMajorVersion = DEFAULT_API_MAJOR_VERSION, std::int8_t apiMinorVersion = DEFAULT_API_MINOR_VERSION,
            std::uint8_t bufCount = DEFAULT_BUFFERS_COUNT, VSyncFlag useVSync = DEFAULT_USE_VSYNC,
            std::uint32_t width = DEFAULT_WIDTH, std::uint32_t height = DEFAULT_HEIGHT) :
            ContextDescriptor(rgbaBits, rgbaBits, rgbaBits, rgbaBits, depthBits, stencilBits, numSamples, apiMajorVersion, apiMinorVersion, bufCount, useVSync, width, height)
        {
        }

        // Version in which you specify the same number of bits for all R, G, B color channels and a different value for Alpha.
        ContextDescriptor(
            std::uint8_t rgbBits, std::uint8_t alphaBits,
            std::uint8_t depthBits = DEFAULT_DEPTH_BITS, std::uint8_t stencilBits = DEFAULT_STENCIL_BITS,
            std::uint8_t numSamples = DEFAULT_SAMPLES_COUNT,
            std::int8_t apiMajorVersion = DEFAULT_API_MAJOR_VERSION, std::int8_t apiMinorVersion = DEFAULT_API_MINOR_VERSION,
            std::uint8_t bufCount = DEFAULT_BUFFERS_COUNT, VSyncFlag useVSync = DEFAULT_USE_VSYNC,
            std::uint32_t width = DEFAULT_WIDTH, std::uint32_t height = DEFAULT_HEIGHT) :
            ContextDescriptor(rgbBits, rgbBits, rgbBits, alphaBits, depthBits, stencilBits, numSamples, apiMajorVersion, apiMinorVersion, bufCount, useVSync, width, height)
        {
        }

        // Returns total number of bits used by a color pixel
        std::uint8_t    ColorBits() const
        {
            return  R + G + B + A;
        }

        // Colors
        std::uint8_t    R;
        std::uint8_t    G;
        std::uint8_t    B;
        std::uint8_t    A;

        // Viewport
        std::uint32_t   ViewportWidth = DEFAULT_WIDTH;
        std::uint32_t   ViewportHeight = DEFAULT_HEIGHT;

        // Depth/stencil
        std::uint8_t    DepthBits;
        std::uint8_t    StencilBits;

        // MSAA / Buffering
        std::uint8_t    SamplesCount;
        std::uint8_t    BuffersCount;   // Rationale is that it's the count of *back* buffers, NOT total buffers. Hence, 1 = double buffering, 2 = triple buffering...

        // API versioning
        std::int8_t     API_MajorVersion;
        std::int8_t     API_MinorVersion;

        // VSync
        VSyncFlag       VSyncState;


        // Default are quite arbitrary, based on the most commonly used values for these usages...
        static const std::uint8_t   DEFAULT_BITS_PER_COLOR      = 8u;
        static const std::uint8_t   DEFAULT_DEPTH_BITS          = 24u;
        static const std::uint8_t   DEFAULT_STENCIL_BITS        = 8u;
        static const std::uint8_t   DEFAULT_SAMPLES_COUNT       = 1u;   // No MSAA by default.
        static const std::uint8_t   DEFAULT_BUFFERS_COUNT       = 1u;   // Use double buffering by default.
        static const std::int8_t    DEFAULT_API_MAJOR_VERSION   = -1;
        static const std::int8_t    DEFAULT_API_MINOR_VERSION   = -1;
        static const VSyncFlag      DEFAULT_USE_VSYNC           = VSyncFlag::VSF_Disabled;
        // DEFAULT_WIDTH * DEFAULT_HEIGHT = a 16:9 ratio that probably won't take full screen
        // TODO: maybe ask the system the ideal default size instead ?
        static const std::uint32_t  DEFAULT_WIDTH = 1280;
        static const std::uint32_t  DEFAULT_HEIGHT = 720;
    };
}

#endif // MOE_CONTEXT_DESCRIPTOR_H_
