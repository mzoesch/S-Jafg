#version 300 es

in mediump vec2 InFragTexCoord;

out mediump vec4 FragColor;

// 32-bit unsigned integer representing the color of the box
// The color is stored in the format 0xBBGGRRAA
uniform int BoxColor;
uniform sampler2D TexSampler;

void main()
{
    // Extract the color components from the 32-bit unsigned integer
    mediump float A = float((BoxColor >> 24) & 0xFF) / 255.0;
    mediump float R = float((BoxColor >> 16) & 0xFF) / 255.0;
    mediump float G = float((BoxColor >>  8) & 0xFF) / 255.0;
    mediump float B = float((BoxColor >>  0) & 0xFF) / 255.0;

    FragColor = texture(TexSampler, InFragTexCoord) * vec4(R, G, B, A);
}
