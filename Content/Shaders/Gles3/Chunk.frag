#version 300 es

in mediump vec2 InFragTexCoord;
in mediump vec3 InFragNormal;
in mediump vec2 InFragBlendTexCoord;

out mediump vec4 FragColor;

uniform uint AtlasBlendOpaqueDomainWCount;
uniform uint AtlasBlendersDomainWCount;
uniform sampler2D BlendOpaqueTexSampler;
uniform sampler2D BlendersTexSampler;

const mediump vec3 Ambient = vec3(0.6);
const mediump vec3 LightNormal = vec3(0.8, 0.7, 0.5);
const mediump vec3 GrassColor = vec3(0.0, 0.8, 0.0);

void main()
{
    mediump vec3 LightDirection = normalize(-LightNormal);
    mediump float Diff = max(dot(InFragNormal, LightDirection), 0.0);
    mediump vec3 Diffuse = Diff * vec3(1);
    mediump vec4 ResultingLight = vec4(Ambient + Diffuse, 1.0);

    mediump float TexMultiplier = 1.0f / float(AtlasBlendOpaqueDomainWCount);
    mediump vec4 TexRes = (texture(BlendOpaqueTexSampler, InFragTexCoord * TexMultiplier));

    mediump float BlendTexMultiplier = 1.0f / float(AtlasBlendersDomainWCount);
    mediump vec4 BlendTexRes = texture(BlendersTexSampler, InFragBlendTexCoord * BlendTexMultiplier);
    mediump vec4 NonBlendedColor = (TexRes * (1.0f - BlendTexRes.a));
    mediump vec4 BlendedColor = (TexRes * vec4(GrassColor, 1.0f) * BlendTexRes.a);

    FragColor = (NonBlendedColor + BlendedColor) * ResultingLight;
}
