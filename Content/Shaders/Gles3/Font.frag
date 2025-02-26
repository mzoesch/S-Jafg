#version 300 es

precision highp float;

in  vec2 TexCoords;

out vec4 FragColor;

uniform vec3 Color;
uniform sampler2D Text;

void main()
{    
    vec4 Sampled = vec4(1.0, 1.0, 1.0, texture(Text, TexCoords).r);
    FragColor = vec4(Color, 1.0) * Sampled;
}
