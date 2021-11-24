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

#include "Buffer.h"
#include <DirectXMath.h>
#include <memory>

class DX11Device;

enum class LightType
{
    Ambient,
    Direction,
    Point,
    Spot
};

class Light final
{
public:
    Light(DX11Device& device, LightType type);

    const DirectX::XMFLOAT3& GetColor() const;
    void SetColor(const DirectX::XMFLOAT3& color);

    float GetIntensity() const;
    void SetIntensity(float intensity);

    float GetFalloff() const;
    void SetFalloff(float falloff);

    float GetSpotAngle() const;
    void SetSpotAngle(float angle);

    float GetSpotBorder() const;
    void SetSpotBorder(float border);

    const DirectX::XMVECTOR& GetPosition() const;
    void Move(const DirectX::XMVECTOR& position);

    const DirectX::XMVECTOR& GetDirection() const;
    void Rotate(const DirectX::XMVECTOR& axis, float angle);

    void Enable();

private:
    // If the bind flag is D3D11_BIND_CONSTANT_BUFFER, you must set the ByteWidth value in multiples of 16
    // https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_buffer_desc
    struct LightData
    {
        LightType m_Type{ LightType::Direction };
        DirectX::XMFLOAT3 m_Color{ 1.0f, 1.0f, 1.0f };
        float m_Intensity{ 1.0f };
        float m_Falloff{ 20.0f };
        float m_SpotAngle{ DirectX::XM_PIDIV2 / 3.0f };
        float m_SpotBorder{ 0.25f };
    };

    DirectX::XMVECTOR m_Position{ DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };
    DirectX::XMVECTOR m_Direction{ DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) };

    LightData m_LightData{ };
    bool m_IsDataDirty{ true };

    std::unique_ptr<ConstantBuffer<LightData>> m_LightBuffer;
};
