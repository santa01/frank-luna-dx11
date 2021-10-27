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

#include "Context.h"
#include "Application.h"
#include <chrono>

Context::Context(Application& application, const ContextInfo& info)
    : m_Application(&application)
{
    m_Window.reset(new Window(info.windowCaption.c_str(), static_cast<LONG>(info.windowWidth), static_cast<LONG>(info.windowHeight)));
    m_Device.reset(new DX11Device(*m_Window));
}

const Window& Context::GetWindow() const
{
    return *m_Window;
}

const DX11Device& Context::GetDevice() const
{
    return *m_Device;
}

float Context::GetFrameTime() const
{
    return m_FrameTime;
}

void Context::Run()
{
    m_Application->Start(*this);

    while (!m_Terminate)
    {
        auto frameBegin = std::chrono::high_resolution_clock::now();

        m_Window->Update(*this);

        m_Device->FrameBegin(*this);
        m_Application->Update(*this);
        m_Device->FrameEnd(*this);

        auto frameEnd = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float> frameDuration = frameEnd - frameBegin;
        m_FrameTime = frameDuration.count();
    }

    m_Application->Shutdown(*this);
}

void Context::Terminate()
{
    m_Terminate = true;
}
