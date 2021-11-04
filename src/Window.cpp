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

#include "Window.h"
#include "Context.h"
#include <windowsx.h>
#include <stdexcept>

Window::Window(Context& context)
{
    const ContextParams& params = context.GetParams();

    m_Width = static_cast<LONG>(params.m_WindowWidth);
    m_Height = static_cast<LONG>(params.m_WindowHeight);
    m_Instance = GetModuleHandle(nullptr);

    {
        WNDCLASSEX wcex{ };
        wcex.cbSize = sizeof(wcex);
        wcex.hInstance = m_Instance;
        wcex.lpfnWndProc = Window::WindowProc;
        wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
        wcex.lpszMenuName = params.m_WindowCaption.c_str();
        wcex.lpszClassName = m_WindowClass;

        m_Class = RegisterClassEx(&wcex);
        if (!m_Class)
            throw std::runtime_error("Failed to register window class");
    }

    {
        RECT windowRect{ 0, 0, m_Width, m_Height };
        AdjustWindowRectEx(&windowRect, m_WindowStyle & ~WS_OVERLAPPED, false, 0);

        LONG windowWidth = windowRect.right - windowRect.left;
        LONG windowHeight = windowRect.bottom - windowRect.top;

        m_Handle = CreateWindowEx(0, m_WindowClass, params.m_WindowCaption.c_str(), m_WindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, nullptr, nullptr, m_Instance, nullptr);
        if (!m_Handle)
            throw std::runtime_error("Failed to create window");
    }

    ShowWindow(m_Handle, SW_SHOW);
    UpdateWindow(m_Handle);
}

Window::~Window()
{
    if (m_Handle)
        DestroyWindow(m_Handle);

    if (m_Class)
        UnregisterClass(m_WindowClass, m_Instance);
}

LONG Window::GetWidth() const
{
    return m_Width;
}

LONG Window::GetHeight() const
{
    return m_Height;
}

HWND Window::GetHandle() const
{
    return m_Handle;
}

FLOAT Window::GetAspectRatio() const
{
    return static_cast<FLOAT>(m_Width) / static_cast<FLOAT>(m_Height);
}

POINT Window::GetCursorPosition() const
{
    POINT cursorPosition{ };
    GetCursorPos(&cursorPosition);
    ScreenToClient(m_Handle, &cursorPosition);
    return cursorPosition;
}

void Window::DrawCursor(BOOL draw) const
{
    ShowCursor(draw);
}

void Window::Update(Context& context)
{
    SetWindowLongPtr(m_Handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&context));

    MSG msg{ };
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
            context.Terminate();
    }
}

LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Context& context = *reinterpret_cast<Context*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (uMsg)
    {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
        context.OnKeyDown(context, static_cast<unsigned int>(wParam));
        return 0;
    }

    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        context.OnKeyUp(context, static_cast<unsigned int>(wParam));
        return 0;
    }

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    {
        context.OnMouseDown(context, uMsg == WM_LBUTTONDOWN ? VK_LBUTTON : VK_RBUTTON);
        return 0;
    }

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    {
        context.OnMouseUp(context, uMsg == WM_LBUTTONUP ? VK_LBUTTON : VK_RBUTTON);
        return 0;
    }

    case WM_MOUSEMOVE:
    {
        context.OnMouseMove(context, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
