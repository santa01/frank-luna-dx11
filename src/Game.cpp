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
    m_Camera->SetAspectRatio(static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight()));

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
    if (key == VK_ESCAPE)
        context.Terminate();
}

void Game::OnKeyUp(Context& context, unsigned int key)
{ }

void Game::OnMouseDown(Context& context, unsigned int key)
{ }

void Game::OnMouseUp(Context& context, unsigned int key)
{ }

void Game::OnMouseMove(Context& context, int x, int y)
{ }

void Game::Update(Context& context)
{
    m_Shader->SetWVP(m_Camera->GetProjection());
    m_Shader->Enable(context);
    m_Mesh->Draw(context);
}
