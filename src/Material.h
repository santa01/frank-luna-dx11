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

class Material final
{
public:
    Material(DX11Device& device);

    float GetAmbientIntensity() const;
    void SetAmbientIntensity(float intensity);

    float GetDiffuseIntensity() const;
    void SetDiffuseIntensity(float intensity);

    float GetSpecularIntensity() const;
    void SetSpecularIntensity(float intensity);

    int GetSpecularHardness() const;
    void SetSpecularHardness(int hardness);

    void Enable();

private:
    // If the bind flag is D3D11_BIND_CONSTANT_BUFFER, you must set the ByteWidth value in multiples of 16
    // https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_buffer_desc
    struct MaterialData
    {
        float m_AmbientIntensity{ 1.0f };
        float m_DiffuseIntensity{ 1.0f };
        float m_SpecularIntensity{ 1.0f };
        int m_SpecularHardness{ 100 };
    };

    MaterialData m_MaterialData{ };
    std::unique_ptr<ConstantBuffer<MaterialData>> m_MaterialBuffer;
};
