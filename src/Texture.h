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
#include <string>

class DX11Device;

class Texture
{
public:
    virtual ~Texture() = default;
    virtual void Enable(DX11Device& device) = 0;
    virtual void Disable(DX11Device& device) = 0;
};

class GeometryTexture final : public Texture
{
public:
    GeometryTexture(DX11Device& device, UINT slot, UINT width, UINT height);
    void Enable(DX11Device& device) override;
    void Disable(DX11Device& device) override;

    ID3D11RenderTargetView* GetRenderView() const;

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShaderView;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderView;

    UINT m_Slot{ 0 };
};

class ImageTexture final : public Texture
{
public:
    ImageTexture(DX11Device& device, UINT anisotropy, const std::wstring& source);
    void Enable(DX11Device& device) override;
    void Disable(DX11Device& device) override;

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShaderView;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_Sampler;
};
