#ifndef MOE_WINDOW_ATTRIBUTES_H_
#define MOE_WINDOW_ATTRIBUTES_H_

#include <cstdint> // std int types
#include "moePixelFormat.h"

// Window attributes is the platform-agnostic structure Monocle uses
// to query graphics context specifications to the underlying
// platform-specific OS windowing API.
// You can specify one in the Init() method of your moe::App specialization,
// or Monocle will pass a default one to its graphics context.
namespace moe
{
    struct WindowAttributes
    {
        std::uint32_t   Width                   = DEFAULT_WIDTH;
        std::uint32_t   Height                  = DEFAULT_HEIGHT;
        const wchar_t*  Title                   = DEFAULT_TITLE;
        PixelFormat     BufferPixelFormat;  // Bits allocated to RGBA, Depth, Stencil etc.
        std::uint8_t    BufferCount             = DEFAULT_BUFFER_COUNT; // Number of buffers used, 2 for double buffering
        std::uint8_t    SamplesCount            = DEFAULT_SAMPLES_COUNT; // Number of samples used for MSAA
        std::int8_t     WantedAPI_MajorVersion  = DEFAULT_API_MAJOR_VERSION;
        std::int8_t     WantedAPI_MinorVersion  = DEFAULT_API_MINOR_VERSION;

        // DEFAULT_WIDTH * DEFAULT_HEIGHT = a 16:9 ratio that probably won't take full screen
        static const std::uint32_t  DEFAULT_WIDTH   = 1280;
        static const std::uint32_t  DEFAULT_HEIGHT  = 720;
        static const wchar_t*       DEFAULT_TITLE;
        static const std::uint8_t   DEFAULT_BUFFER_COUNT = 2;   // Double buffering by default
        static const std::uint8_t   DEFAULT_SAMPLES_COUNT = 4;  // Align default on D3D11's guarantee to support at least MSAA x4
        static const std::int8_t    DEFAULT_API_MAJOR_VERSION = -1; // = sort of "DONT_CARE" = take whatever version available (probably won't be used)
        static const std::int8_t    DEFAULT_API_MINOR_VERSION = -1;
    };
}

#endif // MOE_WINDOW_ATTRIBUTES_H_