#ifndef MOE_D3DCONTEXT_11_1_H_
#define MOE_D3DCONTEXT_11_1_H_

#include "moeD3D11Context.h"
#include <d3d11_1.h>


// The Direct3D Context class for 11.1 version.
// It basically is a classic D3D11 context, using 11.1 interfaces if it can be useful at some point.
namespace moe
{
    class D3DContext_11_1;

    template <>
    struct D3DContextTraits<D3DContext_11_1>
    {
        using Device = ID3D11Device1;
        using DeviceContext = ID3D11DeviceContext1;
        using SwapChain = IDXGISwapChain1;
        using Adapter = IDXGIAdapter1;
        using Factory = IDXGIFactory1;

        static const D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_1;
    };


    class D3DContext_11_1 : public BaseD3DContext<D3DContext_11_1>
    {
    public:
        D3DContext_11_1(const ContextDescriptor& contextDesc, HWND winHandle);

        static Microsoft::WRL::ComPtr<IDXGISwapChain1>  CreateFullWindowSwapChain(const ContextDescriptor& contextDesc,
            Microsoft::WRL::ComPtr<IDXGIFactory1>& factory,
            Microsoft::WRL::ComPtr<IDXGIAdapter1>& adapter,
            Microsoft::WRL::ComPtr<ID3D11Device1>& device,
            HWND winHandle);

    };
}


#endif // MOE_D3DCONTEXT_11_1_H_