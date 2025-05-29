#version 330 core

#if WITH_UV
   in vec2 InFragUv; 
#endif /* WITH_UV */

out vec4 FragColor;

// 32-bit unsigned integer representing the color of the box
// The color is stored in the format 0xBBGGRRAA
uniform int BoxTint;

#if WITH_TEXTURE
    uniform sampler2D TexSampler;
#endif /* WITH_TEXTURE */

#if WITH_BOX_SIZE
    // Screen space location (x,y); then width and height (z,w).
    uniform vec2 BoxSize;
#endif /* WITH_BOX_SIZE */

#if WITH_OUTLINE
    // In px.
    uniform float OutlineThickness;
    // @see BoxTint.
    uniform int OutlineTint;
#endif /* WITH_OUTLINE */

#if WITH_RADII
    // The radius of the corners (TL, TR, BR, BL). In px.
    uniform vec4 Radii;
#endif /* WITH_RADII */

#define GET_COLOR_FROM_INT_NO_TEXTURE(color)         \
    vec4                                             \
    (                                                \
        float((color >> 16) & 0xFF) / 255.0, /* R */ \
        float((color >>  8) & 0xFF) / 255.0, /* G */ \
        float((color >>  0) & 0xFF) / 255.0, /* B */ \
        float((color >> 24) & 0xFF) / 255.0  /* A */ \
    )

#if WITH_TEXTURE
    #define GET_COLOR_FROM_INT(color) texture(TexSampler, InFragUv) * GET_COLOR_FROM_INT_NO_TEXTURE(color)
#else /* WITH_TEXTURE */
    #define GET_COLOR_FROM_INT(color) GET_COLOR_FROM_INT_NO_TEXTURE(color)
#endif /* !WITH_TEXTURE */

void main()
{
#if WITH_OUTLINE || WITH_RADII

    float DistLeft   = (InFragUv.x - 0.0) * BoxSize.x;
    float DistTop    = (InFragUv.y - 0.0) * BoxSize.y;
    float DistRight  = (1.0 - InFragUv.x) * BoxSize.x;
    float DistBottom = (1.0 - InFragUv.y) * BoxSize.y;

#endif /* WITH_OUTLINE || WITH_RADII */

#if WITH_OUTLINE
 
    #if WITH_RADII

        bool bCorner  = false;
        bool bOutline = false;

        // Top-left corner
        if (DistLeft < Radii.x && DistTop < Radii.x) 
        {
            float dist = length(vec2(Radii.x - DistLeft, Radii.x - DistTop));
            if (dist > Radii.x)
            {
                discard;
            }
            else if (dist > Radii.x - OutlineThickness)
            {
                bOutline = true;
            }
            bCorner = true;
        }

        // Top-right corner
        else if (DistRight < Radii.y && DistTop < Radii.y) 
        {
            float dist = length(vec2(Radii.y - DistRight, Radii.y - DistTop));
            if (dist > Radii.y)
            {
                discard;
            }
            else if (dist > Radii.y - OutlineThickness)
            {
                bOutline = true;
            }
            bCorner = true;
        }

        // Bottom-right corner
        else if (DistRight < Radii.z && DistBottom < Radii.z) 
        {
            float dist = length(vec2(Radii.z - DistRight, Radii.z - DistBottom));
            if (dist > Radii.z)
            {
                discard;
            }
            else if (dist > Radii.z - OutlineThickness)
            {
                bOutline = true;
            }
            bCorner = true;
        }

        // Bottom-left corner
        else if (DistLeft < Radii.w && DistBottom < Radii.w) 
        {
            float dist = length(vec2(Radii.w - DistLeft, Radii.w - DistBottom));
            if (dist > Radii.w)
            {
                discard;
            }
            else if (dist > Radii.w - OutlineThickness)
            {
                bOutline = true;
            }
            bCorner = true;
        }

        if (!bCorner) 
        {
            if 
            (
                   DistLeft   < OutlineThickness 
                || DistRight  < OutlineThickness 
                || DistTop    < OutlineThickness 
                || DistBottom < OutlineThickness
            )
            {
                bOutline = true;
            }
        }

        if (bOutline) 
        {
            FragColor = GET_COLOR_FROM_INT_NO_TEXTURE(OutlineTint);
        } 
        else
        {
            FragColor = GET_COLOR_FROM_INT(BoxTint);
        }

    #else /* WITH_RADII */

        if
        (
               DistLeft   < OutlineThickness 
            || DistTop    < OutlineThickness
            || DistRight  < OutlineThickness
            || DistBottom < OutlineThickness 
        ) 
        {
            FragColor = GET_COLOR_FROM_INT_NO_TEXTURE(OutlineTint);
        }
        else
        {
            FragColor = GET_COLOR_FROM_INT(BoxTint);
        }

    #endif /* !WITH_RADII */

#else /* WITH_OUTLINE */

    #if WITH_RADII

        // Top-left corner
        if (DistLeft < Radii.x && DistTop < Radii.x) 
        {
            float dist = length(vec2(Radii.x - DistLeft, Radii.x - DistTop));
            if (dist > Radii.x)
            {
                discard;
            }
        }

        // Top-right corner
        else if (DistRight < Radii.y && DistTop < Radii.y) 
        {
            float dist = length(vec2(Radii.y - DistRight, Radii.y - DistTop));
            if (dist > Radii.y)
            {
                discard;
            }
        }

        // Bottom-right corner
        else if (DistRight < Radii.z && DistBottom < Radii.z) 
        {
            float dist = length(vec2(Radii.z - DistRight, Radii.z - DistBottom));
            if (dist > Radii.z)
            {
                discard;
            }
        }

        // Bottom-left corner
        else if (DistLeft < Radii.w && DistBottom < Radii.w) 
        {
            float dist = length(vec2(Radii.w - DistLeft, Radii.w - DistBottom));
            if (dist > Radii.w)
            {
                discard;
            }
        }

    #endif /* !WITH_RADII */

    FragColor = GET_COLOR_FROM_INT(BoxTint);

#endif /* !WITH_OUTLINE */
}
