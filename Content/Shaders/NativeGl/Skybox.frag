#version 330 core

in vec3 InFragTexCoord;

out vec4 FragColor;

uniform samplerCube SkyboxSampler0;

uniform float CubeLoad0;

void main()
{
    vec4 Color = texture(SkyboxSampler0, vec3(-InFragTexCoord.x, InFragTexCoord.z, InFragTexCoord.y)) * CubeLoad0;

    if (Color.a < 0.01)
    {
        discard;
    }

    FragColor = Color;
}
