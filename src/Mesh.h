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

class Context;

class Mesh final
{
public:
    Mesh(Context& context);

    const DirectX::XMMATRIX& GetRotation() const;
    void Rotate(const DirectX::XMVECTOR& axis, float angle);

    const DirectX::XMVECTOR& GetScaling() const;
    void Scale(const DirectX::XMVECTOR& scaling);

    const DirectX::XMVECTOR& GetPosition() const;
    void Move(const DirectX::XMVECTOR& position);

    const DirectX::XMMATRIX& GetWorld() const;

    void Draw(Context& context) const;

private:
    void UpdateWorld();

    DirectX::XMMATRIX m_Rotataion{ DirectX::XMMatrixIdentity() };
    DirectX::XMVECTOR m_Scaling{ 1.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMVECTOR m_Position{ 0.0f, 0.0f, 0.0f, 1.0f };

    DirectX::XMMATRIX m_World{ DirectX::XMMatrixIdentity() };

    struct Vertex
    {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT4 Color;
    };

    static constexpr Vertex m_Vertices[] =
    {
        { {  0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { {  1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };

    static constexpr UINT m_Indices[] =
    {
        0, 1, 2 // Clockwise
    };

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;
};
