#version 330 core

layout (location = 0) in vec2 InLocation;
layout (location = 1) in vec2 InVertTexCoord;

uniform float OrthoZDepth;

out vec2 InFragTexCoord;

void main()
{
    gl_Position = vec4(InLocation.xy, OrthoZDepth, 1.0);
    InFragTexCoord = InVertTexCoord;
}
