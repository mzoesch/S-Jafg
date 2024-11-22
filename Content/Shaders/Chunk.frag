#version 330 core

in vec2 InFragTexCoord;
in vec3 InFragNormal;

out vec4 FragColor;

uniform uint AtlasDomainWCount;
uniform sampler2D TexSampler;

vec3 Ambient = vec3(0.6);
vec3 LightNormal = vec3(0.8, 0.7, 0.5);

void main()
{
    vec3 LightDirection = normalize(-LightNormal);
    float Diff = max(dot(InFragNormal, LightDirection), 0.0);
    vec3 Diffuse = Diff * vec3(1);
    vec4 ResultingLight = vec4(Ambient + Diffuse, 1.0);

    float TexMultiplier = 1.0f / float(AtlasDomainWCount);
    vec4 TexRes = (texture(TexSampler, InFragTexCoord * TexMultiplier));
    FragColor = TexRes * ResultingLight;

    // FragColor = vec4(((InFragNormal + 1.0f) * 0.5f), 1.0f);
}
