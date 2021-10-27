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

DX11Device::DX11Device(const Window& window)
{
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

        IDXGIDevice2* pDXGIDevice = nullptr;
        IDXGIAdapter* pDXGIAdapter = nullptr;
        IDXGIFactory2* pIDXGIFactory = nullptr;

        HRESULT hr = m_D3D11Device->QueryInterface(__uuidof(IDXGIDevice2), reinterpret_cast<void**>(&pDXGIDevice));
        assert(SUCCEEDED(hr));

        hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDXGIAdapter));
        assert(SUCCEEDED(hr));

        hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&pIDXGIFactory));
        assert(SUCCEEDED(hr));

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc{ };
        swapChainDesc.Width = window.GetWidth();
        swapChainDesc.Height = window.GetHeight();
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2; // For DXGI_SWAP_EFFECT_FLIP_DISCARD
        swapChainDesc.Scaling = DXGI_SCALING_NONE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;

        hr = pIDXGIFactory->CreateSwapChainForHwnd(m_D3D11Device, window.GetHandle(), &swapChainDesc, nullptr, nullptr, &m_D3D11SwapChain);
        if (FAILED(hr))
            throw std::runtime_error("Failed to create swap chain");

        hr = pIDXGIFactory->MakeWindowAssociation(window.GetHandle(), DXGI_MWA_NO_WINDOW_CHANGES);
        assert(SUCCEEDED(hr));

        pIDXGIFactory->Release();
        pDXGIAdapter->Release();
        pDXGIDevice->Release();
    }

    {
        ID3D11Texture2D* pBackBuffer = nullptr;

        HRESULT hr = m_D3D11SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
        assert(SUCCEEDED(hr));

        hr = m_D3D11Device->CreateRenderTargetView(pBackBuffer, nullptr, &m_RenderTargetView);
        assert(SUCCEEDED(hr));

        pBackBuffer->Release();
    }

    {
        D3D11_TEXTURE2D_DESC depthStencilDesc{ };
        depthStencilDesc.Width = window.GetWidth();
        depthStencilDesc.Height = window.GetHeight();
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;

        HRESULT hr = m_D3D11Device->CreateTexture2D(&depthStencilDesc, 0, &m_DepthStencilBuffer);
        assert(SUCCEEDED(hr));

        hr = m_D3D11Device->CreateDepthStencilView(m_DepthStencilBuffer, nullptr, &m_DepthStencilView);
        assert(SUCCEEDED(hr));
    }

    {
        D3D11_VIEWPORT viewport{ };
        viewport.Width = static_cast<FLOAT>(window.GetWidth());
        viewport.Height = static_cast<FLOAT>(window.GetHeight());
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        m_D3D11DeviceContext->RSSetViewports(1, &viewport); // Rasterizer Stage
        m_D3D11DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView); // Output Merger
    }
}

DX11Device::~DX11Device()
{
    if (m_DepthStencilView)
        m_DepthStencilView->Release();

    if (m_DepthStencilBuffer)
        m_DepthStencilBuffer->Release();

    if (m_RenderTargetView)
        m_RenderTargetView->Release();

    if (m_D3D11SwapChain)
        m_D3D11SwapChain->Release();

    if (m_D3D11DeviceContext)
        m_D3D11DeviceContext->Release();

    if (m_D3D11Device)
        m_D3D11Device->Release();
}

void DX11Device::FrameBegin(Context& context)
{
    FLOAT red[] = { 1.0f, 0.0f, 0.0f, 0.0f };
    m_D3D11DeviceContext->ClearRenderTargetView(m_RenderTargetView, red);
    m_D3D11DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DX11Device::FrameEnd(Context& context)
{
    DXGI_PRESENT_PARAMETERS params{ };
    m_D3D11SwapChain->Present1(0, 0, &params);
}
