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
#include "Context.h"
#include <windows.h>
#include <d3dcompiler.h>
#include <string>
#include <stdexcept>
#include <cassert>

Shader::Shader(Context& context)
{
    ID3D11Device& device = context.GetDevice().GetHandle();

#ifndef NDEBUG
    UINT uFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_SKIP_OPTIMIZATION;
#else  // NDEBUG
    UINT uFlags = 0;
#endif // NDEBUG

    {
        Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> errorsBlob;

        HRESULT hr = D3DCompile(m_VertexShaderSource, sizeof(m_VertexShaderSource), "Vertex Shader", nullptr, nullptr, "Main", "vs_5_0", uFlags, 0, &shaderBlob, &errorsBlob);
        if (FAILED(hr))
        {
            std::string error(reinterpret_cast<char*>(errorsBlob->GetBufferPointer()), errorsBlob->GetBufferSize());
            throw std::runtime_error("Failed to compile shader: " + error);
        }

        hr = device.CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &m_VertexShader);
        assert(SUCCEEDED(hr));

        D3D11_INPUT_ELEMENT_DESC inputDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 } // Offset for R32G32B32
        };

        hr = device.CreateInputLayout(inputDesc, 2, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &m_InputLayout);
        assert(SUCCEEDED(hr));
    }

    {
        Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> errorsBlob;

        HRESULT hr = D3DCompile(m_PixelShaderSource, sizeof(m_PixelShaderSource), "Pixel Shader", nullptr, nullptr, "Main", "ps_5_0", uFlags, 0, &shaderBlob, &errorsBlob);
        if (FAILED(hr))
        {
            std::string error(reinterpret_cast<char*>(errorsBlob->GetBufferPointer()), errorsBlob->GetBufferSize());
            throw std::runtime_error("Failed to compile shader: " + error);
        }

        hr = device.CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &m_PixelShader);
        assert(SUCCEEDED(hr));
    }
}

void Shader::Enable(Context& context) const
{
    ID3D11DeviceContext& deviceContext = context.GetDevice().GetContext();

    deviceContext.VSSetShader(m_VertexShader.Get(), nullptr, 0);
    deviceContext.PSSetShader(m_PixelShader.Get(), nullptr, 0);

    deviceContext.IASetInputLayout(m_InputLayout.Get()); // Input Assembly
}
