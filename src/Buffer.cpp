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
        D3D11_SAMPLER_DESC samplerDesc{ };
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
        samplerDesc.BorderColor[1] = 1.0f; // Green
        samplerDesc.BorderColor[3] = 1.0f; // Alpha

        hr = deviceHandle.CreateSamplerState(&samplerDesc, &m_GeometrySampler);
        assert(SUCCEEDED(hr));
    }

    {
        D3D11_TEXTURE2D_DESC colorDesc{ };
        colorDesc.Width = outputDesc.Width;
        colorDesc.Height = outputDesc.Height;
        colorDesc.MipLevels = 1;
        colorDesc.ArraySize = 1;
        colorDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        colorDesc.Usage = D3D11_USAGE_DEFAULT;
        colorDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        colorDesc.SampleDesc.Count = 1;
        colorDesc.SampleDesc.Quality = 0;

        hr = deviceHandle.CreateTexture2D(&colorDesc, 0, &m_ColorTexture);
        assert(SUCCEEDED(hr));

        hr = deviceHandle.CreateRenderTargetView(m_ColorTexture.Get(), nullptr, &m_ColorRenderView);
        assert(SUCCEEDED(hr));

        hr = deviceHandle.CreateShaderResourceView(m_ColorTexture.Get(), nullptr, &m_ColorShaderView);
        assert(SUCCEEDED(hr));

        D3D11_TEXTURE2D_DESC positionDesc{ };
        positionDesc.Width = outputDesc.Width;
        positionDesc.Height = outputDesc.Height;
        positionDesc.MipLevels = 1;
        positionDesc.ArraySize = 1;
        positionDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        positionDesc.Usage = D3D11_USAGE_DEFAULT;
        positionDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        positionDesc.SampleDesc.Count = 1;
        positionDesc.SampleDesc.Quality = 0;

        hr = deviceHandle.CreateTexture2D(&positionDesc, 0, &m_PositionTexture);
        assert(SUCCEEDED(hr));

        hr = deviceHandle.CreateRenderTargetView(m_PositionTexture.Get(), nullptr, &m_PositionRenderView);
        assert(SUCCEEDED(hr));

        hr = deviceHandle.CreateShaderResourceView(m_PositionTexture.Get(), nullptr, &m_PositionShaderView);
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
        ID3D11RenderTargetView* renderViews[] = { m_ColorRenderView.Get(), m_PositionRenderView.Get() };
        deviceContext.OMSetRenderTargets(2, renderViews, m_DepthStencilView.Get()); // Output Merger
        deviceContext.RSSetState(m_RasterizerState.Get()); // Rasterizer State

        FLOAT zero[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        deviceContext.ClearRenderTargetView(m_ColorRenderView.Get(), zero);
        deviceContext.ClearRenderTargetView(m_PositionRenderView.Get(), zero);

        deviceContext.ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
}

void GeometryBuffer::Disable(DX11Device& device)
{
    ID3D11DeviceContext& deviceContext = device.GetContext();

    {
        // Unbound render targets in case target textures to be used in shaders later
        ID3D11RenderTargetView* renderViews[] = { nullptr, nullptr };
        deviceContext.OMSetRenderTargets(2, renderViews, nullptr); // Output Merger

        // Reset rasterizer state to default just in case
        deviceContext.RSSetState(nullptr); // Rasterizer State
    }
}

void GeometryBuffer::EnableGeometry(DX11Device& device) const
{
    ID3D11DeviceContext& deviceContext = device.GetContext();

    {
        ID3D11ShaderResourceView* resourceViews[] = { m_ColorShaderView.Get(), m_PositionShaderView.Get() };
        deviceContext.PSSetShaderResources(0, 2, resourceViews);
        deviceContext.PSSetSamplers(0, 1, m_GeometrySampler.GetAddressOf());
    }
}

void GeometryBuffer::DisableGeometry(DX11Device& device) const
{
    ID3D11DeviceContext& deviceContext = device.GetContext();

    {
        // Unbound shader resources in case they are used as render targets later
        ID3D11ShaderResourceView* resourceViews[] = { nullptr, nullptr };
        deviceContext.PSSetShaderResources(0, 2, resourceViews);

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

void FrameBuffer::Disable(DX11Device& device)
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
