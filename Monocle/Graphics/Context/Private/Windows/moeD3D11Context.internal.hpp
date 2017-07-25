// For internal use only by moeD3D11Context.h

// The return types of some functions can be pretty funky in this file
// because of the statics returning templated-types smart COM pointers which makes for very long type names.
// To improve readability I exceptionally adopted kind of a K&R style if the return type is too complex:
// return_type
// functionName(Params...)
// {
//      ...
// }

#include <vector>

namespace moe
{
    template <class VersionedContext>
    BaseD3DContext<VersionedContext>::BaseD3DContext(const ContextDescriptor& contextDesc, HWND winHandle)
    {
        // First create a factory
        Microsoft::WRL::ComPtr<FactoryType> factory = CreateFactory();

        if (!MOE_ASSERT(factory != nullptr))
        {
            return;
        }

        // We're using the default adapter
        Microsoft::WRL::ComPtr<AdapterType> defaultAdapter = GetDefaultAdapter(factory);
        if (!MOE_ASSERT(defaultAdapter != nullptr))
        {
            return;
        }

        #if defined(MOE_DEBUG) || defined(MOE_DIAGNOSTIC)
        // Show some infos about chosen adapter.
        // The formatting/data can vary given the version so make this a versioned call
        VersionedContext::DescribeAdapter(defaultAdapter);
        #endif

        // Now create the device
        HRESULT hr = CreateDevice(defaultAdapter, m_device, m_immediateContext);
        if (!MOE_ASSERT(SUCCEEDED(hr)))
        {
            return;
        }

        // And create a swap chain that will fit all window space by default
        m_swapChain = VersionedContext::CreateFullWindowSwapChain(contextDesc, factory, defaultAdapter, m_device, winHandle);
        MOE_DEBUG_ASSERT(m_swapChain != nullptr);

        // First get the back buffer render target view
        m_backBufferView = GetBackBufferView(m_device, m_swapChain);
        if (!MOE_ASSERT(m_backBufferView != nullptr))
        {
            MOE_ERROR(moe::ChanGraphics, "D3D11 context failed to create a render target view to the back buffer.");
            return;
        }

        // Then create the depth/stencil buffer
        DXGI_FORMAT depthStencilFormat = DXGI_FORMAT(contextDesc.DepthStencilFormat == ContextDescriptor::DEFAULT_GENERIC_FORMAT ? DXGI_FORMAT_D24_UNORM_S8_UINT : contextDesc.DepthStencilFormat);
        hr = CreateDepthStencilBuffer(m_device, m_swapChain, m_depthStencilView, m_depthStencilBuffer, depthStencilFormat);
        if (!MOE_ASSERT(SUCCEEDED(hr)))
        {
            MOE_ERROR(moe::ChanGraphics, "D3D11 context failed to create a depth stencil buffer.");
            return;
        }

        // And finally bind them to the output merger
        m_immediateContext->OMSetRenderTargets(1, m_backBufferView.GetAddressOf(), nullptr);

        SetFullViewport();
    }


    // First create a base DXGI factory, then try to convert it to our versioned context's actual factory type.
    template <class VersionedContext>
    Microsoft::WRL::ComPtr<typename BaseD3DContext<VersionedContext>::FactoryType>
        BaseD3DContext<VersionedContext>::CreateFactory()
    {
        Microsoft::WRL::ComPtr<IDXGIFactory> factory;
        HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory), reinterpret_cast<void **>( factory.GetAddressOf() ));
        if (FAILED(hr))
        {
            _com_error err(hr);
            MOE_ERROR(moe::ChanGraphics, "D3D11 context CreateDXGIFactory failed. Last COM error: %ls", err.ErrorMessage());
            return nullptr;
        }

        Microsoft::WRL::ComPtr<FactoryType> versionedFactory;
        hr = factory.As(&versionedFactory);
        if (FAILED(hr))
        {
            _com_error err(hr);
            MOE_ERROR(moe::ChanGraphics, "D3D11 context couldn't convert base DXGI factory to requested factory version. Last COM error: %ls", err.ErrorMessage());
            return nullptr;
        }

        return versionedFactory;
    }


    template <class VersionedContext>
    Microsoft::WRL::ComPtr<typename BaseD3DContext<VersionedContext>::AdapterType>
        BaseD3DContext<VersionedContext>::GetDefaultAdapter(Microsoft::WRL::ComPtr<FactoryType>& factory)
    {
        Microsoft::WRL::ComPtr<IDXGIAdapter> defaultAdapter;
        HRESULT hr = factory->EnumAdapters(0, defaultAdapter.GetAddressOf());
        if (FAILED(hr))
        {
            _com_error err(hr);
            MOE_ERROR(moe::ChanGraphics, "D3D11 context cannot enumerate adapters. Last COM error: %ls", err.ErrorMessage());
            return nullptr;
        }

        Microsoft::WRL::ComPtr<AdapterType> versionedDefaultAdapter;
        hr = defaultAdapter.As(&versionedDefaultAdapter);
        if (FAILED(hr))
        {
            _com_error err(hr);
            MOE_ERROR(moe::ChanGraphics, "D3D11 context cannot convert default adapter to the requested version. Last COM error: %ls", err.ErrorMessage());
            return nullptr;
        }

        return versionedDefaultAdapter;
    }


    template <class VersionedContext>
    HRESULT BaseD3DContext<VersionedContext>::CreateDevice(
        Microsoft::WRL::ComPtr<typename BaseD3DContext<VersionedContext>::AdapterType>& adapter,
        Microsoft::WRL::ComPtr<typename BaseD3DContext<VersionedContext>::DeviceType>& device,
        Microsoft::WRL::ComPtr<typename BaseD3DContext<VersionedContext>::DeviceContextType>& immediateContext)
    {
        UINT createDeviceFlags = 0;
#if defined(MOE_DEBUG) || defined(MOE_DIAGNOSTIC)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG /* | D3D11_CREATE_DEVICE_DEBUGGABLE */; // TODO: need to research why D3D11_CREATE_DEVICE_DEBUGGABLE isn't supported in hardware
#endif

        D3D_FEATURE_LEVEL wantedLevel[] = { D3DContextTraits<VersionedContext>::FeatureLevel };
        D3D_FEATURE_LEVEL chosenLevel;
        Microsoft::WRL::ComPtr<ID3D11Device> tmpDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> tmpImmediateContext;

        HRESULT hr = D3D11CreateDevice(
            adapter.Get(),              // our adapter
            D3D_DRIVER_TYPE_UNKNOWN,    // We use UNKNOWN because we input an already known adapter
            nullptr,                    // no software device
            createDeviceFlags,
            wantedLevel,              // Pass our feature level array
            ARRAYSIZE(wantedLevel),
            D3D11_SDK_VERSION,
            tmpDevice.GetAddressOf(),
            &chosenLevel,
            tmpImmediateContext.GetAddressOf()
        );

        if (FAILED(hr))
        {
            _com_error err(hr);
            MOE_ERROR(moe::ChanGraphics, "D3D 11.2 Context cannot create a device. Last COM error: %ls", err.ErrorMessage());
            return S_FALSE;
        }

        hr = tmpDevice.As(&device);
        if (SUCCEEDED(hr))
        {
            hr = tmpImmediateContext.As(&immediateContext);
        }

        if (FAILED(hr))
        {
            _com_error err(hr);
            MOE_ERROR(moe::ChanGraphics, "D3D 11.2 Context fails to convert the device to a 11.2 device. Last COM error: %ls", err.ErrorMessage());
            return S_FALSE;
        }

        return hr;
    }


    template <class VersionedContext>
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> BaseD3DContext<VersionedContext>::GetBackBufferView(
        Microsoft::WRL::ComPtr<typename BaseD3DContext<VersionedContext>::DeviceType>& device,
        Microsoft::WRL::ComPtr<typename BaseD3DContext<VersionedContext>::SwapChainType>& swapChain)
    {
        if (!MOE_ASSERT(device != nullptr && swapChain != nullptr))
        {
            MOE_ERROR(moe::ChanGraphics, "D3D11 Context needs a valid device and swap chain to retrieve the back buffer view.");
            return nullptr;
        }

        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferTex = GetSwapChainBackBufferTexture(swapChain);
        if (backBufferTex == nullptr)
        {
            return nullptr;
        }

        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferView;
        HRESULT hr = device->CreateRenderTargetView(
            backBufferTex.Get(),
            nullptr, // No render target description
            backBufferView.GetAddressOf());
        if (FAILED(hr))
        {
            _com_error err(hr);
            MOE_ERROR(moe::ChanGraphics, "D3D11 Context failed to create a render target view on the back buffer! Last COM error: %ls", err.ErrorMessage());
            return nullptr;
        }

        return backBufferView;
    }


    template <class VersionedContext>
    Microsoft::WRL::ComPtr<ID3D11Texture2D> BaseD3DContext<VersionedContext>::GetSwapChainBackBufferTexture(
        Microsoft::WRL::ComPtr<typename BaseD3DContext<VersionedContext>::SwapChainType>& swapChain)
    {
        if (swapChain == nullptr)
        {
            return nullptr;
        }

        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferTex = nullptr;
        HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>( backBufferTex.GetAddressOf() ));
        if (FAILED(hr))
        {
            _com_error err(hr);
            MOE_ERROR(moe::ChanGraphics, "D3D11 Context failed to retrieve back buffer into a 2D Texture. Last COM error: %ls", err.ErrorMessage());
            return nullptr;
        }

        return backBufferTex;
    }


    template <class VersionedContext>
    HRESULT   BaseD3DContext<VersionedContext>::CreateDepthStencilBuffer(
        Microsoft::WRL::ComPtr<typename BaseD3DContext<VersionedContext>::DeviceType>& device,
        Microsoft::WRL::ComPtr<typename BaseD3DContext<VersionedContext>::SwapChainType>& swapChain,
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depthStencilView,
        Microsoft::WRL::ComPtr<ID3D11Texture2D>& depthStencilBuffer,
        DXGI_FORMAT depthStencilFormat)
    {
        // Create a depth-stencil buffer that's based on the back buffer description.
        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferTex = GetSwapChainBackBufferTexture(swapChain);
        if (backBufferTex == nullptr)
        {
            MOE_ERROR(moe::ChanGraphics, "D3D11 Context needs the back buffer to create the depth-stencil buffer.");
            return S_FALSE;
        }

        D3D11_TEXTURE2D_DESC backBufDesc;
        backBufferTex->GetDesc(&backBufDesc);

        // Modify only if necessary because much things are to be left unchanged (such as width/height or MSAA settings).
        D3D11_TEXTURE2D_DESC depthStencilDesc = backBufDesc;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = depthStencilFormat;
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags = 0;

        // And create the depth-stencil buffer
        HRESULT hr = device->CreateTexture2D(&depthStencilDesc, 0, depthStencilBuffer.GetAddressOf());
        if (FAILED(hr))
        {
            _com_error err(hr);
            MOE_ERROR(moe::ChanGraphics, "D3D11 Context failed to create a texture 2D for the depth stencil buffer! Last COM error: %ls", err.ErrorMessage());
            return hr;
        }

        hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), 0, depthStencilView.GetAddressOf());
        if (FAILED(hr))
        {
            _com_error err(hr);
            MOE_ERROR(moe::ChanGraphics, "D3D11 Context failed to create a render target view for the depth stencil buffer! Last COM error: %ls", err.ErrorMessage());
            return hr;
        }

        return hr;
    }


    template <class VersionedContext>
    void    BaseD3DContext<VersionedContext>::SetFullViewport()
    {
        if (!MOE_ASSERT(m_immediateContext != nullptr))
        {
            MOE_ERROR(moe::ChanGraphics, "D3D Context unable to set viewport: device context is null");
            return;
        }

        // Initializing a full-window viewport
        // Retrieve the back buffer into a Texture 2D to retrieve the dimensions
        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferTex = GetSwapChainBackBufferTexture(m_swapChain);
        if (MOE_ASSERT(backBufferTex != nullptr))
        {
            return;
        }

        // We share a lot of properties with the actual back buffer so we can base ourselves on its description for multisampling
        // Do it now because we're releasing the texture before that
        D3D11_TEXTURE2D_DESC desc = { 0 };
        backBufferTex->GetDesc(&desc);

        // Initialize the viewport with "standard" values
        D3D11_VIEWPORT vp;
        vp.Width = (FLOAT)desc.Width;
        vp.Height = (FLOAT)desc.Height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;

        m_immediateContext->RSSetViewports(1, &vp);
    }


    template <class VersionedContext>
    void BaseD3DContext<VersionedContext>::DescribeAdapter(Microsoft::WRL::ComPtr<typename BaseD3DContext<VersionedContext>::AdapterType>& adapter)
    {
        DXGI_ADAPTER_DESC desc;

        adapter->GetDesc(&desc);
        MOE_INFO(moe::ChanGraphics, "Adapter %ls: Vendor ID %lu, Device ID %lu, Revision %lu, dedicated video memory: %lu MB.",
            desc.Description, desc.VendorId, desc.DeviceId, desc.Revision, desc.DedicatedVideoMemory / 1024 / 1024);
    }



    // Given an adapter, returns the description of a DXGI Mode that respects both the requested format and viewport size.
    // This is necessary e.g. to retrieve the refresh rate for V-Sync.
    template <class VersionedContext>
    DXGI_MODE_DESC BaseD3DContext<VersionedContext>::GetViewportFittingAdapterMode(
        const Microsoft::WRL::ComPtr<IDXGIAdapter>& adapter,
        DXGI_FORMAT requestedFormat, std::uint32_t viewportWidth, std::uint32_t viewportHeight)
    {
        HRESULT hr;

        // Now get the default output of the default adapter
        // Note: Use the 11.2 IDXGIOutput1 interface to be able to query stereo display modes to perhaps manage VR later.
        Microsoft::WRL::ComPtr<IDXGIOutput> defaultOutput;
        adapter->EnumOutputs(0, defaultOutput.GetAddressOf());
        Microsoft::WRL::ComPtr<IDXGIOutput1> output1;
        hr = defaultOutput.As<IDXGIOutput1>(&output1);
        MOE_DEBUG_ASSERT(SUCCEEDED(hr));

        // Get the number of modes that fit our wanted display format for this output
        // Uncomment DXGI_ENUM_MODES_STEREO to enumerate stereoscopic rendering modes
        UINT numModes;
        UINT flags = DXGI_ENUM_MODES_INTERLACED /*| DXGI_ENUM_MODES_STEREO*/;
        hr = output1->GetDisplayModeList(requestedFormat, flags, &numModes, nullptr);
        MOE_DEBUG_ASSERT(SUCCEEDED(hr));

        std::vector<DXGI_MODE_DESC> modeDescs(numModes);
        hr = output1->GetDisplayModeList(requestedFormat, flags, &numModes, modeDescs.data());
        MOE_DEBUG_ASSERT(SUCCEEDED(hr));

        //// Now go through all the display modes and find the one that matches the screen width and height.
        //// Store the refresh rate values of the first display mode numerator and denominator of the refresh rate for that monitor.
        for (const DXGI_MODE_DESC& desc : modeDescs)
        {
            if (desc.Width == viewportWidth && desc.Height == viewportHeight)
            {
                MOE_INFO(moe::ChanGraphics, "Found refresh rate of selected adapter (N,D): %lu,%lu", desc.RefreshRate.Numerator, desc.RefreshRate.Denominator);
                return desc;
            }
        }

        MOE_WARNING(moe::ChanGraphics, "No matching display mode found for selected adapter, setting default refresh rate values");
        return DXGI_MODE_DESC();
    }


    template <class VersionedContext>
    DXGI_SAMPLE_DESC BaseD3DContext<VersionedContext>::GetDeviceBestMultisamplingQuality(Microsoft::WRL::ComPtr<DeviceType>& device, DXGI_FORMAT desiredFormat, UINT desiredSampleCount)
    {
        if (!MOE_ASSERT(device != nullptr))
        {
            return DXGI_SAMPLE_DESC{ 1, 0 };
        }

        DXGI_SAMPLE_DESC desc;
        desc.Count = desiredSampleCount;

        device->CheckMultisampleQualityLevels(desiredFormat, desc.Count, &desc.Quality);
        if (desc.Quality == 0)
        {
            MOE_WARNING(moe::ChanGraphics, "%lux MSAA sampling is unavailable, fallbacking on 4x MSAA.", desiredSampleCount);
            desc.Count = 4;
            device->CheckMultisampleQualityLevels(desiredFormat, desc.Count, &desc.Quality);
            MOE_DEBUG_ASSERT(desc.Quality > 0);
        }

        return desc;
    }

}