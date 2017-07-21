#ifndef MOE_OPENGL_UTILS_H_
#define MOE_OPENGL_UTILS_H_

#include "glad/glad.h" // OpenGL enums

#include <array>
#include <cinttypes>

// Monocle engine utilities for OpenGL APIs.
namespace moe
{
    // A generic bit counter, simply implemented as an array of bytes.
    // Given that usually, we specify a maximum of 32 bits per color, it seems a bit wasteful...
    template <size_t N>
    struct OGLFormatBitCounter
    {
        std::array<std::uint8_t, N> ComponentBits;
    };

    // Specialization of bit counter to count the bits of each color component from a GLenum image format.
    struct ColorBitCounter : public OGLFormatBitCounter<4>
    {
        std::uint8_t    RedBits()
        {
            return ComponentBits[0];
        }

        std::uint8_t    GreenBits()
        {
            return ComponentBits[1];
        }

        std::uint8_t    BlueBits()
        {
            return ComponentBits[2];
        }

        std::uint8_t    AlphaBits()
        {
            return ComponentBits[3];
        }

        std::uint8_t    RGB_Bits()
        {
            return RedBits() + GreenBits() + BlueBits();
        }
    };


    // Takes in an OpenGL color enum format, and returns an array of bits count for each R,G,B and A color.
    ColorBitCounter         DecomposeColorEnumRGBABits(GLenum colorFormat);


    // Specialization of bit counter to count the bits of depth and stencil component from a GLenum depth-stencil format.
    struct DepthStencilBitCounter : public OGLFormatBitCounter<2>
    {
        std::uint8_t    DepthBits()
        {
            return ComponentBits[0];
        }

        std::uint8_t    StencilBits()
        {
            return ComponentBits[1];
        }
    };

    // Takes in an OpenGL color enum format, and returns an array of bits count for each R,G,B and A color.
    DepthStencilBitCounter  DecomposeDepthStencilEnumBits(GLenum depthStencilFormat);
}


#endif // MOE_OPENGL_UTILS_H_
