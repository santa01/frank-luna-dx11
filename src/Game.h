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
#include "Mesh.h"
#include "Shader.h"
#include <memory>

class Context;

class Game final : public Application
{
public:
    void Start(Context& context) override;
    void Shutdown(Context& context) override;

    void OnKeyDown(Context& context, unsigned int key);
    void OnKeyUp(Context& context, unsigned int key);
    void OnMouseDown(Context& context, unsigned int key);
    void OnMouseUp(Context& context, unsigned int key);
    void OnMouseMove(Context& context, int x, int y);

    void Update(Context& context) override;

private:
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<Mesh> m_Mesh;
};
