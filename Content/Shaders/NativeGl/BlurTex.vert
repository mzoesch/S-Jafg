#version 330 core

layout (location = 0) in vec2 InLocation;

uniform float OrthoZDepth;
uniform mat4  Projection;

out vec2 fragmentTexCoord;

void main()
{
   vec4 Pos = Projection * vec4(InLocation.xy, OrthoZDepth, 1.0);
   Pos.y = -Pos.y;
   gl_Position = Pos;

   fragmentTexCoord = (Pos.xy + vec2(1.0)) * 0.5;
}
