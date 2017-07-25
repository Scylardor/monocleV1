#ifndef MOE_D3D11_CONTEXT_H_
#define MOE_D3D11_CONTEXT_H_

#include "Core/Preprocessor/moeDLLVisibility.h"
#include "Graphics/Context/moeGraphicsContext.h"
#include "Graphics/Context/moeContextDescriptor.h"
#include "Core/Misc/moeFalse.h"
#include "Core/Preprocessor/moeAssert.h"
#include "Core/Log/moeLogMacros.h"

#include <d3d11.h>
#include <comdef.h>
#include <wrl/client.h> // ComPtr interface


// Convenient macros to generate a DXGI_FORMAT enum value.
#define MOE_COMPOSE_DXGI_FORMAT_DOUBLE_TYPED(format1, type1, format2, type2) DXGI_FORMAT_ ## format1 ## _ ## type1 ## _ ## format2 ## _ ## type2
#define MOE_COMPOSE_DXGI_FORMAT_COLOR_1D(first, type) DXGI_FORMAT_ ## first ## _ ## type
#define MOE_COMPOSE_DXGI_FORMAT_COLOR_2D(first, second, type) MOE_COMPOSE_DXGI_FORMAT_COLOR_1D(first ## second, type)
#define MOE_COMPOSE_DXGI_FORMAT_COLOR_3D(first, second, third, type) MOE_COMPOSE_DXGI_FORMAT_COLOR_1D(first ## second ## third, type)
#define MOE_COMPOSE_DXGI_FORMAT_COLOR_4D(first, second, third, fourth, type) MOE_COMPOSE_DXGI_FORMAT_COLOR_1D(first ## second ## third ## fourth, type)


namespace moe
{

    // A Context traits structure is used for the BaseD3DContext mix-in class to be
    // able to use types defined by its derived classes.
    template <typename Context>
    struct D3DContextTraits
    {
        using Device                    = typename Context::DeviceType;
        using DeviceContext             = typename Context::DeviceContextType;
        using SwapChain                 = typename Context::SwapChainType;
        using Adapter                   = typename Context::AdapterType;
        using Factory                   = typename Context::FactoryType;
        D3D_FEATURE_LEVEL FeatureLevel  = Context::FeatureLevel;
        static_assert(False<Context>::value, "Define a D3DContextTraits specialization for your device context type to be able to inherit BaseD3DContext");
    };


    // The base mix-in class we inherit from to implement all our D3D11 contexts.
    // This class is based on the fact that whatever the subversion, D3D11 initialization
    // always follows the same basic steps.
    // It heavily relies on derived class-declared types, but is supposed to work the same
    // whatever the version.
    template <class VersionedContext>
    class BaseD3DContext : public GraphicsContext
    {
    public:

        MOE_DLL_API virtual void    OnThreadChange() override {}





    protected:
        BaseD3DContext(const ContextDescriptor& contextDesc, HWND winHandle);

        void    SetFullViewport();

        // Avoid manipulating insanely long types.
        typedef typename D3DContextTraits<VersionedContext>::Device         DeviceType;
        typedef typename D3DContextTraits<VersionedContext>::DeviceContext  DeviceContextType;
        typedef typename D3DContextTraits<VersionedContext>::SwapChain      SwapChainType;
        typedef typename D3DContextTraits<VersionedContext>::Adapter        AdapterType;
        typedef typename D3DContextTraits<VersionedContext>::Factory        FactoryType;


        static Microsoft::WRL::ComPtr<FactoryType>  CreateFactory();

        static Microsoft::WRL::ComPtr<AdapterType>  GetDefaultAdapter(Microsoft::WRL::ComPtr<FactoryType>& factory);

        static HRESULT  CreateDevice(Microsoft::WRL::ComPtr<AdapterType>& adapter, Microsoft::WRL::ComPtr<DeviceType>& device, Microsoft::WRL::ComPtr<DeviceContextType>& immediateContext);

        static Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   GetBackBufferView(Microsoft::WRL::ComPtr<DeviceType>& device, Microsoft::WRL::ComPtr<SwapChainType>& swapChain);
        static Microsoft::WRL::ComPtr<ID3D11Texture2D>          GetSwapChainBackBufferTexture(Microsoft::WRL::ComPtr<SwapChainType>& swapChain);

        static HRESULT  CreateDepthStencilBuffer(
            Microsoft::WRL::ComPtr<DeviceType>& device,
            Microsoft::WRL::ComPtr<SwapChainType>& swapChain,
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depthStencilView,
            Microsoft::WRL::ComPtr<ID3D11Texture2D>& depthStencilBuffer,
            DXGI_FORMAT depthStencilFormat);


        static void DescribeAdapter(Microsoft::WRL::ComPtr<AdapterType>& adapter);

        static DXGI_MODE_DESC   GetViewportFittingAdapterMode(
            const Microsoft::WRL::ComPtr<IDXGIAdapter>& adapter,
            DXGI_FORMAT requestedFormat,
            std::uint32_t viewportWidth, std::uint32_t viewportHeight);

        // Since all devices supporting DX11 are required to support at least 4x MSAA,
        // we fallback on this if there is no quality level available for our initially wanted samples count.
        static DXGI_SAMPLE_DESC GetDeviceBestMultisamplingQuality(Microsoft::WRL::ComPtr<DeviceType>& device, DXGI_FORMAT desiredFormat, UINT desiredSampleCount);

        Microsoft::WRL::ComPtr<DeviceType>              m_device;
        Microsoft::WRL::ComPtr<DeviceContextType>       m_immediateContext;
        Microsoft::WRL::ComPtr<SwapChainType>           m_swapChain;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_backBufferView;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;
        Microsoft::WRL::ComPtr<ID3D11Texture2D>         m_depthStencilBuffer;

        static const DXGI_FORMAT    DEFAULT_COLOR_FORMAT        = DXGI_FORMAT_R8G8B8A8_UNORM;
        static const DXGI_FORMAT    DEFAULT_DEPTHSTENCIL_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24-bit depth and 8-bit stencil seems a sensible default value.
    };
}

#include "Graphics/Context/Private/Windows/moeD3D11Context.internal.hpp"


#endif // MOE_D3D11_CONTEXT_H_