#version 330 core

in vec2 InFragTexCoord;

out vec4 FragColor;

uniform sampler2D TexSampler;

void main()
{
    FragColor = texture(TexSampler, InFragTexCoord);
}
