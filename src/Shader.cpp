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
    : DX11Resource(device)
{
    ID3D11Device& deviceHandle = m_Device.GetHandle();

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
    UINT uFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_SKIP_OPTIMIZATION;
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
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // Offset for R32G32B32 (POSITION)
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }  // Offset for R32G32B32 + R32G32B32 (POSITION + NORMAL)
        };

        hr = deviceHandle.CreateInputLayout(inputDesc, 3, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &m_InputLayout);
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

    m_TransformBuffer.reset(new ConstantBuffer<TransformData>(device, 0, ResourceInput::INPUT_VERTEX_SHADER));
    m_VectorsBuffer.reset(new ConstantBuffer<VectorsData>(device, 1, ResourceInput::INPUT_PIXEL_SHADER));
}

void Shader::SetWorld(const DirectX::XMMATRIX& world)
{
    DirectX::XMVECTOR determinant(DirectX::XMMatrixDeterminant(world));
    DirectX::XMMATRIX worldInverse(DirectX::XMMatrixInverse(&determinant, world));
    DirectX::XMMATRIX worldInverseTranspose(DirectX::XMMatrixTranspose(worldInverse));

    m_TransformData.m_WorldVertices = world;
    m_TransformData.m_WorldNormals = worldInverseTranspose;
    m_TransformBuffer->Update(m_TransformData);
}

void Shader::SetViewProjection(const DirectX::XMMATRIX& viewProjection)
{
    m_TransformData.m_ViewProjection = viewProjection;
    m_TransformBuffer->Update(m_TransformData);
}

void Shader::SetCameraPosition(const DirectX::XMVECTOR& position)
{
    m_VectorsData.m_CameraPosition = position;
    m_VectorsBuffer->Update(m_VectorsData);
}

void Shader::SetLightPosition(const DirectX::XMVECTOR& position)
{
    m_VectorsData.m_LightPosition = position;
    m_VectorsBuffer->Update(m_VectorsData);
}

void Shader::SetLightDirection(const DirectX::XMVECTOR& direction)
{
    m_VectorsData.m_LightDirection = direction;
    m_VectorsBuffer->Update(m_VectorsData);
}

void Shader::SetSampler(UINT slot, D3D11_FILTER filter)
{
    ID3D11Device& deviceHandle = m_Device.GetHandle();

    {
        D3D11_SAMPLER_DESC samplerDesc{ };
        samplerDesc.Filter = filter;
        samplerDesc.MaxAnisotropy = 16;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
        samplerDesc.BorderColor[1] = 1.0f; // Green
        samplerDesc.BorderColor[3] = 1.0f; // Alpha

        Microsoft::WRL::ComPtr<ID3D11SamplerState>& sampler = m_TextureSamplers[slot];
        HRESULT hr = deviceHandle.CreateSamplerState(&samplerDesc, sampler.ReleaseAndGetAddressOf());
        assert(SUCCEEDED(hr));
    }
}

void Shader::Enable()
{
    ID3D11DeviceContext& deviceContext = m_Device.GetContext();

    {
        deviceContext.VSSetShader(m_VertexShader.Get(), nullptr, 0);
        deviceContext.PSSetShader(m_PixelShader.Get(), nullptr, 0);
        deviceContext.IASetInputLayout(m_InputLayout.Get()); // Input Assembly
    }

    m_TransformBuffer->Enable();
    m_VectorsBuffer->Enable();

    for (auto& textureSampler : m_TextureSamplers)
    {
        UINT slot = textureSampler.first;
        Microsoft::WRL::ComPtr<ID3D11SamplerState>& sampler = textureSampler.second;

        ID3D11SamplerState* samplers[] = { sampler.Get() };
        deviceContext.PSSetSamplers(slot, 1, samplers);
    }
}
void Shader::Disable()
{ }
