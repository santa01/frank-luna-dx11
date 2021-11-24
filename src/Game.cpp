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

#include "Game.h"
#include "Context.h"

void Game::Start(Context& context)
{
    Window& window = context.GetWindow();
    DX11Device& device = context.GetDevice();

    m_Camera.reset(new Camera());
    m_Camera->SetAspectRatio(window.GetAspectRatio());

    m_Camera->Move(DirectX::XMVectorSet(4.0f, 3.0f, -3.0f, 0.0f));
    m_Camera->Rotate(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), -30.0f);
    m_Camera->Rotate(m_Camera->GetRight(), 30.0f);

    m_Material.reset(new Material(device));
    m_Texture.reset(new ImageTexture(device, 0, L"Sviborg.dds"));

    MeshData quad
    {
        StaticData::s_QuadVertices,
        StaticData::s_QuadIndices,
        sizeof(StaticData::s_QuadVertices),
        sizeof(StaticData::s_QuadIndices)
    };

    m_Frame.reset(new Mesh(device, quad));

    MeshData cube
    {
        StaticData::s_CubeVertices,
        StaticData::s_CubeIndices,
        sizeof(StaticData::s_CubeVertices),
        sizeof(StaticData::s_CubeIndices)
    };

    auto& mesh1 = m_Meshes.emplace_back(new Mesh(device, cube));
    mesh1->Rotate(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 35.0f);

    auto& mesh2 = m_Meshes.emplace_back(new Mesh(device, cube));
    mesh2->Scale(DirectX::XMVectorSet(0.75f, 0.75f, 0.75f, 1.0f));
    mesh2->Move(DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f));

    auto& mesh3 = m_Meshes.emplace_back(new Mesh(device, cube));
    mesh3->Rotate(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), 45.0f);
    mesh3->Rotate(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), 15.0f);
    mesh3->Move(DirectX::XMVectorSet(3.0f, 0.0f, 3.0f, 0.0f));

    auto& mesh4 = m_Meshes.emplace_back(new Mesh(device, quad));
    mesh4->Rotate(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), 90.0f);
    mesh4->Scale(DirectX::XMVectorSet(20.0f, 20.0f, 20.0f, 1.0f));
    mesh4->Move(DirectX::XMVectorSet(0.0f, -2.0f, 0.0f, 0.0f));

    m_AmbientLight.reset(new Light(device, LightType::Ambient));
    m_AmbientLight->SetIntensity(0.2f);

    //auto& light1 = m_DynamicLights.emplace_back(new DynamicLight(device, LightType::Direction));
    //auto& light1 = m_DynamicLights.emplace_back(new DynamicLight(device, LightType::Point));
    auto& light1 = m_Lights.emplace_back(new Light(device, LightType::Spot));
    light1->SetColor({ 1.0f, 0.0f, 0.0f });
    light1->Move(DirectX::XMVectorSet(0.0f, 5.0f, 5.0f, 0.0f));
    light1->Rotate(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), 135.0f);

    //auto& light2 = m_DynamicLights.emplace_back(new DynamicLight(device, LightType::Direction));
    //auto& light2 = m_DynamicLights.emplace_back(new DynamicLight(device, LightType::Point));
    auto& light2 = m_Lights.emplace_back(new Light(device, LightType::Spot));
    light2->SetColor({ 0.0f, 1.0f, 0.0f });
    light2->Move(DirectX::XMVectorSet(0.0f, 5.0f, -5.0f, 0.0f));
    light2->Rotate(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), 45.0f);

    context.OnKeyDown.Connect(std::bind(&Game::OnKeyDown, this, std::placeholders::_1, std::placeholders::_2));
    context.OnKeyUp.Connect(std::bind(&Game::OnKeyUp, this, std::placeholders::_1, std::placeholders::_2));
    context.OnMouseDown.Connect(std::bind(&Game::OnMouseDown, this, std::placeholders::_1, std::placeholders::_2));
    context.OnMouseUp.Connect(std::bind(&Game::OnMouseUp, this, std::placeholders::_1, std::placeholders::_2));
    context.OnMouseMove.Connect(std::bind(&Game::OnMouseMove, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void Game::Shutdown(Context& context)
{ }

void Game::Update(Context& context)
{
    Window& window = context.GetWindow();

    const BYTE* keyboardState = window.GetKeyboardState();
    float moveStep = context.GetFrameTime() * 5.0f;

    if (keyboardState['W'])
    {
        const DirectX::XMVECTOR& forward = m_Camera->GetForward();
        m_Camera->Move(DirectX::XMVectorScale(forward, moveStep));
    }

    if (keyboardState['S'])
    {
        const DirectX::XMVECTOR& forward = m_Camera->GetForward();
        m_Camera->Move(DirectX::XMVectorScale(forward, -moveStep));
    }

    if (keyboardState['A'])
    {
        const DirectX::XMVECTOR& right = m_Camera->GetRight();
        m_Camera->Move(DirectX::XMVectorScale(right, -moveStep));
    }

    if (keyboardState['D'])
    {
        const DirectX::XMVECTOR& right = m_Camera->GetRight();
        m_Camera->Move(DirectX::XMVectorScale(right, moveStep));
    }

    if (keyboardState[VK_ESCAPE])
        context.Terminate();
}

void Game::OnKeyDown(Context& context, unsigned int key)
{ }

void Game::OnKeyUp(Context& context, unsigned int key)
{ }

void Game::OnMouseDown(Context& context, unsigned int key)
{
    Window& window = context.GetWindow();

    if (!m_IsLeftMouseButtonPressed && key == VK_LBUTTON)
    {
        window.DrawCursor(false);
        window.LockCursor(true);

        m_IsLeftMouseButtonPressed = true;
    }
}

void Game::OnMouseUp(Context& context, unsigned int key)
{
    Window& window = context.GetWindow();

    if (m_IsLeftMouseButtonPressed && key == VK_LBUTTON)
    {
        window.DrawCursor(true);
        window.LockCursor(false);

        m_IsLeftMouseButtonPressed = false;
    }
}

void Game::OnMouseMove(Context& context, int x, int y)
{
    if (m_IsLeftMouseButtonPressed)
    {
        if (x != 0)
            m_Camera->Rotate({ 0.0f, 1.0f, 0.0f, 0.0f }, static_cast<float>(x) * 0.25f);

        if (y != 0)
            m_Camera->Rotate(m_Camera->GetRight(), static_cast<float>(y) * 0.25f);
    }
}

void Game::RenderGeometry(Context& context)
{
    DX11Device& device = context.GetDevice();

    Shader& shader = device.GetGeometryShader();
    shader.SetViewProjection(DirectX::XMMatrixMultiply(m_Camera->GetView(), m_Camera->GetProjection()));

    m_Material->Enable();
    m_Texture->Enable();

    for (auto& mesh : m_Meshes)
    {
        shader.SetWorld(mesh->GetWorld());
        shader.UpdateTransform();

        mesh->Draw(device);
    }
}

void Game::RenderAmbientLight(Context& context)
{
    DX11Device& device = context.GetDevice();

    if (m_AmbientLight != nullptr)
    {
        m_AmbientLight->Enable();
        m_Frame->Draw(device);
    }
}

void Game::RenderDynamicLight(Context& context)
{
    DX11Device& device = context.GetDevice();

    Shader& shader = device.GetDynamicLightShader();
    shader.SetCameraPosition(m_Camera->GetPosition());

    for (auto& light : m_Lights)
    {
        shader.SetLightPosition(light->GetPosition());
        shader.SetLightDirection(light->GetDirection());
        shader.UpdateVectors();

        light->Enable();
        m_Frame->Draw(device);
    }
}
