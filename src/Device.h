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

#include "RenderTarget.h"
#include "Shader.h"
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>
#include <memory>

class Context;

class DX11Device final
{
public:
    DX11Device(Context& context);

    ID3D11Device& GetHandle() const;
    ID3D11DeviceContext& GetContext() const;
    IDXGISwapChain1& GetSwapChain() const;

    Shader& GetGeometryShader() const;
    Shader& GetFrameShader() const;

    void GeometryBegin(Context& context);
    void GeometryEnd(Context& context);
    void FrameBegin(Context& context);
    void FrameEnd(Context& context);

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_D3D11Device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_D3D11DeviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain1> m_D3D11SwapChain1;

    std::unique_ptr<GeometryBuffer> m_GeometryBuffer;
    std::unique_ptr<FrameBuffer> m_FrameBuffer;

    std::unique_ptr<Shader> m_GeometryShader;
    std::unique_ptr<Shader> m_FrameShader;
};
