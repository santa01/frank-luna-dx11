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

#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class DX11Device;

class Buffer
{
public:
    virtual ~Buffer() = default;
    virtual void Enable(DX11Device& device) = 0;
    virtual void Disable(DX11Device& device) = 0;
};

class GeometryBuffer final : public Buffer
{
public:
    GeometryBuffer(DX11Device& device);
    void Enable(DX11Device& device) override;
    void Disable(DX11Device& device) override;

    void EnableGeometry(DX11Device& device) const;
    void DisableGeometry(DX11Device& device) const;

private:
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterizerState;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_GeometrySampler;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_ColorTexture;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_ColorRenderView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ColorShaderView;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_PositionTexture;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_PositionRenderView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_PositionShaderView;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_DepthStencilTexture;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
};

class FrameBuffer final : public Buffer
{
public:
    FrameBuffer(DX11Device& device);
    void Enable(DX11Device& device) override;
    void Disable(DX11Device& device) override;

private:
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterizerState;

    // Frame texture is a part of swap chain output buffer
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_FrameRenderView;
};
