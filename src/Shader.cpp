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

#include "Shader.h"
#include "Device.h"
#include <windows.h>
#include <d3dcompiler.h>
#include <fstream>
#include <stdexcept>
#include <cassert>

Shader::Shader(DX11Device& device, const std::string& source)
{
    ID3D11Device& deviceHandle = device.GetHandle();

    std::ifstream sourceFile(source, std::ios::in | std::ios::binary);
    if (!sourceFile)
    {
        throw std::runtime_error("Failed to open shader: " + source);
    }

    sourceFile.seekg(0, std::ios::end);
    size_t sourceSize = sourceFile.tellg();
    sourceFile.seekg(0, std::ios::beg);

    std::unique_ptr<char[]> shaderSource(new char[sourceSize]);
    sourceFile.read(shaderSource.get(), sourceSize);

#ifndef NDEBUG
    UINT uFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_SKIP_OPTIMIZATION;
#else  // NDEBUG
    UINT uFlags = 0;
#endif // NDEBUG

    {
        D3D_SHADER_MACRO shaderMacro[] = { "VERTEX_SHADER", "", nullptr, nullptr };

        Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> errorsBlob;

        HRESULT hr = D3DCompile(shaderSource.get(), sourceSize, "Vertex Shader", shaderMacro, nullptr, "Main", "vs_5_0", uFlags, 0, &shaderBlob, &errorsBlob);
        if (FAILED(hr))
        {
            std::string error(reinterpret_cast<char*>(errorsBlob->GetBufferPointer()), errorsBlob->GetBufferSize());
            throw std::runtime_error("Failed to compile shader: " + error);
        }

        hr = deviceHandle.CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &m_VertexShader);
        assert(SUCCEEDED(hr));

        D3D11_INPUT_ELEMENT_DESC inputDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 } // Offset for R32G32B32
        };

        hr = deviceHandle.CreateInputLayout(inputDesc, 2, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &m_InputLayout);
        assert(SUCCEEDED(hr));
    }

    {
        D3D_SHADER_MACRO shaderMacro[] = { "PIXEL_SHADER", "", nullptr, nullptr };

        Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> errorsBlob;

        HRESULT hr = D3DCompile(shaderSource.get(), sourceSize, "Pixel Shader", shaderMacro, nullptr, "Main", "ps_5_0", uFlags, 0, &shaderBlob, &errorsBlob);
        if (FAILED(hr))
        {
            std::string error(reinterpret_cast<char*>(errorsBlob->GetBufferPointer()), errorsBlob->GetBufferSize());
            throw std::runtime_error("Failed to compile shader: " + error);
        }

        hr = deviceHandle.CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &m_PixelShader);
        assert(SUCCEEDED(hr));
    }

    {
        D3D11_BUFFER_DESC desc{ };
        desc.ByteWidth = sizeof(m_VertexTransformData);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        HRESULT hr = deviceHandle.CreateBuffer(&desc, nullptr, &m_TransformBuffer);
        assert(SUCCEEDED(hr));
    }
}

const DirectX::XMMATRIX& Shader::GetWorld() const
{
    return m_VertexTransformData.m_World;
}

void Shader::SetWorld(const DirectX::XMMATRIX& world)
{
    m_VertexTransformData.m_World = world;
}

const DirectX::XMMATRIX& Shader::GetViewProjection() const
{
    return m_VertexTransformData.m_ViewProjection;
}

void Shader::SetViewProjection(const DirectX::XMMATRIX& viewProjection)
{
    m_VertexTransformData.m_ViewProjection = viewProjection;
}

void Shader::Enable(DX11Device& device)
{
    ID3D11DeviceContext& deviceContext = device.GetContext();

    deviceContext.VSSetShader(m_VertexShader.Get(), nullptr, 0);
    deviceContext.PSSetShader(m_PixelShader.Get(), nullptr, 0);

    deviceContext.IASetInputLayout(m_InputLayout.Get()); // Input Assembly
    deviceContext.VSSetConstantBuffers(0, 1, m_TransformBuffer.GetAddressOf());
}

void Shader::Update(DX11Device& device)
{
    UpdateVertexTransform(device);
}

void Shader::UpdateVertexTransform(DX11Device& device)
{
    ID3D11DeviceContext& deviceContext = device.GetContext();

    {
        D3D11_MAPPED_SUBRESOURCE mappedSubresource{ };

        deviceContext.Map(m_TransformBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
        std::memcpy(mappedSubresource.pData, &m_VertexTransformData, sizeof(m_VertexTransformData));
        deviceContext.Unmap(m_TransformBuffer.Get(), 0);
    }
}
