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
#include <windows.h>
#include <memory>
#include <cassert>

class DX11Device;

enum ResourceInput
{
    INPUT_VERTEX_SHADER = (1 << 0),
    INPUT_PIXEL_SHADER  = (1 << 1)
};

template <typename T>
class ConstantBuffer final : public DX11Resource
{
public:
    ConstantBuffer(DX11Device& device, UINT slot, ResourceInput input)
        : DX11Resource(device)
        , m_ResourceSlot(slot)
        , m_ResourceInput(input)
    {
        ID3D11Device& deviceHandle = m_Device.GetHandle();

        {
            D3D11_BUFFER_DESC desc{ };
            desc.ByteWidth = sizeof(T);
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            HRESULT hr = deviceHandle.CreateBuffer(&desc, nullptr, &m_ConstantBuffer);
            assert(SUCCEEDED(hr));
        }
    }

    void Enable() override
    {
        ID3D11DeviceContext& deviceContext = m_Device.GetContext();

        {
            ID3D11Buffer* buffers[] = { m_ConstantBuffer.Get() };

            if (m_ResourceInput & INPUT_VERTEX_SHADER)
                deviceContext.VSSetConstantBuffers(m_ResourceSlot, 1, buffers);

            if (m_ResourceInput & INPUT_PIXEL_SHADER)
                deviceContext.PSSetConstantBuffers(m_ResourceSlot, 1, buffers);
        }
    }

    void Disable() override
    {
        ID3D11DeviceContext& deviceContext = m_Device.GetContext();

        {
            ID3D11Buffer* buffers[] = { nullptr };

            if (m_ResourceInput & INPUT_VERTEX_SHADER)
                deviceContext.VSSetConstantBuffers(m_ResourceSlot, 1, buffers);

            if (m_ResourceInput & INPUT_PIXEL_SHADER)
                deviceContext.PSSetConstantBuffers(m_ResourceSlot, 1, buffers);
        }
    }

    void Update(const T& data)
    {
        ID3D11DeviceContext& deviceContext = m_Device.GetContext();

        {
            D3D11_MAPPED_SUBRESOURCE mappedSubresource{ };

            deviceContext.Map(m_ConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
            std::memcpy(mappedSubresource.pData, &data, sizeof(data));
            deviceContext.Unmap(m_ConstantBuffer.Get(), 0);
        }
    }

private:
    UINT m_ResourceSlot{ 0 };
    ResourceInput m_ResourceInput{ 0 };

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_ConstantBuffer;
};
