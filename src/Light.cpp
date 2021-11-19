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

AmbientLight::AmbientLight(DX11Device& device)
{
    m_LightBuffer.reset(new ConstantBuffer<LightData>(device, 0, ResourceInput::INPUT_PIXEL_SHADER));
    m_LightBuffer->Update(m_LightData);
}

const DirectX::XMFLOAT3& AmbientLight::GetColor() const
{
    return m_LightData.m_Color;
}

void AmbientLight::SetColor(const DirectX::XMFLOAT3& color)
{
    m_LightData.m_Color = color;
    m_LightBuffer->Update(m_LightData);
}

float AmbientLight::GetIntensity() const
{
    return m_LightData.m_Intensity;
}

void AmbientLight::SetIntensity(float intensity)
{
    m_LightData.m_Intensity = intensity;
    m_LightBuffer->Update(m_LightData);
}

void AmbientLight::Enable()
{
    m_LightBuffer->Enable();
}

DynamicLight::DynamicLight(DX11Device& device, LightType type)
{
    m_LightData.m_Type = type;

    m_LightBuffer.reset(new ConstantBuffer<LightData>(device, 0, ResourceInput::INPUT_PIXEL_SHADER));
    m_LightBuffer->Update(m_LightData);
}

const DirectX::XMFLOAT3& DynamicLight::GetColor() const
{
    return m_LightData.m_Color;
}

void DynamicLight::SetColor(const DirectX::XMFLOAT3& color)
{
    m_LightData.m_Color = color;
    m_LightBuffer->Update(m_LightData);
}

int DynamicLight::GetSpotHardness() const
{
    return m_LightData.m_SpotHardness;
}

void DynamicLight::SetSpotHardness(int hardness)
{
    m_LightData.m_SpotHardness = hardness;
    m_LightBuffer->Update(m_LightData);
}

const DirectX::XMVECTOR& DynamicLight::GetPosition() const
{
    return m_Position;
}

void DynamicLight::Move(const DirectX::XMVECTOR& position)
{
    m_Position = DirectX::XMVectorAdd(m_Position, position);
}

const DirectX::XMVECTOR& DynamicLight::GetDirection() const
{
    return m_Direction;
}

void DynamicLight::Rotate(const DirectX::XMVECTOR& axis, float angle)
{
    DirectX::XMMATRIX rotation(DirectX::XMMatrixRotationAxis(axis, DirectX::XMConvertToRadians(angle)));
    m_Direction = DirectX::XMVector4Transform(m_Direction, rotation);
}

void DynamicLight::Enable()
{
    m_LightBuffer->Enable();
}
