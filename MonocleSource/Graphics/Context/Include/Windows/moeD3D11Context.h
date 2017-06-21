#ifndef MOE_D3D11_CONTEXT_H_
#define MOE_D3D11_CONTEXT_H_

#include "Graphics/Context/Include/moeGraphicsContext.h"
#include "Graphics/Window/Include/moeContextDescriptor.h"
#include "Core/Misc/Include/moeFalse.h"
#include "Core/Preprocessor/Include/moeAssert.h"
#include "Core/Log/Include/moeLogMacros.h"

#include <d3d11_2.h>
#include <comdef.h>
#include <wrl/client.h> // ComPtr interface
#include <cinttypes>


#define CRTP_ASSERT_CHILD_FUNC(FuncName) static_assert(std::is_same<decltype(&BaseD3DContext<VersionedContext>::FuncName), decltype(&VersionedContext::FuncName)>::value, "toto")


namespace moe
{
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

    template <class VersionedContext>
    class BaseD3DContext : public GraphicsContext
    {
    public:

        virtual void    OnThreadChange() override {}

        void    SetFullViewport();

        bool    IsFullyInitialized() const;

    protected:
        // Avoid manipulating insanely long types.
        typedef typename D3DContextTraits<VersionedContext>::Device         DeviceType;
        typedef typename D3DContextTraits<VersionedContext>::DeviceContext  DeviceContextType;
        typedef typename D3DContextTraits<VersionedContext>::SwapChain      SwapChainType;
        typedef typename D3DContextTraits<VersionedContext>::Adapter        AdapterType;
        typedef typename D3DContextTraits<VersionedContext>::Factory        FactoryType;

        BaseD3DContext(const ContextDescriptor& contextDesc, HWND winHandle);

        static Microsoft::WRL::ComPtr<FactoryType>  CreateFactory();

        static Microsoft::WRL::ComPtr<AdapterType>  GetDefaultAdapter(Microsoft::WRL::ComPtr<FactoryType>& factory);

        static HRESULT  CreateDevice(Microsoft::WRL::ComPtr<AdapterType>& adapter, Microsoft::WRL::ComPtr<DeviceType>& device, Microsoft::WRL::ComPtr<DeviceContextType>& immediateContext);

        static Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   GetBackBufferView(Microsoft::WRL::ComPtr<DeviceType>& device, Microsoft::WRL::ComPtr<SwapChainType>& swapChain);
        static Microsoft::WRL::ComPtr<ID3D11Texture2D>          GetSwapChainBackBufferTexture(Microsoft::WRL::ComPtr<SwapChainType>& swapChain);

        static HRESULT  CreateDepthStencilBuffer(Microsoft::WRL::ComPtr<DeviceType>& device, Microsoft::WRL::ComPtr<SwapChainType>& swapChain,
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depthStencilView, Microsoft::WRL::ComPtr<ID3D11Texture2D>& depthStencilBuffer);


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
    };
}

#include "moeD3D11Context.internal.hpp"


#endif // MOE_D3D11_CONTEXT_H_