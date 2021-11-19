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

cbuffer DynamicLight : register(b0)
{
    int dynamicLightType;
    float3 dynamicLightColor;
    int dynamicLightSpotHardness;
    float3 padding;
};

cbuffer WorldVectors : register(b1)
{
    float4 cameraPosition;
    float4 lightPosition;
    float4 lightDirection;
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
    float4 specularSample = specularTexture.Sample(geometrySampler, input.texcoord);
    float4 positionSample = positionTexture.Sample(geometrySampler, input.texcoord);
    float4 normalSample = normalTexture.Sample(geometrySampler, input.texcoord);

    // --- Calculate diffuse color

    float3 diffuseColor = diffuseSample.rgb;
    float diffuseIntensity = specularSample.a;

    float3 pixelDiffuseColor = mul(diffuseColor, diffuseIntensity);
    float3 lightDiffuseColor = mul(dynamicLightColor, 1.0f); // TODO
    float4 combinedDiffuseColor = float4(pixelDiffuseColor * lightDiffuseColor, 1.0f);

    // --- Calculate specular color

    float3 specularColor = specularSample.rgb;
    float specularIntensity = positionSample.a;

    float3 pixelSpecularColor = mul(specularColor, specularIntensity);
    float3 lightSpecularColor = mul(dynamicLightColor, 1.0f); // TODO
    float4 combinedSpecularColor = float4(pixelSpecularColor * lightSpecularColor, 1.0f);

    // --- Calculate diffuse light intensity

    float3 pixelPosition = positionSample.xyz;
    float3 pixelNormal = normalize(normalSample.xyz);

    float3 directionFromLight = normalize(dynamicLightType == 1 ? pixelPosition - lightPosition.xyz : lightDirection.xyz); // LightType::Point
    float3 directionToLight = -directionFromLight;

    float diffuseLightIntensity = dot(directionToLight, pixelNormal);
    diffuseLightIntensity = max(diffuseLightIntensity, 0.0f);

    // --- Calculate specular light intensity

    float3 dynamicLightReflection = reflect(directionFromLight, pixelNormal);
    float3 directionToCamera = normalize(cameraPosition.xyz - pixelPosition);

    int specularHardness = normalSample.a;
    float specularLightIntensity = dot(directionToCamera, dynamicLightReflection);
    specularLightIntensity = max(specularLightIntensity, 0.0f);
    specularLightIntensity = pow(specularLightIntensity, specularHardness);

    // --- Calculate spot light falloff inside the light cone (cender to border)

    if (dynamicLightType == 2) // LightType::Spot
    {
        float3 directionFromLightToPixel = normalize(pixelPosition - lightPosition.xyz);
        float spotIntensity = dot(directionFromLight, directionFromLightToPixel);
        spotIntensity = max(spotIntensity, 0.0f);
        spotIntensity = pow(spotIntensity, dynamicLightSpotHardness);

        diffuseLightIntensity *= spotIntensity;
        specularLightIntensity *= spotIntensity;
    }

    PixelOutput output;
    output.color = mul(combinedDiffuseColor, diffuseLightIntensity) + mul(combinedSpecularColor, specularLightIntensity);
    return output;
}

#endif // PIXEL_SHADER
