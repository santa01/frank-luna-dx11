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

#define LIGHT_DIRECTION 0
#define LIGHT_POINT     1
#define LIGHT_SPOT      2

Texture2D diffuseTexture : register(t0);
Texture2D specularTexture : register(t1);
Texture2D positionTexture : register(t2);
Texture2D normalTexture : register(t3);
SamplerState geometrySampler : register(s0);

cbuffer DynamicLight : register(b0)
{
    int dynamicLightType;
    float dynamicLightFalloff;
    float dynamicLightSpotAngle;
    float dynamicLightSpotBorder;
    float3 dynamicLightColor;
    float dynamicLightPadding;
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

    float3 directionFromLight = normalize(dynamicLightType == LIGHT_POINT ? pixelPosition - lightPosition.xyz : lightDirection.xyz);
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

    // --- Calculate spot light falloff inside the light cone

    if (dynamicLightType == LIGHT_SPOT)
    {
        float3 directionFromLightToPixel = normalize(pixelPosition - lightPosition.xyz);
        float dynamicLightAngleCos = dot(directionFromLight, directionFromLightToPixel);
        dynamicLightAngleCos = max(dynamicLightAngleCos, 0.0f);

        float dynamicLightHardAngleCos = cos(dynamicLightSpotAngle * (1.0f - dynamicLightSpotBorder));
        float dynamicLightSoftAngleCos = cos(dynamicLightSpotAngle);
        float spotIntensity = (dynamicLightSoftAngleCos - dynamicLightAngleCos) / (dynamicLightSoftAngleCos - dynamicLightHardAngleCos);
        spotIntensity = saturate(spotIntensity);

        diffuseLightIntensity *= spotIntensity;
        specularLightIntensity *= spotIntensity;
    }

    // --- Calculate light energy falloff

    if (dynamicLightType != LIGHT_DIRECTION)
    {
        float dynamicLightFalloffSquare = pow(dynamicLightFalloff, 2);
        float lightDistanceFalloffSquare = pow(distance(pixelPosition, lightPosition.xyz), 2);
        float lightFalloff = dynamicLightFalloffSquare / (dynamicLightFalloffSquare + lightDistanceFalloffSquare);

        diffuseLightIntensity *= lightFalloff;
        specularLightIntensity *= lightFalloff;
    }

    PixelOutput output;
    output.color = mul(combinedDiffuseColor, diffuseLightIntensity) + mul(combinedSpecularColor, specularLightIntensity);
    return output;
}

#endif // PIXEL_SHADER
