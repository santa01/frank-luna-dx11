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

#include "Buffer.h"
#include "Device.h"
#include <windows.h>
#include <cassert>

FrameBuffer::FrameBuffer(DX11Device& device)
{
    ID3D11Device& deviceHandle = device.GetHandle();
    IDXGISwapChain1& swapChain = device.GetSwapChain();

    {
        D3D11_RASTERIZER_DESC rasterizerDesc{ };
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_BACK;
        rasterizerDesc.DepthClipEnable = TRUE;

        HRESULT hr = deviceHandle.CreateRasterizerState(&rasterizerDesc, &m_RasterizerState);
        assert(SUCCEEDED(hr));
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pOutputTexture;

    {
        HRESULT hr = swapChain.GetBuffer(0, IID_PPV_ARGS(&pOutputTexture));
        assert(SUCCEEDED(hr));

        hr = deviceHandle.CreateRenderTargetView(pOutputTexture.Get(), nullptr, &m_RenderTargetView);
        assert(SUCCEEDED(hr));
    }

    {
        D3D11_TEXTURE2D_DESC outputDesc{ };
        pOutputTexture->GetDesc(&outputDesc);

        D3D11_TEXTURE2D_DESC depthStencilDesc{ };
        depthStencilDesc.Width = outputDesc.Width;
        depthStencilDesc.Height = outputDesc.Height;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;

        HRESULT hr = deviceHandle.CreateTexture2D(&depthStencilDesc, 0, &m_DepthStencilTexture);
        assert(SUCCEEDED(hr));

        hr = deviceHandle.CreateDepthStencilView(m_DepthStencilTexture.Get(), nullptr, &m_DepthStencilView);
        assert(SUCCEEDED(hr));
    }
}

void FrameBuffer::Enable(DX11Device& device)
{
    ID3D11DeviceContext& deviceContext = device.GetContext();

    {
        deviceContext.RSSetState(m_RasterizerState.Get()); // Rasterizer State
        deviceContext.OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get()); // Output Merger

        FLOAT black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        deviceContext.ClearRenderTargetView(m_RenderTargetView.Get(), black);
        deviceContext.ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
}
