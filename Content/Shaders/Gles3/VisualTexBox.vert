#version 330 es

in vec2 InLocation;
in vec2 InVertTexCoord;

uniform float OrthoZDepth;
uniform mat4  Projection;

out vec2 InFragTexCoord;

void main()
{
    vec4 Pos =  Projection * vec4(InLocation.xy, OrthoZDepth, 1.0);
    Pos.y = -Pos.y;
    gl_Position = Pos;

    InFragTexCoord = InVertTexCoord;
}
