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

#include "RenderTarget.h"
#include "Device.h"
#include <windows.h>
#include <cassert>

RenderTarget::RenderTarget(DX11Device& device)
    : DX11Resource(device)
{ }

GeometryBuffer::GeometryBuffer(DX11Device& device)
    : RenderTarget(device)
{
    ID3D11Device& deviceHandle = m_Device.GetHandle();
    IDXGISwapChain1& swapChain = m_Device.GetSwapChain();

    {
        D3D11_RENDER_TARGET_BLEND_DESC targetBlendDesc{ };
        targetBlendDesc.BlendEnable = FALSE;
        targetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        D3D11_BLEND_DESC blendDesc{ };
        blendDesc.RenderTarget[0] = targetBlendDesc;
        blendDesc.RenderTarget[1] = targetBlendDesc;
        blendDesc.RenderTarget[2] = targetBlendDesc;
        blendDesc.RenderTarget[3] = targetBlendDesc;

        HRESULT hr = deviceHandle.CreateBlendState(&blendDesc, &m_BlendState);
        assert(SUCCEEDED(hr));
    }

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

    m_DiffuseTexture.reset(new GeometryTexture(m_Device, 0, outputDesc.Width, outputDesc.Height));
    m_SpecularTexture.reset(new GeometryTexture(m_Device, 1, outputDesc.Width, outputDesc.Height));
    m_PositionTexture.reset(new GeometryTexture(m_Device, 2, outputDesc.Width, outputDesc.Height));
    m_NormalTexture.reset(new GeometryTexture(m_Device, 3, outputDesc.Width, outputDesc.Height));

    m_DepthStencilTexture.reset(new DepthStencilTexture(m_Device, 4, outputDesc.Width, outputDesc.Height));
}

void GeometryBuffer::Enable()
{
    ID3D11DeviceContext& deviceContext = m_Device.GetContext();

    {
        ID3D11RenderTargetView* renderViews[] =
        {
            &m_DiffuseTexture->GetRenderView(),
            &m_SpecularTexture->GetRenderView(),
            &m_PositionTexture->GetRenderView(),
            &m_NormalTexture->GetRenderView()
        };

        deviceContext.OMSetRenderTargets(4, renderViews, &m_DepthStencilTexture->GetDepthStencilView()); // Output Merger
        deviceContext.OMSetBlendState(m_BlendState.Get(), nullptr, 0xffffffff); // Output Merger
        deviceContext.RSSetState(m_RasterizerState.Get()); // Rasterizer State

        FLOAT zero[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        deviceContext.ClearRenderTargetView(&m_DiffuseTexture->GetRenderView(), zero);
        deviceContext.ClearRenderTargetView(&m_SpecularTexture->GetRenderView(), zero);
        deviceContext.ClearRenderTargetView(&m_PositionTexture->GetRenderView(), zero);
        deviceContext.ClearRenderTargetView(&m_NormalTexture->GetRenderView(), zero);

        deviceContext.ClearDepthStencilView(&m_DepthStencilTexture->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
}

void GeometryBuffer::Disable()
{
    ID3D11DeviceContext& deviceContext = m_Device.GetContext();

    {
        // Unbound render targets in case target textures to be used in shaders later
        ID3D11RenderTargetView* renderViews[] = { nullptr, nullptr, nullptr, nullptr };
        deviceContext.OMSetRenderTargets(4, renderViews, nullptr); // Output Merger
        deviceContext.OMSetBlendState(nullptr, nullptr, 0xffffffff); // Output Merger

        // Reset rasterizer state to default just in case
        deviceContext.RSSetState(nullptr); // Rasterizer State
    }
}

GeometryTexture& GeometryBuffer::GetDiffuseTexture() const
{
    return *m_DiffuseTexture.get();
}

GeometryTexture& GeometryBuffer::GetSpecularTexture() const
{
    return *m_SpecularTexture.get();
}

GeometryTexture& GeometryBuffer::GetPositionTexture() const
{
    return *m_PositionTexture.get();
}

GeometryTexture& GeometryBuffer::GetNormalTexture() const
{
    return *m_NormalTexture.get();
}

DepthStencilTexture& GeometryBuffer::GetDepthStencilTexture() const
{
    return *m_DepthStencilTexture.get();
}

FrameBuffer::FrameBuffer(DX11Device& device)
    : RenderTarget(device)
{
    ID3D11Device& deviceHandle = m_Device.GetHandle();
    IDXGISwapChain1& swapChain = m_Device.GetSwapChain();

    {
        D3D11_RENDER_TARGET_BLEND_DESC targetBlendDesc{ };
        targetBlendDesc.BlendEnable = TRUE;
        targetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
        targetBlendDesc.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        targetBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        targetBlendDesc.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        targetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
        targetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        targetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        D3D11_BLEND_DESC blendDesc{ };
        blendDesc.RenderTarget[0] = targetBlendDesc;

        HRESULT hr = deviceHandle.CreateBlendState(&blendDesc, &m_BlendState);
        assert(SUCCEEDED(hr));
    }

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

void FrameBuffer::Enable()
{
    ID3D11DeviceContext& deviceContext = m_Device.GetContext();

    {
        ID3D11RenderTargetView* renderViews[] = { m_FrameRenderView.Get() };
        deviceContext.OMSetRenderTargets(1, renderViews, nullptr); // Output Merger
        deviceContext.OMSetBlendState(m_BlendState.Get(), nullptr, 0xffffffff); // Output Merger
        deviceContext.RSSetState(m_RasterizerState.Get()); // Rasterizer State

        FLOAT black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        deviceContext.ClearRenderTargetView(m_FrameRenderView.Get(), black);
    }
}

void FrameBuffer::Disable()
{
    ID3D11DeviceContext& deviceContext = m_Device.GetContext();

    {
        // Unbound render targets in case target textures to be used in shaders later
        ID3D11RenderTargetView* renderViews[] = { nullptr };
        deviceContext.OMSetRenderTargets(1, renderViews, nullptr); // Output Merger
        deviceContext.OMSetBlendState(nullptr, nullptr, 0xffffffff); // Output Merger

        // Reset rasterizer state to default just in case
        deviceContext.RSSetState(nullptr); // Rasterizer State
    }
}
