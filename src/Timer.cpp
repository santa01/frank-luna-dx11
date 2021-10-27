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

#include "Timer.h"

void Timer::Start(float timeout)
{
    auto now = std::chrono::high_resolution_clock::now();

    if (IsPaused())
    {
        if (IsStarted())
        {
            m_PauseTime += now - m_StartTime;
        }
        else
        {
            m_PauseTime = now;
        }
    }

    m_Timeout = timeout;
    m_StartTime = now;
    m_IsStarted = true;
}

void Timer::Stop()
{
    m_IsStarted = false;
}

void Timer::Pause()
{
    m_PauseTime = std::chrono::high_resolution_clock::now();
    m_IsPaused = true;
}

void Timer::Resume()
{
    m_StartTime += std::chrono::high_resolution_clock::now() - m_PauseTime;
    m_IsPaused = false;
}

bool Timer::IsStarted() const
{
    return m_IsStarted;
}

bool Timer::IsPaused() const
{
    return m_IsPaused;
}

bool Timer::IsElapsed() const
{
    return IsStarted() && (GetLeft() <= 0.0f);
}

float Timer::GetTimeout() const
{
    return m_Timeout;
}

float Timer::GetLeft() const
{
    auto now = std::chrono::high_resolution_clock::now();

    if (IsPaused())
    {
        now = m_PauseTime;
    }

    std::chrono::duration<float> passed = now - m_StartTime;
    return IsStarted() ? (GetTimeout() - passed.count()) : 0.0f;
}
