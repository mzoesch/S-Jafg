#version 330 core

layout (location = 0) in vec3 InLocation;

out vec3 InFragTexCoord;

uniform mat4 Projection;
uniform mat4 View;

void main()
{
    // vec3 InLocation2 = vec3(InLocation.x, InLocation.z, InLocation.y);

    vec3 InLocation2 = InLocation;

    InFragTexCoord = InLocation2;
    vec4 pos = Projection * View * vec4(InLocation2, 1.0);
    pos.x = -pos.x;
    gl_Position = pos.xyww;
}
