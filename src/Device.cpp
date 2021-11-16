/*
 * Copyright (c) 2020 Pavlo Lavrenenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Device.h"
#include "Context.h"
#include "Window.h"
#include <windows.h>
#include <cassert>
#include <stdexcept>

DX11Device::DX11Device(Context& context)
{
    Window& window = context.GetWindow();

    {
#ifndef NDEBUG
        UINT uD3D11Flags = D3D11_CREATE_DEVICE_DEBUG;
#else  // NDEBUG
        UINT uD3D11Flags = 0;
#endif // NDEBUG

        D3D_FEATURE_LEVEL pD3D11FeatureLevels[] = { D3D_FEATURE_LEVEL_11_1 };

        HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, uD3D11Flags, pD3D11FeatureLevels, 1, D3D11_SDK_VERSION, &m_D3D11Device, nullptr, &m_D3D11DeviceContext);
        if (FAILED(hr))
            throw std::runtime_error("Failed to create DX11 device");
    }

    {
        // https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_2/nn-dxgi1_2-idxgifactory2

        Microsoft::WRL::ComPtr<IDXGIDevice2> pDXGIDevice2;
        Microsoft::WRL::ComPtr<IDXGIAdapter> pDXGIAdapter;
        Microsoft::WRL::ComPtr<IDXGIFactory2> pIDXGIFactory2;

        HRESULT hr = m_D3D11Device.As(&pDXGIDevice2);
        assert(SUCCEEDED(hr));

        hr = pDXGIDevice2->GetParent(IID_PPV_ARGS(&pDXGIAdapter));
        assert(SUCCEEDED(hr));

        hr = pDXGIAdapter->GetParent(IID_PPV_ARGS(&pIDXGIFactory2));
        assert(SUCCEEDED(hr));

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc1{ };
        swapChainDesc1.Width = window.GetWidth();
        swapChainDesc1.Height = window.GetHeight();
        swapChainDesc1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc1.BufferCount = 2; // For DXGI_SWAP_EFFECT_FLIP_DISCARD
        swapChainDesc1.Scaling = DXGI_SCALING_NONE;
        swapChainDesc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc1.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        swapChainDesc1.SampleDesc.Count = 1;
        swapChainDesc1.SampleDesc.Quality = 0;

        hr = pIDXGIFactory2->CreateSwapChainForHwnd(m_D3D11Device.Get(), window.GetHandle(), &swapChainDesc1, nullptr, nullptr, &m_D3D11SwapChain1);
        if (FAILED(hr))
            throw std::runtime_error("Failed to create swap chain");

        hr = pIDXGIFactory2->MakeWindowAssociation(window.GetHandle(), DXGI_MWA_NO_WINDOW_CHANGES);
        assert(SUCCEEDED(hr));
    }

    m_GeometryBuffer.reset(new GeometryBuffer(*this));
    m_FrameBuffer.reset(new FrameBuffer(*this));

    m_GeometryShader.reset(new Shader(*this, "Geometry.fx"));
    m_GeometryShader->SetSampler(0, D3D11_FILTER_MIN_MAG_MIP_POINT);

    m_FrameShader.reset(new Shader(*this, "Frame.fx"));
    m_GeometryShader->SetSampler(0, D3D11_FILTER_ANISOTROPIC);
}

ID3D11Device& DX11Device::GetHandle() const
{
    return *m_D3D11Device.Get();
}

ID3D11DeviceContext& DX11Device::GetContext() const
{
    return *m_D3D11DeviceContext.Get();
}

IDXGISwapChain1& DX11Device::GetSwapChain() const
{
    return *m_D3D11SwapChain1.Get();
}

Shader& DX11Device::GetGeometryShader() const
{
    return *m_GeometryShader.get();
}

Shader& DX11Device::GetFrameShader() const
{
    return *m_FrameShader.get();
}

void DX11Device::GeometryBegin(Context& context)
{
    Window& window = context.GetWindow();

    {
        D3D11_VIEWPORT viewport{ };
        viewport.Width = static_cast<FLOAT>(window.GetWidth());
        viewport.Height = static_cast<FLOAT>(window.GetHeight());
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        m_D3D11DeviceContext->RSSetViewports(1, &viewport); // Rasterizer Stage
    }

    m_GeometryShader->Enable();
    m_GeometryBuffer->Enable();
}

void DX11Device::GeometryEnd(Context& context)
{
    m_GeometryBuffer->Disable();
}

void DX11Device::FrameBegin(Context& context)
{
    m_GeometryBuffer->GetDiffuseTexture().Enable();
    m_GeometryBuffer->GetSpecularTexture().Enable();
    m_GeometryBuffer->GetPositionTexture().Enable();
    m_GeometryBuffer->GetNormalTexture().Enable();

    m_FrameShader->Enable();
    m_FrameBuffer->Enable();
}

void DX11Device::FrameEnd(Context& context)
{
    m_GeometryBuffer->GetDiffuseTexture().Disable();
    m_GeometryBuffer->GetSpecularTexture().Disable();
    m_GeometryBuffer->GetPositionTexture().Disable();
    m_GeometryBuffer->GetNormalTexture().Disable();

    m_FrameBuffer->Disable();

    DXGI_PRESENT_PARAMETERS params{ };
    m_D3D11SwapChain1->Present1(0, 0, &params);
}
