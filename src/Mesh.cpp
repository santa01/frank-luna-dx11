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
#include "Context.h"
#include <windows.h>
#include <cassert>

Mesh::Mesh(Context& context)
{
    ID3D11Device& device = context.GetDevice().GetHandle();

    {
        D3D11_BUFFER_DESC vertexBufferDesc{ };
        vertexBufferDesc.ByteWidth = sizeof(m_Vertices);
        vertexBufferDesc.StructureByteStride = sizeof(m_Vertices[0]);
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertexBufferData{ };
        vertexBufferData.pSysMem = m_Vertices;

        HRESULT hr = device.CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_VertexBuffer);
        assert(SUCCEEDED(hr));
    }

    {
        D3D11_BUFFER_DESC indexBufferDesc{ };
        indexBufferDesc.ByteWidth = sizeof(m_Indices);
        indexBufferDesc.StructureByteStride = sizeof(m_Indices[0]);
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA indexBufferData{ };
        indexBufferData.pSysMem = m_Indices;

        HRESULT hr = device.CreateBuffer(&indexBufferDesc, &indexBufferData, &m_IndexBuffer);
        assert(SUCCEEDED(hr));
    }
}

void Mesh::Draw(Context& context) const
{
    ID3D11DeviceContext& deviceContext = context.GetDevice().GetContext();

    {
        ID3D11Buffer* buffers[] = { m_VertexBuffer.Get() };
        UINT strides[] = { sizeof(m_Vertices[0]) };
        UINT offsets[] = { 0 };

        deviceContext.IASetVertexBuffers(0, 1, buffers, strides, offsets); // Input Assembly
        deviceContext.IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0); // Input Assembly
        deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // Input Assembly
    }

    deviceContext.DrawIndexed(sizeof(m_Indices) / sizeof(m_Indices[0]), 0, 0);
}
