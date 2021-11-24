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

#include "Mesh.h"
#include "Device.h"
#include <windows.h>
#include <cassert>

Mesh::Mesh(DX11Device& device, const MeshData& data)
    : DX11Resource(device)
    , m_Indices(data.m_IndexSize / sizeof(data.m_IndexData[0]))
{
    ID3D11Device& deviceHandle = m_Device.GetHandle();

    {
        D3D11_BUFFER_DESC vertexBufferDesc{ };
        vertexBufferDesc.ByteWidth = data.m_VertexSize;
        vertexBufferDesc.StructureByteStride = sizeof(data.m_VertexData[0]);
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertexBufferData{ };
        vertexBufferData.pSysMem = data.m_VertexData;

        HRESULT hr = deviceHandle.CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_VertexBuffer);
        assert(SUCCEEDED(hr));
    }

    {
        D3D11_BUFFER_DESC indexBufferDesc{ };
        indexBufferDesc.ByteWidth = data.m_IndexSize;
        indexBufferDesc.StructureByteStride = sizeof(data.m_IndexData[0]);
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA indexBufferData{ };
        indexBufferData.pSysMem = data.m_IndexData;

        HRESULT hr = deviceHandle.CreateBuffer(&indexBufferDesc, &indexBufferData, &m_IndexBuffer);
        assert(SUCCEEDED(hr));
    }

    UpdateWorld();
}

const DirectX::XMMATRIX& Mesh::GetRotation() const
{
    return m_Rotataion;
}

void Mesh::Rotate(const DirectX::XMVECTOR& axis, float angle)
{
    DirectX::XMMATRIX rotation(DirectX::XMMatrixRotationAxis(axis, DirectX::XMConvertToRadians(angle)));
    m_Rotataion = DirectX::XMMatrixMultiply(m_Rotataion, rotation);
    UpdateWorld();
}

const DirectX::XMVECTOR& Mesh::GetScaling() const
{
    return m_Scaling;
}

void Mesh::Scale(const DirectX::XMVECTOR& scaling)
{
    m_Scaling = DirectX::XMVectorMultiply(m_Scaling, scaling);
    UpdateWorld();
}

const DirectX::XMVECTOR& Mesh::GetPosition() const
{
    return m_Position;
}

void Mesh::Move(const DirectX::XMVECTOR& position)
{
    m_Position = DirectX::XMVectorAdd(m_Position, position);
    UpdateWorld();
}

const DirectX::XMMATRIX& Mesh::GetWorld() const
{
    return m_World;
}

void Mesh::Enable()
{
    ID3D11DeviceContext& deviceContext = m_Device.GetContext();

    {
        ID3D11Buffer* buffers[] = { m_VertexBuffer.Get() };
        UINT strides[] = { sizeof(Vertex) };
        UINT offsets[] = { 0 };

        deviceContext.IASetVertexBuffers(0, 1, buffers, strides, offsets); // Input Assembly
        deviceContext.IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0); // Input Assembly
        deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // Input Assembly
    }
}

void Mesh::Disable()
{
    ID3D11DeviceContext& deviceContext = m_Device.GetContext();

    {
        ID3D11Buffer* buffers[] = { nullptr };
        UINT strides[] = { sizeof(Vertex) };
        UINT offsets[] = { 0 };

        deviceContext.IASetVertexBuffers(0, 1, buffers, strides, offsets); // Input Assembly
        deviceContext.IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0); // Input Assembly
    }
}

void Mesh::Draw() const
{
    ID3D11DeviceContext& deviceContext = m_Device.GetContext();
    deviceContext.DrawIndexed(m_Indices, 0, 0);
}

void Mesh::UpdateWorld()
{
    DirectX::XMMATRIX scaling(DirectX::XMMatrixScalingFromVector(m_Scaling));
    DirectX::XMMATRIX translation(DirectX::XMMatrixTranslationFromVector(m_Position));

    m_World = scaling * m_Rotataion * translation;
}
