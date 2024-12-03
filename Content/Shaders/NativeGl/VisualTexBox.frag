#version 330 core

in vec2 InFragTexCoord;

out vec4 FragColor;

// 32-bit unsigned integer representing the color of the box
// The color is stored in the format 0xBBGGRRAA
uniform int BoxColor;
uniform sampler2D TexSampler;

void main()
{
    // Extract the color components from the 32-bit unsigned integer
    float A = float((BoxColor >> 24) & 0xFF) / 255.0;
    float R = float((BoxColor >> 16) & 0xFF) / 255.0;
    float G = float((BoxColor >>  8) & 0xFF) / 255.0;
    float B = float((BoxColor >>  0) & 0xFF) / 255.0;

    FragColor = texture(TexSampler, InFragTexCoord) * vec4(R, G, B, A);
}
