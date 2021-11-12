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

#pragma pack_matrix(row_major) // DirectXMath uses row-major matrices

#ifdef VERTEX_SHADER

cbuffer VertexTransform : register(b0)
{
    float4x4 world;
    float4x4 viewProjection;
};

struct VertexInput
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct VertexOutput
{
    float4 projectionPosition : SV_POSITION; // System Value
    float4 worldPosition : POSITION;
    float2 texcoord : TEXCOORD;
};

VertexOutput Main(VertexInput input)
{
    VertexOutput output;

    float4 vertexPosition = float4(input.position, 1.0f);
    output.projectionPosition = mul(vertexPosition, mul(world, viewProjection));
    output.worldPosition = mul(vertexPosition, world);
    output.texcoord = input.texcoord;

    return output;
}

#endif // VERTEX_SHADER

#ifdef PIXEL_SHADER

struct PixelInput
{
    float4 projectionPosition : SV_POSITION; // System Value
    float4 worldPosition : POSITION;
    float2 texcoord : TEXCOORD;
};

struct PixelOutput
{
    float4 color : SV_Target0; // System Value
    float4 position : SV_Target1; // System Value
};

PixelOutput Main(PixelInput input)
{
    PixelOutput output;

    output.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    output.position = input.worldPosition;

    return output;
}

#endif // PIXEL_SHADER
