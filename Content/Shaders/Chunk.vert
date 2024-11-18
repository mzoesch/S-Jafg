#version 330 core

layout (location = 0) in vec3 InLocation;
layout (location = 1) in vec2 InVertTexCoord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec2 InFragTexCoord;

void main()
{
    vec4 Pos = Projection * View * Model * vec4(InLocation, 1.0);
    Pos.x = -Pos.x;
    gl_Position = Pos;

    InFragTexCoord = InVertTexCoord;
}
