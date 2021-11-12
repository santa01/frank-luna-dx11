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

GeometryTexture::GeometryTexture(DX11Device& device, UINT slot, UINT width, UINT height)
    : m_Slot(slot)
{
    ID3D11Device& deviceHandle = device.GetHandle();

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

void GeometryTexture::Enable(DX11Device& device)
{
    ID3D11DeviceContext& deviceContext = device.GetContext();
    deviceContext.PSSetShaderResources(m_Slot, 1, m_ShaderView.GetAddressOf());
}

void GeometryTexture::Disable(DX11Device& device)
{
    ID3D11DeviceContext& deviceContext = device.GetContext();

    {
        ID3D11ShaderResourceView* resourceViews[] = { nullptr };
        deviceContext.PSSetShaderResources(m_Slot, 1, resourceViews);
    }
}

ID3D11RenderTargetView* GeometryTexture::GetRenderView() const
{
    return m_RenderView.Get();
}

ImageTexture::ImageTexture(DX11Device& device, UINT anisotropy, const std::wstring& source)
{
    ID3D11Device& deviceHandle = device.GetHandle();

    {
        HRESULT hr = DirectX::CreateDDSTextureFromFile(&deviceHandle, source.c_str(), nullptr, m_ShaderView.GetAddressOf());
        assert(SUCCEEDED(hr));
    }

    {
        D3D11_SAMPLER_DESC samplerDesc{ };
        samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        samplerDesc.MaxAnisotropy = anisotropy;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
        samplerDesc.BorderColor[1] = 1.0f; // Green
        samplerDesc.BorderColor[3] = 1.0f; // Alpha

        HRESULT hr = deviceHandle.CreateSamplerState(&samplerDesc, &m_Sampler);
        assert(SUCCEEDED(hr));
    }
}

void ImageTexture::Enable(DX11Device& device)
{
    ID3D11DeviceContext& deviceContext = device.GetContext();

    {
        deviceContext.PSSetShaderResources(0, 1, m_ShaderView.GetAddressOf());
        deviceContext.PSSetSamplers(0, 1, m_Sampler.GetAddressOf());
    }
}

void ImageTexture::Disable(DX11Device& device)
{
    ID3D11DeviceContext& deviceContext = device.GetContext();

    {
        ID3D11ShaderResourceView* resourceViews[] = { nullptr };
        deviceContext.PSSetShaderResources(0, 1, resourceViews);

        ID3D11SamplerState* samplerStates[] = { nullptr };
        deviceContext.PSSetSamplers(0, 1, samplerStates);
    }
}
