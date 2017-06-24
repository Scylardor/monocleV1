#ifndef MOE_OPENGL_UTILS_H_
#define MOE_OPENGL_UTILS_H_

#include "glad/glad.h" // OpenGL enums

#include <array>
#include <cinttypes>

// Monocle engine utilities for OpenGL APIs.
namespace moe
{
    struct ColorBitCounter
    {
        std::array<std::uint8_t, 4> ColorBits;

        std::uint8_t RedBits()
        {
            return ColorBits[0];
        }

        std::uint8_t GreenBits()
        {
            return ColorBits[1];
        }

        std::uint8_t BlueBits()
        {
            return ColorBits[2];
        }

        std::uint8_t AlphaBits()
        {
            return ColorBits[3];
        }


        std::uint8_t    RGB_Bits()
        {
            return RedBits() + GreenBits() + BlueBits();
        }
    };


    // Takes in an OpenGL color enum format, and returns an array of bits count for each R,G,B and A color.
    ColorBitCounter   DecomposeColorEnumRGBABits(GLenum colorFormat);
}


#endif // MOE_OPENGL_UTILS_H_
