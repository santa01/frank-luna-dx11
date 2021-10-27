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
#include <stdexcept>

Window::Window(LPCSTR caption, LONG width, LONG height)
    : m_Width(width)
    , m_Height(height)
    , m_Instance(GetModuleHandle(nullptr))
{
    {
        WNDCLASSEX wcex{ };
        wcex.cbSize = sizeof(wcex);
        wcex.hInstance = m_Instance;
        wcex.lpfnWndProc = Window::WindowProc;
        wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
        wcex.lpszMenuName = caption;
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

        m_Handle = CreateWindowEx(0, m_WindowClass, caption, m_WindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, nullptr, nullptr, m_Instance, nullptr);
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
    Context* context = reinterpret_cast<Context*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (uMsg)
    {
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    {
        //int virtualKey = static_cast<int>(wParam);
        //KeyboardKey keyboardKey = Input::keycodeToKeyboardKey(virtualKey);

        //if (keyboardKey != KeyboardKey::KEY_UNKNOWN) {
        //    bool keyState = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN);
        //    self->keyboardState[keyboardKey] = keyState;
        //    self->onKeyboardKeySignal(keyboardKey, keyState);
        //}

        return 0;
    }

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    {
        //bool mouseButtonState = (message == WM_LBUTTONDOWN);
        //self->mouseState[MouseButton::BUTTON_LEFT] = mouseButtonState;
        //self->onMouseButtonSignal(MouseButton::BUTTON_LEFT, mouseButtonState);
        return 0;
    }

    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    {
        //bool mouseButtonState = (message == WM_RBUTTONDOWN);
        //self->mouseState[MouseButton::BUTTON_RIGHT] = mouseButtonState;
        //self->onMouseButtonSignal(MouseButton::BUTTON_RIGHT, mouseButtonState);
        return 0;
    }

    case WM_MOUSEMOVE:
    {
        //int xPosition = GET_X_LPARAM(lParam);
        //int yPosition = GET_Y_LPARAM(lParam);
        //bool mouseMoved = true;

        //if (self->isMouseCaptured()) {
        //    POINT windowCenter = { self->width >> 1, self->height >> 1 };
        //    xPosition -= windowCenter.x;
        //    yPosition -= windowCenter.y;

        //    if (xPosition != 0 || yPosition != 0) {
        //        self->warpMousePointer(windowCenter.x, windowCenter.y);
        //    }
        //    else {
        //        mouseMoved = false;  // WM_MOUSEMOVE after SetCursorPos() to window center
        //    }
        //}

        //if (mouseMoved) {
        //    self->mousePosition = std::make_pair(xPosition, yPosition);
        //    self->onMouseMotionSignal(xPosition, yPosition);
        //}

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
