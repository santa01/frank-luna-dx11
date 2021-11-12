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
#include <DirectXMath.h>
#include <string>

class DX11Device;

class Shader final
{
public:
    Shader(DX11Device& device, const std::string& source);

    const DirectX::XMMATRIX& GetWorld() const;
    void SetWorld(const DirectX::XMMATRIX& world);

    const DirectX::XMMATRIX& GetViewProjection() const;
    void SetViewProjection(const DirectX::XMMATRIX& viewProjection);

    void Enable(DX11Device& device);
    void Update(DX11Device& device);

private:
    void UpdateVertexTransform(DX11Device& device);

    // If the bind flag is D3D11_BIND_CONSTANT_BUFFER, you must set the ByteWidth value in multiples of 16
    // https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_buffer_desc
    struct VertexTransformData
    {
        // Describes a 4*4 matrix aligned on a 16-byte boundary that maps to four hardware vector registers.
        // https://docs.microsoft.com/en-us/windows/win32/api/directxmath/ns-directxmath-xmmatrix
        DirectX::XMMATRIX m_World{ DirectX::XMMatrixIdentity() };
        DirectX::XMMATRIX m_ViewProjection{ DirectX::XMMatrixIdentity() };
    }
    m_VertexTransformData;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_TransformBuffer;
};
