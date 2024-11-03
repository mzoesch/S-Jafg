#version 330 core

layout (location = 0) in vec2 InLocation;

uniform float OrthoZDepth;
uniform mat4  Projection;

void main()
{
   // vec4 Pos = Projection * vec4(aPos.x, aPos.y, -0.001, 1.0);
   vec4 Pos = Projection * vec4(InLocation.xy, OrthoZDepth, 1.0);
   Pos.y = -Pos.y;
   gl_Position = Pos;
}
