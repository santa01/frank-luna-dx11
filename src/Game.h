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
#include "RenderTarget.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Light.h"
#include <memory>
#include <vector>

class Context;

class Game final : public Application
{
public:
    void Start(Context& context) override;
    void Shutdown(Context& context) override;

    void Update(Context& context) override;
    void Render(Context& context);

    void OnKeyDown(Context& context, unsigned int key);
    void OnKeyUp(Context& context, unsigned int key);
    void OnMouseDown(Context& context, unsigned int key);
    void OnMouseUp(Context& context, unsigned int key);
    void OnMouseMove(Context& context, int x, int y);

private:
    std::unique_ptr<GeometryBuffer> m_GeometryBuffer;
    std::unique_ptr<FrameBuffer> m_FrameBuffer;

    std::unique_ptr<Shader> m_GeometryShader;
    std::unique_ptr<Shader> m_AmbientLightShader;
    std::unique_ptr<Shader> m_DynamicLightShader;

    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<Material> m_Material;
    std::unique_ptr<ImageTexture> m_Texture;

    std::unique_ptr<Mesh> m_Frame;
    std::vector<std::unique_ptr<Mesh>> m_Meshes;

    std::unique_ptr<Light> m_AmbientLight;
    std::vector<std::unique_ptr<Light>> m_Lights;

    bool m_IsLeftMouseButtonPressed{ false };
};
