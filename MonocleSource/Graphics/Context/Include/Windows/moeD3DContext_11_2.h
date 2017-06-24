#ifndef MOE_D3DCONTEXT_11_2_H_
#define MOE_D3DCONTEXT_11_2_H_

#include "moeD3D11Context.h"
#include <d3d11_2.h>

// The Direct3D Context class for 11.2 version.
// Its particularities:
// - it doesn't manage MSAA like the older versions: the backbuffer is no longer MSAA'd,
// and if you really want MSAA, you need a multisampled offscreen render target to render into.
// - VSync isn't configurable : it is supposed to be enabled
// - because of its use of newer flip swap effects, back buffer formats are drastically restricted.
namespace moe
{
    class D3DContext_11_2;

    template <>
    struct D3DContextTraits<D3DContext_11_2>
    {
        using Device = ID3D11Device2;
        using DeviceContext = ID3D11DeviceContext2;
        using SwapChain = IDXGISwapChain2;
        using Adapter = IDXGIAdapter2;
        using Factory = IDXGIFactory2;

        static const D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_1; // There is no 11_2 feature level per se; 11.2 features are bundled in the old ones
    };


    class D3DContext_11_2 : public BaseD3DContext<D3DContext_11_2>
    {
    public:
        D3DContext_11_2(const ContextDescriptor& contextDesc, HWND winHandle);

        static void DescribeAdapter(Microsoft::WRL::ComPtr<IDXGIAdapter2>& adapter);

        static Microsoft::WRL::ComPtr<IDXGISwapChain2>  CreateFullWindowSwapChain(const ContextDescriptor& contextDesc,
            Microsoft::WRL::ComPtr<IDXGIFactory2>& factory,
            Microsoft::WRL::ComPtr<IDXGIAdapter2>&, // unused
            Microsoft::WRL::ComPtr<ID3D11Device2>& device,
            HWND winHandle);


    private:
        // Make the Base class a friend so it's able to use these static functions
        friend BaseD3DContext<D3DContext_11_2>;
        HRESULT CreateMultisampledOffscreenTarget(UINT samplesCount);

        // In DX 11.2, multisampling needs to be done on a separate target.
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_offscreenMSAATarget;
    };
}


#endif // MOE_D3DCONTEXT_11_2_H_
