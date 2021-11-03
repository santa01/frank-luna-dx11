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

struct VertexOutput
{
    float4 position : SV_POSITION; // System Value
    float4 color : COLOR;
};

#ifdef VERTEX_SHADER

struct VertexInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

VertexOutput Main(VertexInput vertex)
{
    VertexOutput output;

    output.position = float4(vertex.position, 1.0f);
    output.color = vertex.color;

    return output;
}

#endif // VERTEX_SHADER

#ifdef PIXEL_SHADER

float4 Main(VertexOutput vertex) : SV_TARGET
{
    return vertex.color;
}

#endif // PIXEL_SHADER
