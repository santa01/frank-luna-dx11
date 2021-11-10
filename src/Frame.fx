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

// https://docs.microsoft.com/en-us/windows/win32/dxmath/pg-xnamath-getting-started?redirectedfrom=MSDN#matrix-convention
#pragma pack_matrix(row_major) // DirectXMath uses row-major matrices

struct VertexOutput
{
    // https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics#system-value-semantics
    float4 position : SV_POSITION; // System Value
    float4 color : COLOR;
};

#ifdef VERTEX_SHADER

cbuffer VertexTransform
{
    float4x4 wvp; // World - View - Projection
};

struct VertexInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

VertexOutput Main(VertexInput vertex)
{
    VertexOutput output;

    float4 position = float4(vertex.position, 1.0f);
    output.position = mul(position, wvp);
    output.color = vertex.color;

    return output;
}

#endif // VERTEX_SHADER

#ifdef PIXEL_SHADER

struct PixelOutput
{
    // https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics#system-value-semantics
    float4 color : SV_Target0; // System Value
};

// https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics#system-value-semantics
PixelOutput Main(VertexOutput vertex)
{
    PixelOutput output;

    output.color = vertex.color;

    return output;
}

#endif // PIXEL_SHADER
