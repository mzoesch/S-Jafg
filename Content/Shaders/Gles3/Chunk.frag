#version 300 es

precision highp float;

in vec2 InFragTexCoord;
in vec3 InFragNormal;
in vec2 InFragBlendTexCoord;

out vec4 FragColor;

uniform uint AtlasBlendOpaqueDomainWCount;
uniform uint AtlasBlendersDomainWCount;
uniform sampler2D BlendOpaqueTexSampler;
uniform sampler2D BlendersTexSampler;

const vec3 Ambient = vec3(0.6);
const vec3 LightNormal = vec3(0.8, 0.7, 0.5);
const vec3 GrassColor = vec3(0.0, 0.8, 0.0);

void main()
{
    vec3 LightDirection = normalize(-LightNormal);
    float Diff = max(dot(InFragNormal, LightDirection), 0.0);
    vec3 Diffuse = Diff * vec3(1);
    vec4 ResultingLight = vec4(Ambient + Diffuse, 1.0);

    float TexMultiplier = 1.0f / float(AtlasBlendOpaqueDomainWCount);
    vec4 TexRes = (texture(BlendOpaqueTexSampler, InFragTexCoord * TexMultiplier));

    float BlendTexMultiplier = 1.0f / float(AtlasBlendersDomainWCount);
    vec4 BlendTexRes = texture(BlendersTexSampler, InFragBlendTexCoord * BlendTexMultiplier);
    vec4 NonBlendedColor = (TexRes * (1.0f - BlendTexRes.a));
    vec4 BlendedColor = (TexRes * vec4(GrassColor, 1.0f) * BlendTexRes.a);

    FragColor = (NonBlendedColor + BlendedColor) * ResultingLight;
}
