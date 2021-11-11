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

void Buffer::Enable(DX11Device& device)
{ }

void Buffer::Disable(DX11Device& device)
{
    ID3D11DeviceContext& deviceContext = device.GetContext();

    {
        // Unbound render targets in case target textures to be used in shaders later
        ID3D11RenderTargetView* renderViews[] = { nullptr };
        deviceContext.OMSetRenderTargets(1, renderViews, nullptr); // Output Merger

        // Reset rasterizer state to default just in case
        deviceContext.RSSetState(nullptr); // Rasterizer State
    }
}

GeometryBuffer::GeometryBuffer(DX11Device& device)
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

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pFrameTexture;

    HRESULT hr = swapChain.GetBuffer(0, IID_PPV_ARGS(&pFrameTexture));
    assert(SUCCEEDED(hr));

    D3D11_TEXTURE2D_DESC outputDesc{ };
    pFrameTexture->GetDesc(&outputDesc);

    {
        D3D11_TEXTURE2D_DESC geometryDesc{ };
        geometryDesc.Width = outputDesc.Width;
        geometryDesc.Height = outputDesc.Height;
        geometryDesc.MipLevels = 1;
        geometryDesc.ArraySize = 1;
        geometryDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        geometryDesc.Usage = D3D11_USAGE_DEFAULT;
        geometryDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        geometryDesc.SampleDesc.Count = 1;
        geometryDesc.SampleDesc.Quality = 0;

        hr = deviceHandle.CreateTexture2D(&geometryDesc, 0, &m_GeometryTexture);
        assert(SUCCEEDED(hr));

        D3D11_SAMPLER_DESC samplerDesc{ };
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
        samplerDesc.BorderColor[1] = 1.0f; // Green
        samplerDesc.BorderColor[3] = 1.0f; // Alpha

        hr = deviceHandle.CreateSamplerState(&samplerDesc, &m_GeometrySampler);
        assert(SUCCEEDED(hr));

        hr = deviceHandle.CreateRenderTargetView(m_GeometryTexture.Get(), nullptr, &m_GeometryRenderView);
        assert(SUCCEEDED(hr));

        hr = deviceHandle.CreateShaderResourceView(m_GeometryTexture.Get(), nullptr, &m_GeometryShaderView);
        assert(SUCCEEDED(hr));
    }

    {
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

        hr = deviceHandle.CreateTexture2D(&depthStencilDesc, 0, &m_DepthStencilTexture);
        assert(SUCCEEDED(hr));

        hr = deviceHandle.CreateDepthStencilView(m_DepthStencilTexture.Get(), nullptr, &m_DepthStencilView);
        assert(SUCCEEDED(hr));
    }
}

void GeometryBuffer::Enable(DX11Device& device)
{
    ID3D11DeviceContext& deviceContext = device.GetContext();

    {
        deviceContext.OMSetRenderTargets(1, m_GeometryRenderView.GetAddressOf(), m_DepthStencilView.Get()); // Output Merger
        deviceContext.RSSetState(m_RasterizerState.Get()); // Rasterizer State

        FLOAT black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        deviceContext.ClearRenderTargetView(m_GeometryRenderView.Get(), black);
        deviceContext.ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
}

void GeometryBuffer::EnableGeometry(DX11Device& device) const
{
    ID3D11DeviceContext& deviceContext = device.GetContext();

    {
        deviceContext.PSSetShaderResources(0, 1, m_GeometryShaderView.GetAddressOf());
        deviceContext.PSSetSamplers(0, 1, m_GeometrySampler.GetAddressOf());
    }
}

void GeometryBuffer::DisableGeometry(DX11Device& device) const
{
    ID3D11DeviceContext& deviceContext = device.GetContext();

    {
        // Unbound shader resources in case they are used as render targets later
        ID3D11ShaderResourceView* resourceViews[] = { nullptr };
        deviceContext.PSSetShaderResources(0, 1, resourceViews);

        // Unbound sampler states just in case
        ID3D11SamplerState* samplerStates[] = { nullptr };
        deviceContext.PSSetSamplers(0, 1, samplerStates);
    }
}

FrameBuffer::FrameBuffer(DX11Device& device)
{
    ID3D11Device& deviceHandle = device.GetHandle();
    IDXGISwapChain1& swapChain = device.GetSwapChain();

    {
        D3D11_RASTERIZER_DESC rasterizerDesc{ };
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_BACK;
        rasterizerDesc.DepthClipEnable = FALSE;

        HRESULT hr = deviceHandle.CreateRasterizerState(&rasterizerDesc, &m_RasterizerState);
        assert(SUCCEEDED(hr));
    }

    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> pFrameTexture;

        HRESULT hr = swapChain.GetBuffer(0, IID_PPV_ARGS(&pFrameTexture));
        assert(SUCCEEDED(hr));

        hr = deviceHandle.CreateRenderTargetView(pFrameTexture.Get(), nullptr, &m_FrameRenderView);
        assert(SUCCEEDED(hr));
    }
}

void FrameBuffer::Enable(DX11Device& device)
{
    ID3D11DeviceContext& deviceContext = device.GetContext();

    {
        deviceContext.OMSetRenderTargets(1, m_FrameRenderView.GetAddressOf(), nullptr); // Output Merger
        deviceContext.RSSetState(m_RasterizerState.Get()); // Rasterizer State

        FLOAT black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        deviceContext.ClearRenderTargetView(m_FrameRenderView.Get(), black);
    }
}
