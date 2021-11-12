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

#include "Application.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
#include <memory>
#include <vector>

class Context;

class Game final : public Application
{
public:
    void Start(Context& context) override;
    void Shutdown(Context& context) override;
    void Update(Context& context) override;

    void OnKeyDown(Context& context, unsigned int key);
    void OnKeyUp(Context& context, unsigned int key);
    void OnMouseDown(Context& context, unsigned int key);
    void OnMouseUp(Context& context, unsigned int key);
    void OnMouseMove(Context& context, int x, int y);

    void RenderGeometry(Context& context) override;
    void RenderFrame(Context& context) override;

private:
    std::unique_ptr<Camera> m_Camera;

    std::unique_ptr<Texture> m_Texture;
    std::vector<std::unique_ptr<Mesh>> m_Meshes;
    std::unique_ptr<Mesh> m_Frame;

    bool m_IsLeftMouseButtonPressed{ false };
};
