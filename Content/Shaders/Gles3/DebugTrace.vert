#version 300 es

precision highp float;

layout (location = 0) in vec3 InLocation;

uniform mat4 View;
uniform mat4 Projection;

void main()
{
    vec4 Pos = Projection * View * vec4(InLocation.xyz, 1.0);
    Pos.x = -Pos.x;
    gl_Position = Pos;
}
