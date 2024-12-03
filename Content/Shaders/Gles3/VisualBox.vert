#version 300 es

in vec2 InLocation;

uniform float OrthoZDepth;
uniform mat4  Projection;

void main()
{
   vec4 Pos = Projection * vec4(InLocation.xy, OrthoZDepth, 1.0);
   Pos.y = -Pos.y;
   gl_Position = Pos;
}
