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
    float3 normal : NORMAL;
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

Texture2D diffuseTexture : register(t0);
Texture2D specularTexture : register(t1);
Texture2D positionTexture : register(t2);
Texture2D normalTexture : register(t3);
SamplerState geometrySampler : register(s0);

cbuffer AmbientLight : register(b0)
{
    float3 ambientLightColor;
    float ambientLightIntensity;
};

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
    float4 diffuseSample = diffuseTexture.Sample(geometrySampler, input.texcoord);

    float3 diffuseColor = diffuseSample.rgb;
    float ambientIntensity = diffuseSample.a;

    float3 pixelColor = mul(diffuseColor, ambientIntensity);
    float3 lightColor = mul(ambientLightColor, ambientLightIntensity);

    PixelOutput output;
    output.color = float4(pixelColor * lightColor, 1.0f);
    return output;
}

#endif // PIXEL_SHADER
