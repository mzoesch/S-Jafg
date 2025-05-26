#version 300 es

precision highp float;

out vec4 FragColor;

// 32-bit unsigned integer representing the color of the box
// The color is stored in the format 0xBBGGRRAA
uniform int BoxTint;

void main()
{
      // Extract the color components from the 32-bit unsigned integer
      float A = float((BoxTint >> 24) & 0xFF) / 255.0;
      float R = float((BoxTint >> 16) & 0xFF) / 255.0;
      float G = float((BoxTint >>  8) & 0xFF) / 255.0;
      float B = float((BoxTint >>  0) & 0xFF) / 255.0;

      // Output the color of the box
      FragColor = vec4(R, G, B, A);
}
