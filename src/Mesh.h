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
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

class DX11Device;

struct Vertex final
{
    DirectX::XMFLOAT3 Position{ 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 Normal{ 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT2 TexCoord{ 0.0f, 0.0f };
};

struct StaticData final
{
    static constexpr Vertex s_QuadVertices[] =
    {
        { { -1.0f,  1.0f,  0.0f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f } }, // 0
        { {  1.0f,  1.0f,  0.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f } }, // 1
        { {  1.0f, -1.0f,  0.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f } }, // 2
        { { -1.0f, -1.0f,  0.0f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f } }  // 3
    };

    static constexpr UINT s_QuadIndices[] =
    {
        0, 1, 2, 0, 2, 3  // Front
    };

    static constexpr Vertex s_CubeVertices[] =
    {
        { { -1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f } }, // 0
        { {  1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f } }, // 1
        { {  1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f } }, // 2
        { { -1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f } }, // 3
        { {  1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f } }, // 4
        { { -1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f } }, // 5
        { { -1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f } }, // 6
        { {  1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f } }, // 7
        { {  1.0f,  1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f } }, // 8
        { {  1.0f,  1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } }, // 9
        { {  1.0f, -1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f } }, // 10
        { {  1.0f, -1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } }, // 11
        { { -1.0f,  1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f } }, // 12
        { { -1.0f,  1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } }, // 13
        { { -1.0f, -1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f } }, // 14
        { { -1.0f, -1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } }, // 15
        { { -1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f } }, // 16
        { {  1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f } }, // 17
        { {  1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f } }, // 18
        { { -1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f } }, // 19
        { {  1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 0.0f } }, // 20
        { { -1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f } }, // 21
        { { -1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 1.0f } }, // 22
        { {  1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f } }  // 23
    };

    static constexpr UINT s_CubeIndices[] =
    {
         0,  1,  2,  0,  2,  3, // Front
         4,  5,  6,  4,  6,  7, // Back
         8,  9, 10,  8, 10, 11, // Right
        12, 13, 14, 12, 14, 15, // Left
        16, 17, 18, 16, 18, 19, // Top
        20, 21, 22, 20, 22, 23  // Bottom
    };
};

struct MeshData final
{
    const Vertex* m_VertexData{ nullptr };
    const UINT* m_IndexData{ nullptr };
    UINT m_VertexSize{ 0 };
    UINT m_IndexSize{ 0 };
};

class Mesh final : public DX11Resource
{
public:
    Mesh(DX11Device& device, const MeshData& data);

    const DirectX::XMMATRIX& GetRotation() const;
    void Rotate(const DirectX::XMVECTOR& axis, float angle);

    const DirectX::XMVECTOR& GetScaling() const;
    void Scale(const DirectX::XMVECTOR& scaling);

    const DirectX::XMVECTOR& GetPosition() const;
    void Move(const DirectX::XMVECTOR& position);

    const DirectX::XMMATRIX& GetWorld() const;

    void Enable() override;
    void Disable() override;

    void Draw() const;

private:
    void UpdateWorld();

    DirectX::XMMATRIX m_Rotataion{ DirectX::XMMatrixIdentity() };
    DirectX::XMVECTOR m_Scaling{ DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f) };
    DirectX::XMVECTOR m_Position{ DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };

    DirectX::XMMATRIX m_World{ DirectX::XMMatrixIdentity() };

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;

    UINT m_Indices{ 0 };
};
