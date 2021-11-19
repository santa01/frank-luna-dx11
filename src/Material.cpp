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

#include "Material.h"
#include "Device.h"

Material::Material(DX11Device& device)
{
    m_MaterialBuffer.reset(new ConstantBuffer<MaterialData>(device, 0, ResourceInput::INPUT_PIXEL_SHADER));
    m_MaterialBuffer->Update(m_MaterialData);
}

float Material::GetAmbientIntensity() const
{
    return m_MaterialData.m_AmbientIntensity;
}

void Material::SetAmbientIntensity(float intensity)
{
    m_MaterialData.m_AmbientIntensity = intensity;
    m_MaterialBuffer->Update(m_MaterialData);
}

float Material::GetDiffuseIntensity() const
{
    return m_MaterialData.m_DiffuseIntensity;
}

void Material::SetDiffuseIntensity(float intensity)
{
    m_MaterialData.m_DiffuseIntensity = intensity;
    m_MaterialBuffer->Update(m_MaterialData);
}

float Material::GetSpecularIntensity() const
{
    return m_MaterialData.m_SpecularIntensity;
}

void Material::SetSpecularIntensity(float intensity)
{
    m_MaterialData.m_SpecularIntensity = intensity;
    m_MaterialBuffer->Update(m_MaterialData);
}

int Material::GetSpecularHardness() const
{
    return m_MaterialData.m_SpecularHardness;
}

void Material::SetSpecularHardness(int hardness)
{
    m_MaterialData.m_SpecularHardness = hardness;
    m_MaterialBuffer->Update(m_MaterialData);
}

void Material::Enable()
{
    m_MaterialBuffer->Enable();
}
