#include "Platform/Pipeline.h"
#include "Platform/Window.h"
#include "Misc/Utils.h"
#include "Misc/Log.h"

#include <d3d11.h>
#include <dxgi.h>

#undef near
#undef far

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

ID3D11Debug* debug;
ID3D11Device* device;
IDXGISwapChain* swapchain;
ID3D11DeviceContext* context;

ID3D11RenderTargetView* swapchain_rtv;

D3D_FEATURE_LEVEL feature_level;



void Pipeline::InternalInit() {
    D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_0
    };

    UINT flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT res = S_OK;

    res = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, feature_levels,
                            ARRAYSIZE(feature_levels), D3D11_SDK_VERSION, &device, &feature_level, &context);

    if( FAILED(res) ) 
        Log::Crash("Failed to create DX 11.0 hardware device");
    
    IDXGIDevice *dxgi_device = nullptr;
    res = device->QueryInterface(__uuidof(IDXGIDevice), ToPtr<void**>(&dxgi_device));
    if( FAILED(res) )
        Log::Crash("Failed to query dxgi device");

    IDXGIAdapter *dxgi_adapter = nullptr;
    res = dxgi_device->GetAdapter(&dxgi_adapter);
    if( FAILED(res) )
        Log::Crash("Failed to query dxgi adapter");

    IDXGIFactory *dxgi_factory = nullptr;
    res = dxgi_adapter->GetParent(__uuidof(IDXGIFactory), ToPtr<void**>(&dxgi_factory));
    if( FAILED(res) )
        Log::Crash("Failed to query dxgi factory");


    DXGI_SWAP_CHAIN_DESC swapchain_desc;
    swapchain_desc.BufferCount = 2;
    swapchain_desc.BufferDesc.Width = Window::GetWidth();
    swapchain_desc.BufferDesc.Height = Window::GetHeight();
    swapchain_desc.BufferDesc.RefreshRate.Numerator = 60;
    swapchain_desc.BufferDesc.RefreshRate.Denominator = 1;
    swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapchain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapchain_desc.SampleDesc.Quality = 0;
    swapchain_desc.SampleDesc.Count = 1;
    swapchain_desc.OutputWindow = To<HWND>(Window::GetHandle());
    swapchain_desc.Windowed = Window::IsWindowed();
    swapchain_desc.Flags = 0;

    res = dxgi_factory->CreateSwapChain(device, &swapchain_desc, &swapchain);
    if( FAILED(res) )
        Log::Crash("Failed to create dxgi swapchain");

    dxgi_factory->Release();

    if( flags & D3D11_CREATE_DEVICE_DEBUG )
        device->QueryInterface(__uuidof(ID3D11Debug), ToPtr<void**>(&debug));

    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
    /// Init render targets & initilize render states
    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////

    ID3D11Texture2D *backbuffer_temp;
    res = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), ToPtr<LPVOID*>(&backbuffer_temp));
    if( FAILED(res) )
        Log::Crash("Failed to create query swapchain backbuffer");

    {
        D3D11_RENDER_TARGET_VIEW_DESC desc;
        desc.Format = swapchain_desc.BufferDesc.Format;
        desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipSlice = 0;

        res = device->CreateRenderTargetView(backbuffer_temp, NULL, &swapchain_rtv);
        backbuffer->private_data[0] = nullptr;
        backbuffer->private_data[1] = swapchain_rtv;
        backbuffer->quality = TextureQuality::High;
        backbuffer->is_depth = false;

        backbuffer_temp->Release();
    }

}

void Pipeline::SubmitState() {
    CheckStates();

    RenderState* rs = render_state;

    if( viewport ) {
        D3D11_VIEWPORT vp;
        vp.TopLeftX = rs->viewport.x;
        vp.TopLeftY = rs->viewport.y;
        vp.Width    = rs->viewport.width;
        vp.Height   = rs->viewport.height;
        vp.MinDepth = rs->viewport.min_depth;
        vp.MaxDepth = rs->viewport.max_depth;

        context->RSSetViewports(1, &vp);
    }
    
    if( scissor ) {
        D3D11_RECT sc;
        sc.left   = To<LONG>(rs->scissor.x);
        sc.top    = To<LONG>(rs->scissor.y);
        sc.right  = To<LONG>(rs->scissor.x + rs->scissor.width);
        sc.bottom = To<LONG>(rs->scissor.y + rs->scissor.height);

        context->RSSetScissorRects(1, &sc);
    }
    
    if( raster ) {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = rs->rasterizer.wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
        desc.CullMode = rs->rasterizer.culling;
        desc.ScissorEnable = rs->rasterizer.scissor;
        desc.FrontCounterClockwise = rs->rasterizer.front_counter_clockwise;

        ID3D11RasterizerState* rs;

        device->CreateRasterizerState(&desc, &rs);
        context->RSSetState(rs);
    }

    if( blend ) {
        D3D11_BLEND_DESC desc{};
        desc.RenderTarget[0].BlendEnable    = rs->blend.enable;
        desc.RenderTarget[0].SrcBlend       = rs->blend.src;
        desc.RenderTarget[0].DestBlend      = rs->blend.dest;
        desc.RenderTarget[0].BlendOp        = rs->blend.op;
        desc.RenderTarget[0].SrcBlendAlpha  = rs->blend.src_alpha;
        desc.RenderTarget[0].DestBlendAlpha = rs->blend.dest_alpha;
        desc.RenderTarget[0].BlendOpAlpha   = rs->blend.op_alpha;
        desc.RenderTarget[0].RenderTargetWriteMask = rs->blend.write_mask;

        ID3D11BlendState* bs;
        float factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

        device->CreateBlendState(&desc, &bs);
        context->OMSetBlendState(bs, factor, 0xFFFFFFFF);
    }

    if( depth ) {
        D3D11_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable      = rs->depth.depth_enable;
        desc.StencilEnable    = rs->depth.stencil_enable;
        desc.StencilReadMask  = rs->depth.stencil_read_mask;
        desc.StencilWriteMask = rs->depth.stencil_write_mask;
        desc.DepthFunc        = rs->depth.depth_compare;

        desc.FrontFace.StencilFailOp      = rs->depth.ff_fail_op;
        desc.FrontFace.StencilDepthFailOp = rs->depth.ff_depth_fail_op;
        desc.FrontFace.StencilPassOp      = rs->depth.ff_pass_op;
        desc.FrontFace.StencilFunc        = rs->depth.ff_func;
        
        desc.BackFace.StencilFailOp      = rs->depth.bf_fail_op;
        desc.BackFace.StencilDepthFailOp = rs->depth.bf_depth_fail_op;
        desc.BackFace.StencilPassOp      = rs->depth.bf_pass_op;
        desc.BackFace.StencilFunc        = rs->depth.bf_func;

        ID3D11DepthStencilState* dss;
        device->CreateDepthStencilState(&desc, &dss);
        context->OMSetDepthStencilState(dss, rs->depth.stencil_ref);
    }

    PostSubmitStates();
}

void Pipeline::Present() {
    swapchain->Present(1, 0);
}
