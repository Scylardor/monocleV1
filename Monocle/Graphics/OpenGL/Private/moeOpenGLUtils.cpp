#include "Graphics/OpenGL/moeOpenGLUtils.h"

namespace moe
{

    ColorBitCounter   DecomposeColorEnumRGBABits(GLenum colorFormat)
    {
        ColorBitCounter bitCounter;
        std::array<std::uint8_t, 4>& colorBitsCounts = bitCounter.ComponentBits;
        colorBitsCounts = {{ 0 }};

        switch (colorFormat)
        {
        case GL_R3_G3_B2:
            colorBitsCounts = {{ 3, 3, 2, 0 }};
            break;

        case GL_RGB4:
        case GL_RGB4_S3TC:
            colorBitsCounts = {{ 4, 4, 4, 0 }};
            break;

        case GL_RGB5:
            colorBitsCounts = {{ 5, 5, 5, 0 }};
            break;

        case GL_RGB8:
        case GL_RGB8UI:
        case GL_RGB8I:
        case GL_RGB8_SNORM:
        case GL_SRGB8:
            colorBitsCounts = {{ 8, 8, 8, 0 }};
            break;

        case GL_RGB10:
            colorBitsCounts = {{ 10, 10, 10, 0 }};
            break;

        case GL_RGB12:
            colorBitsCounts = {{ 12, 12, 12, 0 }};
            break;

        case GL_RGB16:
        case GL_RGB16F:
            colorBitsCounts = {{ 16, 16, 16, 0 }};
            break;

        case GL_RGBA2:
            colorBitsCounts = {{ 2, 2, 2, 2 }};
            break;

        case GL_RGBA4:
        case GL_RGBA4_S3TC:
        case GL_RGBA4_DXT5_S3TC:
            colorBitsCounts = {{ 4, 4, 4, 4 }};
            break;

        case GL_RGB5_A1:
            colorBitsCounts = {{ 5, 5, 5, 1 }};
            break;

        case GL_RGBA8:
        case GL_RGBA8UI:
        case GL_RGBA8I:
        case GL_RGBA8_SNORM:
        case GL_SRGB8_ALPHA8:
            colorBitsCounts = {{ 8, 8, 8, 8 }};
            break;

        case GL_RGB10_A2:
        case GL_RGB10_A2UI:
            colorBitsCounts = {{ 10, 10, 10, 2 }};
            break;

        case GL_RGBA12:
            colorBitsCounts = {{ 12, 12, 12, 12 }};
            break;

        case GL_RGBA16:
        case GL_RGBA16F:
        case GL_RGBA16UI:
        case GL_RGBA16I:
        case GL_RGBA16_SNORM:
            colorBitsCounts = {{ 16, 16, 16, 16 }};
            break;

        case GL_RGBA32F:
        case GL_RGBA32UI:
        case GL_RGBA32I:
            colorBitsCounts = {{ 32, 32, 32, 32 }};
            break;

        case GL_RGB32UI:
        case GL_RGB32I:
        case GL_RGB32F:
            colorBitsCounts = {{ 32, 32, 32, 0 }};
            break;

        case GL_RGB16UI:
        case GL_RGB16I:
        case GL_RGB16_SNORM:
            colorBitsCounts = {{ 16, 16, 16, 0 }};
            break;

        case GL_R8:
        case GL_R8I:
        case GL_R8UI:
        case GL_R8_SNORM:
            colorBitsCounts = {{ 8, 0, 0, 0 }};
            break;

        case GL_RG8:
        case GL_RG8I:
        case GL_RG8UI:
        case GL_RG8_SNORM:
            colorBitsCounts = {{ 8, 8, 0, 0 }};
            break;

        case GL_RG16:
        case GL_RG16F:
        case GL_RG16I:
        case GL_RG16UI:
        case GL_RG16_SNORM:
            colorBitsCounts = {{ 16, 16, 0, 0 }};
            break;

        case GL_R16F:
        case GL_R16I:
        case GL_R16UI:
        case GL_R16:
        case GL_R16_SNORM:
            colorBitsCounts = {{ 16, 0, 0, 0 }};
            break;

        case GL_R32F:
        case GL_R32I:
        case GL_R32UI:
            colorBitsCounts = {{ 32, 0, 0, 0 }};
            break;

        case GL_RG32F:
        case GL_RG32I:
        case GL_RG32UI:
            colorBitsCounts = {{ 32, 32, 0, 0 }};
            break;

        case GL_RGB2_EXT:
            colorBitsCounts = {{ 2, 2, 2, 0 }};
            break;

        default:
            // Unmanaged color format
            break;
        }

        return bitCounter;
    }


    DepthStencilBitCounter  DecomposeDepthStencilEnumBits(GLenum depthStencilFormat)
    {
        DepthStencilBitCounter bitCounter;
        std::array<std::uint8_t, 2>& colorBitsCounts = bitCounter.ComponentBits;
        colorBitsCounts = { 0 };

        switch (depthStencilFormat)
        {
        case GL_DEPTH24_STENCIL8:
            colorBitsCounts[0] = 24;
            colorBitsCounts[1] = 8;
            break;
        case GL_DEPTH32F_STENCIL8:
            colorBitsCounts[0] = 32;
            colorBitsCounts[1] = 8;
            break;

        default:
            // Unmanaged depth stencil format.
            break;
        }

        return bitCounter;
    }
}