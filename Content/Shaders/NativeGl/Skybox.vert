#version 330 core

layout (location = 0) in vec3 InLocation;

out vec3 InFragTexCoord;

uniform mat4 Projection;
uniform mat4 View;

void main()
{
    InFragTexCoord = InLocation;

    vec4 pos = Projection * View * vec4(InLocation, 1.0);
    pos.x = -pos.x;
    gl_Position = pos.xyww;
}
