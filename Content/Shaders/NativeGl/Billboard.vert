#version 330 core

layout(location = 0) in vec2 InLocation;
layout(location = 1) in vec2 InUv;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec2 TexCoords;

void main()
{
    TexCoords = InUv;

    vec4 Pos = Projection * View * Model * vec4(InLocation, 0.0, 1.0);
    Pos.x = -Pos.x;
    gl_Position = Pos;
}
