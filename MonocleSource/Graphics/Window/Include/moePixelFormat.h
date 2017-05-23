#ifndef MOE_PIXEL_FORMAT_H_
#define MOE_PIXEL_FORMAT_H_

#include "moeDLLVisibility.h"
#include <cstdint>

// A data holder that describes how many bits we
// want to allocate for pixel data in our window
// context for both color bits (RGB and alpha),
// and depth and stencil buffer in a platform-agnostic way.
// It will then be used and converted by the platform-specific
// layer to express our choice with the appropriate API.
namespace moe
{
    struct MOE_DLL_API PixelFormat
    {
        PixelFormat(std::uint8_t rgbaBits = DEFAULT_BITS_PER_COLOR, std::uint8_t depthBits = DEFAULT_DEPTH_BITS, std::uint8_t stencilBits = DEFAULT_STENCIL_BITS) :
            depth(depthBits), stencil(stencilBits)
        {
            r = g = b = a = rgbaBits;
        }

        PixelFormat(std::uint8_t rgbBits, std::uint8_t alphaBits, std::uint8_t depthBits = DEFAULT_DEPTH_BITS, std::uint8_t stencilBits = DEFAULT_STENCIL_BITS) :
            a(alphaBits), depth(depthBits), stencil(stencilBits)
        {
            r = g = b = rgbBits;
        }
        PixelFormat(std::uint8_t rBits, std::uint8_t gBits, std::uint8_t bBits, std::uint8_t aBits,
            std::uint8_t depthBits = DEFAULT_DEPTH_BITS, std::uint8_t stencilBits = DEFAULT_STENCIL_BITS) :
            r(rBits), g(gBits), b(bBits), a(aBits), depth(depthBits), stencil(stencilBits)
        {}

        std::uint8_t    r;
        std::uint8_t    g;
        std::uint8_t    b;
        std::uint8_t    a;
        std::uint8_t    depth;
        std::uint8_t    stencil;

        // Default are quite arbitrary, based on the most commonly used values for these usages...
        static const std::uint8_t   DEFAULT_BITS_PER_COLOR  = 8u;
        static const std::uint8_t   DEFAULT_DEPTH_BITS      = 24u;
        static const std::uint8_t   DEFAULT_STENCIL_BITS    = 8u;
    };
}

#endif // MOE_PIXEL_FORMAT_H_