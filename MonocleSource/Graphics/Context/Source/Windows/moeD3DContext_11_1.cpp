#include "Graphics/Context/Include/Windows/moeD3DContext_11_1.h"

namespace moe
{
    D3DContext_11_1::D3DContext_11_1(const ContextDescriptor& contextDesc, HWND winHandle) :
        BaseD3DContext<D3DContext_11_1>(contextDesc, winHandle)
    {
    }


    Microsoft::WRL::ComPtr<IDXGISwapChain1>  D3DContext_11_1::CreateFullWindowSwapChain(const ContextDescriptor& contextDesc,
        Microsoft::WRL::ComPtr<IDXGIFactory1>& factory,
        Microsoft::WRL::ComPtr<IDXGIAdapter1>& adapter,
        Microsoft::WRL::ComPtr<ID3D11Device1>& device,
        HWND winHandle)
    {
        DXGI_SWAP_CHAIN_DESC scd = { 0 };

        DXGI_FORMAT swapChainFormat = DXGI_FORMAT(contextDesc.ColorFormat == ContextDescriptor::DEFAULT_GENERIC_FORMAT ? DEFAULT_COLOR_FORMAT : contextDesc.ColorFormat);

        // Obtain the available MSAA sampling quality level for the wanted samples count.
        scd.SampleDesc = GetDeviceBestMultisamplingQuality(device, swapChainFormat, contextDesc.SamplesCount);

        // Compute the VSync refresh rate if vsync is requested.
        // D3D11 doesn't manage adaptive V-Sync out-of-the-box (unlike OpenGL), it will just activate "normal" v-sync if that's requested.
        if ((int)contextDesc.VSyncState & (int)ContextDescriptor::VSyncFlag::VSF_Enabled)
        {
            scd.BufferDesc = GetViewportFittingAdapterMode(adapter, swapChainFormat, contextDesc.ViewportWidth, contextDesc.ViewportHeight);
        }
        else // No vsync: fill the mode desc ourselves
        {
            scd.BufferDesc.Width = contextDesc.ViewportWidth;
            scd.BufferDesc.Height = contextDesc.ViewportHeight;
            scd.BufferDesc.Format = swapChainFormat;
            scd.BufferDesc.RefreshRate.Numerator = 1;
            scd.BufferDesc.RefreshRate.Denominator = 0;
        }

        scd.BufferCount = contextDesc.BuffersCount;
        // scd.BufferDesc was already filled
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.OutputWindow = winHandle;
        // scd.SampleDesc was already filled, but...
        // The range of valid quality levels is between zero and one less than the return value of CheckMultisampleQualityLevels.
        scd.SampleDesc.Quality -= 1;
        scd.Windowed = TRUE; // TODO: check fullscreen


        Microsoft::WRL::ComPtr<IDXGISwapChain> tmpSwapChain;
        HRESULT hr = factory->CreateSwapChain(device.Get(), &scd, tmpSwapChain.GetAddressOf());
        Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
        if (SUCCEEDED(hr))
        {
            tmpSwapChain.As(&swapChain1);
        }

        if (FAILED(hr))
        {
            _com_error err(hr);
            MOE_ERROR(moe::ChanGraphics, "D3D 11.1 Context failed to convert the swap chain to a DXGI 1.2 one. Last COM error: %ls", err.ErrorMessage());
            return nullptr;
        }

        return swapChain1;
    }
}