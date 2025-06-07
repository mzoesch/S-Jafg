#version 330 core

layout (location = 0) in vec3 InLocation;
layout (location = 1) in vec2 InVertTexCoord;
layout (location = 2) in int  InNormal;
layout (location = 3) in vec2 InVertBlendTexCoord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec2 InFragTexCoord;
out vec3 InFragNormal;
out vec2 InFragBlendTexCoord;

const vec3 Normals[] = vec3[]
(
    vec3( 0,  0,  1), // Up vector
    vec3( 0,  0, -1), // Down vector
    vec3( 1,  0,  0), // Forward vector
    vec3(-1,  0,  0), // Backward vector
    vec3( 0,  1,  0), // Right vector
    vec3( 0, -1,  0)  // Left vector
);

void main()
{
    InFragTexCoord = InVertTexCoord;
    InFragNormal = Normals[InNormal];
    InFragBlendTexCoord = InVertBlendTexCoord;

    vec4 Pos = Projection * View * Model * vec4(InLocation, 1.0);
    Pos.x = -Pos.x;
    gl_Position = Pos;
}
