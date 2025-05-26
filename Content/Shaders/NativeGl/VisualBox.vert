#version 330 core

layout (location = 0) in vec2 InLocation;

#if WITH_UV
   layout (location = 1) in vec2 InUv;
#endif /* WITH_UV */

uniform float OrthoZDepth;
uniform mat4  Projection;

#if WITH_UV
   out vec2 InFragUv; 
#endif /* WITH_UV */

void main()
{
#if WITH_UV
   InFragUv = InUv;
#endif /* WITH_UV */

   vec4 Pos = Projection * vec4(InLocation.xy, OrthoZDepth, 1.0);
   Pos.y = -Pos.y;
   gl_Position = Pos;
}
