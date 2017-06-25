#include "Graphics/Context/Include/Windows/moeD3DContext_11_2.h"

namespace moe
{
    D3DContext_11_2::D3DContext_11_2(const ContextDescriptor& contextDesc, HWND winHandle) :
        BaseD3DContext<D3DContext_11_2>(contextDesc, winHandle)
    {
        // If we want multisampling, there's more work to do. Because starting from DirectX 11.2 and its flip swap effects,
        // swap chains don't support automatic MSAA anymore. So if we want multisampling, we need to create a multisampled offscreen render target
        // that we'll render our scenes into, and then blit this render target into the no-MSAA back buffer using ResolveSubresource.
        if (contextDesc.SamplesCount > 1)
        {
            MOE_ASSERT(SUCCEEDED(CreateMultisampledOffscreenTarget(contextDesc.SamplesCount)));
        }
    }


    HRESULT D3DContext_11_2::CreateMultisampledOffscreenTarget(UINT samplesCount)
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferTex = nullptr;
        HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>( backBufferTex.GetAddressOf() ));
        if (FAILED(hr))
        {
            _com_error err(hr);
            MOE_ERROR(moe::ChanGraphics, "D3D 11.2 Context failed to retrieve back buffer into a 2D Texture. Last COM error: %ls", err.ErrorMessage());
            return S_FALSE;
        }

        // We share a lot of properties with the actual back buffer so we can base ourselves on its description for multisampling
        D3D11_TEXTURE2D_DESC desc = { 0 };
        backBufferTex->GetDesc(&desc);
        // Width/height, mip levels, format, flags, are to be the same than actual back buffer ones
        desc.Usage = D3D11_USAGE_DEFAULT; // we'll be using ResolveSubresource on this so GPU will read/write I guess

        // Check if wanted sampling count is supported
        DXGI_SAMPLE_DESC samplesDesc = GetDeviceBestMultisamplingQuality(m_device, desc.Format, samplesCount);
        if (samplesDesc.Count < 2)
        {
            MOE_WARNING(moe::ChanGraphics, "D3D 11.2 Context device doesn't support multisampling. MSAA will be disabled.");
            return S_FALSE;
        }

        MOE_ASSERT(samplesDesc.Quality > 0);
        desc.SampleDesc.Count = samplesDesc.Count;
        desc.SampleDesc.Quality = samplesDesc.Quality - 1; // valid range starts at 0

        Microsoft::WRL::ComPtr<ID3D11Texture2D> multisampledTexture;
        // No subresource data
        hr = m_device->CreateTexture2D(&desc, nullptr, multisampledTexture.GetAddressOf());
        if (FAILED(hr))
        {
            _com_error err(hr);
            MOE_WARNING(moe::ChanGraphics, "D3D 11.2 Context failed to create a multisampled texture 2D for MSAA offscreen rendering. MSAA will be disabled. Last COM error: %ls", err.ErrorMessage());
            return S_FALSE;
        }
        else
        {
            hr = m_device->CreateRenderTargetView(
                multisampledTexture.Get(),
                nullptr, // No render target description
                m_offscreenMSAATarget.GetAddressOf());
            if (FAILED(hr))
            {
                _com_error err(hr);
                MOE_WARNING(moe::ChanGraphics, "D3D 11.2 Context failed to create a view to its offscreen multisampled render target. MSAA will be disabled. Last COM error: %ls", err.ErrorMessage());
            }
        }

        return hr;
    }


    void D3DContext_11_2::DescribeAdapter(Microsoft::WRL::ComPtr<IDXGIAdapter2>& adapter)
    {
        DXGI_ADAPTER_DESC2 desc;

        adapter->GetDesc2(&desc);
        MOE_INFO(moe::ChanGraphics, "Adapter %ls: Vendor ID %lu, Device ID %lu, Revision %lu, dedicated video memory: %lu MB.",
            desc.Description, desc.VendorId, desc.DeviceId, desc.Revision, desc.DedicatedVideoMemory / 1024 / 1024);
        MOE_INFO(moe::ChanGraphics, "Graphics preemption granularity: %d, Compute preemption granularity: %d",
            desc.GraphicsPreemptionGranularity, desc.ComputePreemptionGranularity);
    }


    Microsoft::WRL::ComPtr<IDXGISwapChain2> D3DContext_11_2::CreateFullWindowSwapChain(
        const ContextDescriptor& contextDesc,
        Microsoft::WRL::ComPtr<IDXGIFactory2>& factory,
        Microsoft::WRL::ComPtr<IDXGIAdapter2>&, // unused
        Microsoft::WRL::ComPtr<ID3D11Device2>& device,
        HWND winHandle)
    {
        // in DirectX 11.2, we use the flip sequential swap effect, and the DXGI flip model sets the following requirements:
        // - only three supported formats: DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_B8G8R8A8_UNORM or DXGI_FORMAT_R8G8B8A8_UNORM
        // - no MSAA
        // - a buffer count between 2 and 16
        // cf. https://msdn.microsoft.com/en-us/library/windows/desktop/hh706346(v=vs.85).aspx
        DXGI_FORMAT swapChainFormat = DXGI_FORMAT(contextDesc.ColorFormat == ContextDescriptor::DEFAULT_GENERIC_FORMAT ? DEFAULT_COLOR_FORMAT : contextDesc.ColorFormat);
        if (swapChainFormat != DXGI_FORMAT_R16G16B16A16_FLOAT && swapChainFormat != DXGI_FORMAT_B8G8R8A8_UNORM && swapChainFormat != DXGI_FORMAT_R8G8B8A8_UNORM)
        {
            MOE_ERROR(moe::ChanGraphics, "D3D 11.2 Context doesn't support the desired format for swap chain. Supported formats: DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_B8G8R8A8_UNORM or DXGI_FORMAT_R8G8B8A8_UNORM.");
            return nullptr;
        }

        DXGI_SAMPLE_DESC msaaDesc;
        msaaDesc.Count = 1;
        msaaDesc.Quality = 0;

        // TODO: check if full-screen mode: the count isn't the same. When you create a full-screen swap chain, you typically include the front buffer in this value.
        // Monocle context descriptions use the number of *back* buffers, so basically we would need to use contextDesc.BuffersCount + 1.
        UINT bufferCount = contextDesc.BuffersCount;
        const UINT minSupportedCount = 2;
        const UINT maxSupportedCount = 16;
        if (bufferCount < minSupportedCount)
        {
            MOE_WARNING(moe::ChanGraphics, "D3D 11.2 Context requires at least %lu buffers. Using this value in the swap chain description.", minSupportedCount);
            bufferCount = minSupportedCount;
        }
        else if (bufferCount > maxSupportedCount)
        {
            MOE_WARNING(moe::ChanGraphics, "D3D 11.2 Context doesn't support more than %lu buffers. Using this value the swap chain description.", maxSupportedCount);
            bufferCount = maxSupportedCount;
        }

        DXGI_SWAP_CHAIN_DESC1 scd;
        ZeroMemory(&scd, sizeof(scd));

        // scd.Width/Height: Leaving them to 0 is OK because it will use the window dimensions.
        scd.Format = swapChainFormat;
        // scd.Stereo = 0, No stereo management for now
        scd.SampleDesc = msaaDesc;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        // Using a value of at least 2 to be compatible with flip swap effects.
        // cf. https://msdn.microsoft.com/en-us/library/windows/desktop/hh404528(v=vs.85).aspx
        scd.BufferCount = bufferCount;
        // Leaving scd.Scaling to 0 = DXGI_SCALING_STRETCH is OK
        scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // DX 11.2 new flip mode
        // Leaving scd.AlphaMode to 0 = DXGI_ALPHA_MODE_UNSPECIFIED is OK
        // Leaving scd.Flags to 0 as we don't manage additional flags yet. TODO: manage them (e.g. for fullscreen)

        // First query a IDXGISwapChain1 to transform it into a IDXGISwapChain2.
        Microsoft::WRL::ComPtr<IDXGISwapChain1> tmpSwapChain;
        HRESULT hr = factory->CreateSwapChainForHwnd(
            device.Get(),
            winHandle,
            &scd,
            nullptr, // no full screen description (for now)
            nullptr, // no output restriction
            tmpSwapChain.GetAddressOf());

        if (FAILED(hr))
        {
            _com_error err(hr);
            MOE_ERROR(moe::ChanGraphics, "D3D 11.2 Context failed to create the swap chain. Last COM error: %ls", err.ErrorMessage());
            return nullptr;
        }

        Microsoft::WRL::ComPtr<IDXGISwapChain2> swapChain2;
        hr = tmpSwapChain.As(&swapChain2);
        if (FAILED(hr))
        {
            _com_error err(hr);
            MOE_ERROR(moe::ChanGraphics, "D3D 11.2 Context failed to convert the swap chain to a DXGI 1.3 one. Last COM error: %ls", err.ErrorMessage());
            return nullptr;
        }

        return swapChain2;
    }




}