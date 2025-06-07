#version 330 core

in vec3 InFragTexCoord;

out vec4 FragColor;

uniform samplerCube SkyboxSampler;

void main()
{    
    FragColor = texture(SkyboxSampler, vec3(-InFragTexCoord.x, InFragTexCoord.z, InFragTexCoord.y));
}
