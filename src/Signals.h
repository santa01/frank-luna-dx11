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

#include <functional>
#include <vector>

template <typename... Args>
using Wrapper = std::function<void(Args...)>;

template <typename... Args>
class Slot final
{
public:
    Slot(const Wrapper<Args...>& wrapper)
        : m_Wrapper(wrapper)
    { }

    template <typename... CallArgs>
    void operator ()(CallArgs&&... args)
    {
        m_Wrapper(std::forward<CallArgs>(args)...);
    }

private:
    Wrapper<Args...> m_Wrapper;
};

template <typename... Args>
class Signal final
{
public:
    void Connect(const Wrapper<Args...>& wrapper)
    {
        m_Slots.emplace_back(wrapper);
    }

    template <typename... CallArgs>
    void operator ()(CallArgs&&... args)
    {
        for (Slot<Args...>& slot : m_Slots)
        {
            slot(std::forward<CallArgs>(args)...);
        }
    }

private:
    std::vector<Slot<Args...>> m_Slots;
};
