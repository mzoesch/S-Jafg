#version 300 es

precision highp float;

in vec2 InFragTexCoord;

out vec4 FragColor;

uniform sampler2D TexSampler;

void main()
{
    FragColor = texture(TexSampler, InFragTexCoord);
}
