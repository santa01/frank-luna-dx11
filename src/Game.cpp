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

    m_Camera.reset(new Camera());
    m_Camera->SetAspectRatio(window.GetAspectRatio());
    m_Camera->Move({ 0.0f, 0.0f, -5.0f, 0.0f });

    m_Shader.reset(new Shader(context, "Shader.fx"));
    m_Mesh.reset(new Mesh(context));

    context.OnKeyDown.Connect(std::bind(&Game::OnKeyDown, this, std::placeholders::_1, std::placeholders::_2));
    context.OnKeyUp.Connect(std::bind(&Game::OnKeyUp, this, std::placeholders::_1, std::placeholders::_2));
    context.OnMouseDown.Connect(std::bind(&Game::OnMouseDown, this, std::placeholders::_1, std::placeholders::_2));
    context.OnMouseUp.Connect(std::bind(&Game::OnMouseUp, this, std::placeholders::_1, std::placeholders::_2));
    context.OnMouseMove.Connect(std::bind(&Game::OnMouseMove, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void Game::Shutdown(Context& context)
{ }

void Game::OnKeyDown(Context& context, unsigned int key)
{
    if (key == 'W')
    {
        const DirectX::XMVECTOR& forward = m_Camera->GetForward();
        m_Camera->Move(DirectX::XMVectorScale(forward, 0.1f));
    }

    if (key == 'S')
    {
        const DirectX::XMVECTOR& forward = m_Camera->GetForward();
        m_Camera->Move(DirectX::XMVectorScale(forward, -0.1f));
    }

    if (key == 'A')
    {
        const DirectX::XMVECTOR& right = m_Camera->GetRight();
        m_Camera->Move(DirectX::XMVectorScale(right, -0.1f));
    }

    if (key == 'D')
    {
        const DirectX::XMVECTOR& right = m_Camera->GetRight();
        m_Camera->Move(DirectX::XMVectorScale(right, 0.1f));
    }

    bool isShiftPressed = GetAsyncKeyState(VK_SHIFT);
    bool isControlPressed = GetAsyncKeyState(VK_CONTROL);

    if (key == '1' || key == '2')
    {
        bool isRight = (key == '2');

        if (isShiftPressed)
            m_Mesh->Scale({ isRight ? 1.25f : 0.8f, 1.0f, 1.0f, 1.0f });
        else if (isControlPressed)
            m_Mesh->Rotate({ 1.0f, 0.0f, 0.0f, 0.0f }, isRight ? 5.0f : -5.0f);
        else
            m_Mesh->Move({ isRight ? 0.25f : -0.25f, 0.0f, 0.0f, 0.0f });
    }

    if (key == '3' || key == '4')
    {
        bool isUp = (key == '4');

        if (isShiftPressed)
            m_Mesh->Scale({ 1.0f, isUp ? 1.25f : 0.8f, 1.0f, 1.0f });
        else if (isControlPressed)
            m_Mesh->Rotate({ 0.0f, 1.0f, 0.0f, 0.0f }, isUp ? 5.0f : -5.0f);
        else
            m_Mesh->Move({ 0.0f, isUp ? 0.25f : -0.25f, 0.0f, 0.0f });
    }

    if (key == '5' || key == '6')
    {
        bool isForward = (key == '6');

        if (isShiftPressed)
            m_Mesh->Scale({ 1.0f, 1.0f, isForward ? 1.25f : 0.8f, 1.0f });
        else if (isControlPressed)
            m_Mesh->Rotate({ 0.0f, 0.0f, 1.0f, 0.0f }, isForward ? 5.0f : -5.0f);
        else
            m_Mesh->Move({ 0.0f, 0.0f, isForward ? 0.25f : -0.25f, 0.0f });
    }

    if (key == VK_ESCAPE)
        context.Terminate();
}

void Game::OnKeyUp(Context& context, unsigned int key)
{ }

void Game::OnMouseDown(Context& context, unsigned int key)
{
    Window& window = context.GetWindow();

    if (!m_IsLeftMouseButtonPressed && key == VK_LBUTTON)
    {
        m_IsLeftMouseButtonPressed = true;
        window.DrawCursor(false);

        POINT cursorPosition(window.GetCursorPosition());
        m_MouseX = cursorPosition.x;
        m_MouseY = cursorPosition.y;
    }
}

void Game::OnMouseUp(Context& context, unsigned int key)
{
    Window& window = context.GetWindow();

    if (m_IsLeftMouseButtonPressed && key == VK_LBUTTON)
    {
        m_IsLeftMouseButtonPressed = false;
        window.DrawCursor(true);
    }
}

void Game::OnMouseMove(Context& context, int x, int y)
{
    if (m_IsLeftMouseButtonPressed)
    {
        int xDelta = x - m_MouseX;
        int yDelta = y - m_MouseY;

        if (xDelta != 0)
            m_Camera->Rotate({ 0.0f, 1.0f, 0.0f, 0.0f }, static_cast<float>(xDelta) * 0.25f);

        if (yDelta != 0)
            m_Camera->Rotate(m_Camera->GetRight(), static_cast<float>(yDelta) * 0.25f);

        m_MouseX = x;
        m_MouseY = y;
    }
}

void Game::Update(Context& context)
{
    DirectX::XMMATRIX vp = DirectX::XMMatrixMultiply(m_Camera->GetView(), m_Camera->GetProjection());
    DirectX::XMMATRIX wvp = DirectX::XMMatrixMultiply(m_Mesh->GetWorld(), vp);

    m_Shader->SetWVP(wvp);
    m_Shader->Enable(context);
    m_Mesh->Draw(context);
}
