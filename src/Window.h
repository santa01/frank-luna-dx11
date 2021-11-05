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

#include <windows.h>

class Context;

class Window final
{
public:
    Window(Context& context);
    ~Window();

    LONG GetWidth() const;
    LONG GetHeight() const;
    HWND GetHandle() const;

    FLOAT GetAspectRatio() const;
    POINT GetCursorPosition() const;
    const BYTE* GetKeyboardState() const;

    void DrawCursor(BOOL draw) const;
    void Update(Context& context);

private:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    const LPCSTR m_WindowClass{ "Window Class" };
    const DWORD m_WindowStyle{ WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX };

    LONG m_Width{ 0 };
    LONG m_Height{ 0 };

    ATOM m_Class{ 0 };
    HWND m_Handle{ nullptr };
    HINSTANCE m_Instance{ nullptr };

    BYTE m_KeyboardState[256] = { };
};
