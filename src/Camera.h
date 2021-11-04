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

#include <DirectXMath.h>

class Camera final
{
public:
    Camera();

    float GetFov() const;
    void SetFov(float fov);

    float GetAspectRatio() const;
    void SetAspectRatio(float aspectRatio);

    float GetNearPlane() const;
    void SetNearPlane(float nearPlane);

    float GetFarPlane() const;
    void SetFarPlane(float farPlane);

    const DirectX::XMMATRIX& GetProjection() const;

private:
    void UpdateProjection();

    float m_Fov{ 90.0f };
    float m_AspectRatio{ 1.0f };
    float m_NearPlane{ 1.0f };
    float m_FarPlane{ 10.0f };

    DirectX::XMMATRIX m_Projection{ };
};
