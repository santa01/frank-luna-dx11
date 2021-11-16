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

#include "Resource.h"
#include "Texture.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

class DX11Device;

class RenderTarget : public DX11Resource
{
public:
    RenderTarget(DX11Device& device);
    virtual ~RenderTarget() = default;

protected:
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterizerState;
};

class GeometryBuffer final : public RenderTarget
{
public:
    GeometryBuffer(DX11Device& device);

    void Enable() override;
    void Disable() override;

    GeometryTexture& GetDiffuseTexture() const;
    GeometryTexture& GetSpecularTexture() const;
    GeometryTexture& GetPositionTexture() const;
    GeometryTexture& GetNormalTexture() const;

    DepthStencilTexture& GetDepthStencilTexture() const;

private:
    std::unique_ptr<GeometryTexture> m_DiffuseTexture;
    std::unique_ptr<GeometryTexture> m_SpecularTexture;
    std::unique_ptr<GeometryTexture> m_PositionTexture;
    std::unique_ptr<GeometryTexture> m_NormalTexture;

    std::unique_ptr<DepthStencilTexture> m_DepthStencilTexture;
};

class FrameBuffer final : public RenderTarget
{
public:
    FrameBuffer(DX11Device& device);

    void Enable() override;
    void Disable() override;

private:
    // Frame texture is a part of swap chain output buffer
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_FrameRenderView;
};
