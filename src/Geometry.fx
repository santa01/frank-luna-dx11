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
    float3 pixelPosition : PIXEL_POSITION;
    float3 pixelNormal : PIXEL_NORMAL;
    float2 texcoord : TEXCOORD;
};

VertexOutput Main(VertexInput input)
{
    VertexOutput output;

    float4 vertexPosition = float4(input.position, 1.0f);
    output.projectionPosition = mul(vertexPosition, mul(world, viewProjection));
    output.pixelPosition = mul(vertexPosition, world).xyz;
    output.pixelNormal = float3(0.0f, 0.0f, 0.0f);
    output.texcoord = input.texcoord;

    return output;
}

#endif // VERTEX_SHADER

#ifdef PIXEL_SHADER

Texture2D diffuseTexture : register(t0);
SamplerState diffuseSampler : register(s0);

cbuffer Material : register(b0)
{
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;
    int specularHardness;
    float3 diffuseColor;
    float _padding1;
    float3 specularColor;
    float _padding2;
};

struct PixelInput
{
    float4 projectionPosition : SV_POSITION; // System Value
    float3 pixelPosition: PIXEL_POSITION;
    float3 pixelNormal: PIXEL_NORMAL;
    float2 texcoord : TEXCOORD;
};

struct PixelOutput
{
    float4 diffuse : SV_Target0;  // System Value
    float4 specular : SV_Target1; // System Value
    float4 position : SV_Target2; // System Value
    float4 normal : SV_Target3;   // System Value
};

PixelOutput Main(PixelInput input)
{
    PixelOutput output;

    float3 diffuseColor = diffuseTexture.Sample(diffuseSampler, input.texcoord);
    float3 specularColor = diffuseColor;

    output.diffuse = float4(diffuseColor, ambientIntensity);
    output.specular = float4(specularColor, diffuseIntensity);
    output.position = float4(input.pixelPosition, specularIntensity);
    output.normal = float4(input.pixelNormal, specularHardness);

    return output;
}

#endif // PIXEL_SHADER
