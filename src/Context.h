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

#include "Window.h"
#include "Device.h"
#include "Signals.h"
#include <memory>
#include <string>

class Application;

struct ContextInfo final
{
    std::string windowCaption;
    size_t windowWidth;
    size_t windowHeight;
};

class Context final
{
public:
    Context(Application& application, const ContextInfo& info);

    Window& GetWindow() const;
    DX11Device& GetDevice() const;
    float GetFrameTime() const;

    void Run();
    void Terminate();

    Signal<Context&, unsigned int> OnKeyDown;
    Signal<Context&, unsigned int> OnKeyUp;
    Signal<Context&, unsigned int> OnMouseDown;
    Signal<Context&, unsigned int> OnMouseUp;
    Signal<Context&, int, int> OnMouseMove;

private:
    Application* m_Application{ nullptr };

    std::unique_ptr<Window> m_Window;
    std::unique_ptr<DX11Device> m_Device;

    float m_FrameTime{ 0.0f };
    bool m_Terminate{ false };
};
