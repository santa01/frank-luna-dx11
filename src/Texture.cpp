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

#include "Texture.h"
#include "Device.h"
#include <windows.h>
#include <DDSTextureLoader.h>
#include <cassert>

Texture::Texture(DX11Device& device, UINT slot)
    : DX11Resource(device)
    , m_ResourceSlot(slot)
{ }

void Texture::Enable()
{
    ID3D11DeviceContext& deviceContext = m_Device.GetContext();

    {
        ID3D11ShaderResourceView* resourceViews[] = { m_ShaderView.Get() };
        deviceContext.PSSetShaderResources(m_ResourceSlot, 1, resourceViews);
    }
}

void Texture::Disable()
{
    ID3D11DeviceContext& deviceContext = m_Device.GetContext();

    {
        ID3D11ShaderResourceView* resourceViews[] = { nullptr };
        deviceContext.PSSetShaderResources(m_ResourceSlot, 1, resourceViews);
    }
}

ID3D11ShaderResourceView& Texture::GetShaderView() const
{
    return *m_ShaderView.Get();
}

GeometryTexture::GeometryTexture(DX11Device& device, UINT slot, UINT width, UINT height)
    : Texture(device, slot)
{
    ID3D11Device& deviceHandle = m_Device.GetHandle();

    {
        D3D11_TEXTURE2D_DESC textureDesc{ };
        textureDesc.Width = width;
        textureDesc.Height = height;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;

        HRESULT hr = deviceHandle.CreateTexture2D(&textureDesc, 0, &m_Texture);
        assert(SUCCEEDED(hr));

        hr = deviceHandle.CreateShaderResourceView(m_Texture.Get(), nullptr, &m_ShaderView);
        assert(SUCCEEDED(hr));

        hr = deviceHandle.CreateRenderTargetView(m_Texture.Get(), nullptr, &m_RenderView);
        assert(SUCCEEDED(hr));
    }
}

ID3D11RenderTargetView& GeometryTexture::GetRenderView() const
{
    return *m_RenderView.Get();
}

DepthStencilTexture::DepthStencilTexture(DX11Device& device, UINT slot, UINT width, UINT height)
    : Texture(device, slot)
{
    ID3D11Device& deviceHandle = m_Device.GetHandle();

    {
        D3D11_TEXTURE2D_DESC depthStencilDesc{ };
        depthStencilDesc.Width = width;
        depthStencilDesc.Height = height;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;

        HRESULT hr = deviceHandle.CreateTexture2D(&depthStencilDesc, 0, &m_Texture);
        assert(SUCCEEDED(hr));

        hr = deviceHandle.CreateDepthStencilView(m_Texture.Get(), nullptr, &m_DepthStencilView);
        assert(SUCCEEDED(hr));
    }
}

ID3D11DepthStencilView& DepthStencilTexture::GetDepthStencilView() const
{
    return *m_DepthStencilView.Get();
}

ImageTexture::ImageTexture(DX11Device& device, UINT slot, const std::wstring& source)
    : Texture(device, slot)
{
    ID3D11Device& deviceHandle = m_Device.GetHandle();

    {
        HRESULT hr = DirectX::CreateDDSTextureFromFile(&deviceHandle, source.c_str(), nullptr, m_ShaderView.GetAddressOf());
        assert(SUCCEEDED(hr));
    }
}
