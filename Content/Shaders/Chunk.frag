#version 330 core

in vec2 InFragTexCoord;

out vec4 FragColor;

uniform uint AtlasDomainWCount;
uniform sampler2D TexSampler;

void main()
{
    float TexMultiplier = 1.0f / float(AtlasDomainWCount);

    FragColor = texture(TexSampler, InFragTexCoord * TexMultiplier);
}
