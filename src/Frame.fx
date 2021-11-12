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

struct VertexInput
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct VertexOutput
{
    float4 position : SV_POSITION; // System Value
    float2 texcoord : TEXCOORD;
};

VertexOutput Main(VertexInput input)
{
    VertexOutput output;

    output.position = float4(input.position, 1.0f);
    output.texcoord = input.texcoord;

    return output;
}

#endif // VERTEX_SHADER

#ifdef PIXEL_SHADER

Texture2D colorTexture : register(t0);
Texture2D positionTexture : register(t1);
SamplerState geometrySampler : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION; // System Value
    float2 texcoord : TEXCOORD;
};

struct PixelOutput
{
    float4 color : SV_Target0; // System Value
};

PixelOutput Main(PixelInput input)
{
    PixelOutput output;

    output.color = colorTexture.Sample(geometrySampler, input.texcoord);

    return output;
}

#endif // PIXEL_SHADER
