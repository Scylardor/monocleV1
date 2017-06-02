#ifndef MOE_PIXEL_FORMAT_H_
#define MOE_PIXEL_FORMAT_H_

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
        ContextDescriptor(std::uint8_t rgbaBits = DEFAULT_BITS_PER_COLOR, std::uint8_t depthBits = DEFAULT_DEPTH_BITS, std::uint8_t stencilBits = DEFAULT_STENCIL_BITS,
            std::uint8_t numSamples = DEFAULT_SAMPLES_COUNT, std::int8_t apiMajorVersion = DEFAULT_API_MAJOR_VERSION, std::int8_t apiMinorVersion = DEFAULT_API_MINOR_VERSION,
            bool hasDoubleBuffering = DEFAULT_DOUBLE_BUFFERING) :
            DepthBits(depthBits),
            StencilBits(stencilBits),
            SamplesCount(numSamples),
            API_MajorVersion(apiMajorVersion),
            API_MinorVersion(apiMinorVersion),
            UseDoubleBuffering(hasDoubleBuffering)
        {
            R = G = B = A = rgbaBits;
        }

        ContextDescriptor(std::uint8_t rgbBits, std::uint8_t alphaBits, std::uint8_t depthBits = DEFAULT_DEPTH_BITS, std::uint8_t stencilBits = DEFAULT_STENCIL_BITS,
            std::uint8_t numSamples = DEFAULT_SAMPLES_COUNT, std::int8_t apiMajorVersion = DEFAULT_API_MAJOR_VERSION, std::int8_t apiMinorVersion = DEFAULT_API_MINOR_VERSION,
            bool hasDoubleBuffering = DEFAULT_DOUBLE_BUFFERING) :
            A(alphaBits),
            DepthBits(depthBits),
            StencilBits(stencilBits),
            SamplesCount(numSamples),
            API_MajorVersion(apiMajorVersion),
            API_MinorVersion(apiMinorVersion),
            UseDoubleBuffering(hasDoubleBuffering)
        {
            R = G = B = rgbBits;
        }
        ContextDescriptor(std::uint8_t rBits, std::uint8_t gBits, std::uint8_t bBits, std::uint8_t aBits,
            std::uint8_t depthBits = DEFAULT_DEPTH_BITS, std::uint8_t stencilBits = DEFAULT_STENCIL_BITS,
            std::uint8_t numSamples = DEFAULT_SAMPLES_COUNT,
            std::int8_t apiMajorVersion = DEFAULT_API_MAJOR_VERSION, std::int8_t apiMinorVersion = DEFAULT_API_MINOR_VERSION,
            bool hasDoubleBuffering = DEFAULT_DOUBLE_BUFFERING) :
            R(rBits),
            G(gBits),
            B(bBits),
            A(aBits),
            DepthBits(depthBits),
            StencilBits(stencilBits),
            SamplesCount(numSamples),
            API_MajorVersion(apiMajorVersion),
            API_MinorVersion(apiMinorVersion),
            UseDoubleBuffering(hasDoubleBuffering)
        {}

        // Returns total number of bits used by a color pixel
        std::uint8_t    ColorBits() const
        {
            return  R + G + B + A;
        }

        std::uint8_t    R;
        std::uint8_t    G;
        std::uint8_t    B;
        std::uint8_t    A;
        std::uint8_t    DepthBits;
        std::uint8_t    StencilBits;
        std::uint8_t    SamplesCount;
        std::int8_t     API_MajorVersion;
        std::int8_t     API_MinorVersion;
        bool            UseDoubleBuffering;


        // Default are quite arbitrary, based on the most commonly used values for these usages...
        static const std::uint8_t   DEFAULT_BITS_PER_COLOR      = 8u;
        static const std::uint8_t   DEFAULT_DEPTH_BITS          = 24u;
        static const std::uint8_t   DEFAULT_STENCIL_BITS        = 8u;
        static const std::uint8_t   DEFAULT_SAMPLES_COUNT       = 2u;
        static const std::int8_t    DEFAULT_API_MAJOR_VERSION   = -1;
        static const std::int8_t    DEFAULT_API_MINOR_VERSION   = -1;
        static const std::uint8_t   DEFAULT_DOUBLE_BUFFERING    = true;
    };
}

#endif // MOE_PIXEL_FORMAT_H_