#version 330 core

in vec2 fragmentTexCoord;
out vec4 FragColor;

uniform float BlurStrength;
uniform sampler2D TexSampler;

void main()
{
    float Kernel = mix(3.0f, 19.0f, BlurStrength);
    int Half = int(Kernel * 0.5f);
    float Coefficient = 1.0f / (Kernel * Kernel);

    vec2 Offset = vec2(0.002, 0.002) * BlurStrength;

    vec4 Out = vec4(0.0);
    for (int x = -Half; x <= Half; ++x)
    {
        for (int y = -Half; y <= Half; ++y)
        {
            Out += Coefficient * texture(
                TexSampler,
                fragmentTexCoord + vec2(x, y) * Offset
            );
        }
    }

    FragColor = vec4(Out.xyz, 1);
}
