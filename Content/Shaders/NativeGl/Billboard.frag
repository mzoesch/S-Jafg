#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D Sampler;

void main()
{
    vec4 Sampled = texture(Sampler, TexCoords);

    if (Sampled.a < 0.01)
    {
        discard; 
    } 

    FragColor = Sampled;
}
