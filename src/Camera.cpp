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

#include "Camera.h"

Camera::Camera()
{
    UpdateView();
    UpdateProjection();
}

const DirectX::XMVECTOR& Camera::GetPosition() const
{
    return m_Position;
}

void Camera::Move(const DirectX::XMVECTOR& vector)
{
    m_Position = DirectX::XMVectorAdd(m_Position, vector);
    UpdateView();
}

const DirectX::XMVECTOR& Camera::GetRight() const
{
    return m_Right;
}

const DirectX::XMVECTOR& Camera::GetUp() const
{
    return m_Up;
}

const DirectX::XMVECTOR& Camera::GetForward() const
{
    return m_Forward;
}

void Camera::Rotate(const DirectX::XMVECTOR& axis, float angle)
{
    DirectX::XMMATRIX rotation(DirectX::XMMatrixRotationAxis(axis, DirectX::XMConvertToRadians(angle)));
    m_Up = DirectX::XMVector4Transform(m_Up, rotation);
    m_Forward = DirectX::XMVector4Transform(m_Forward, rotation);
    UpdateView();
    m_Right = { DirectX::XMVectorGetX(m_View.r[0]), DirectX::XMVectorGetX(m_View.r[1]), DirectX::XMVectorGetX(m_View.r[2]), DirectX::XMVectorGetX(m_View.r[3]) };
}

const DirectX::XMMATRIX& Camera::GetView() const
{
    return m_View;
}

float Camera::GetFov() const
{
    return m_Fov;
}

void Camera::SetFov(float fov)
{
    m_Fov = fov;
    UpdateProjection();
}

float Camera::GetAspectRatio() const
{
    return m_AspectRatio;
}

void Camera::SetAspectRatio(float aspectRatio)
{
    m_AspectRatio = aspectRatio;
    UpdateProjection();
}

float Camera::GetNearPlane() const
{
    return m_NearPlane;
}

void Camera::SetNearPlane(float nearPlane)
{
    m_NearPlane = nearPlane;
    UpdateProjection();
}

float Camera::GetFarPlane() const
{
    return m_FarPlane;
}

void Camera::SetFarPlane(float farPlane)
{
    m_FarPlane = farPlane;
    UpdateProjection();
}

const DirectX::XMMATRIX& Camera::GetProjection() const
{
    return m_Projection;
}

void Camera::UpdateView()
{
    m_View = DirectX::XMMatrixLookToLH(m_Position, m_Forward, m_Up);
}

void Camera::UpdateProjection()
{
    m_Projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_Fov), m_AspectRatio, m_NearPlane, m_FarPlane);
}
