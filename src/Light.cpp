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

#include "Light.h"
#include "Device.h"

Light::Light(DX11Device& device, LightType type)
{
    m_LightData.m_Type = type;
    m_LightBuffer.reset(new ConstantBuffer<LightData>(device, 0, ResourceInput::INPUT_PIXEL_SHADER));
}

const DirectX::XMFLOAT3& Light::GetColor() const
{
    return m_LightData.m_Color;
}

void Light::SetColor(const DirectX::XMFLOAT3& color)
{
    m_LightData.m_Color = color;
    m_IsDataDirty = true;
}

float Light::GetIntensity() const
{
    return m_LightData.m_Intensity;
}

void Light::SetIntensity(float intensity)
{
    m_LightData.m_Intensity = intensity;
    m_IsDataDirty = true;
}

float Light::GetFalloff() const
{
    return m_LightData.m_Falloff;
}

void Light::SetFalloff(float falloff)
{
    m_LightData.m_Falloff = falloff;
    m_IsDataDirty = true;
}

float Light::GetSpotAngle() const
{
    return DirectX::XMConvertToDegrees(m_LightData.m_SpotAngle);
}

void Light::SetSpotAngle(float angle)
{
    m_LightData.m_SpotAngle = DirectX::XMConvertToRadians(angle);
    m_IsDataDirty = true;
}

float Light::GetSpotBorder() const
{
    return m_LightData.m_SpotBorder;
}

void Light::SetSpotBorder(float border)
{
    m_LightData.m_SpotBorder = border;
    m_IsDataDirty = true;
}

const DirectX::XMVECTOR& Light::GetPosition() const
{
    return m_Position;
}

void Light::Move(const DirectX::XMVECTOR& position)
{
    m_Position = DirectX::XMVectorAdd(m_Position, position);
}

const DirectX::XMVECTOR& Light::GetDirection() const
{
    return m_Direction;
}

void Light::Rotate(const DirectX::XMVECTOR& axis, float angle)
{
    DirectX::XMMATRIX rotation(DirectX::XMMatrixRotationAxis(axis, DirectX::XMConvertToRadians(angle)));
    m_Direction = DirectX::XMVector4Transform(m_Direction, rotation);
}

void Light::Enable()
{
    if (m_IsDataDirty)
    {
        m_LightBuffer->Update(m_LightData);
        m_IsDataDirty = false;
    }

    m_LightBuffer->Enable();
}
