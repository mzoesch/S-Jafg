#version 300 es

precision highp float;

out vec4 FragColor;

// 32-bit unsigned integer representing the color of the box
// The color is stored in the format 0xBBGGRRAA
uniform int Color;

void main()
{
    float A = float((Color >> 24) & 0xFF) / 255.0;
    float R = float((Color >> 16) & 0xFF) / 255.0;
    float G = float((Color >>  8) & 0xFF) / 255.0;
    float B = float((Color >>  0) & 0xFF) / 255.0;

    FragColor = vec4(R, G, B, A);
}
