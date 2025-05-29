// Copyright mzoesch. All rights reserved.

#include "Rhi/OrthographicRoundedOutlineImageBoxShader.h"
#include "System/EnginePath.h"
#include "Rhi/RhiVendorInclude.h"
#include "Widgets/Viewport.h"

bool Jafg::LOrthographicRoundedOutlineImageBoxShader::Make(const LName InName)
{
    if (const bool bOut = Super::Make(InName); bOut == false)
    {
        return false;
    }

    LOG_VERBOSE(LogRhi, "Creating LOrthographicRoundedOutlineBoxShader at [{}].", InName)

    this->Program = LShader(LEnginePath(EEnginePaths::Shaders, "VisualBox"),
    {
        {
            "WITH_UV", "1"
        },
        {
            "WITH_BOX_SIZE", "1"
        },
        {
            "WITH_OUTLINE", "1"
        },
        {
            "WITH_RADII", "1"
        },
    });

    this->Program.Use();

    glGenVertexArrays(1, &this->Vao);
    glGenBuffers(1, &this->Vbo);

    glBindVertexArray(this->Vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return true;
}

void Jafg::LOrthographicRoundedOutlineImageBoxShader::UpdateViewportUniforms(const LViewport& Context)
{
    Super::UpdateViewportUniforms(Context);

    const LIntVector2 WindowDimensions = Context.GetDimensions();
    const LMatrix Projection = Maths::MakeOrthographicProjectionMatrix
    (
        LVector2(static_cast<f32>(WindowDimensions.X), static_cast<f32>(WindowDimensions.Y))
    );

    this->Program.Use();
    this->Program.SetMatrixUniform("Projection", Projection);

    return;
}

void Jafg::LOrthographicRoundedOutlineImageBoxShader::OnFree()
{
    glDeleteVertexArrays(1, &this->Vao);
    glDeleteBuffers(1, &this->Vbo);

#if WITH_DEBUG_ZERO_UNBOUND
    this->Vao = 0;
    this->Vbo = 0;
#endif /* WITH_DEBUG_ZERO_UNBOUND */

    Super::OnFree();

    return;
}

void Jafg::LOrthographicRoundedOutlineImageBoxShader::Draw
(
    const LViewport& Context,
    const LVector2&  Size,
    const LVector2&  TopLeft,
    const LColor&    Tint,
    const f32        OutlineThickness,
    const LColor&    OutlineTint,
    const LVector4&  Radii
) const
{
    check( this->IsMeaningful() )

    if (Size.X <= 0.0f || Size.Y <= 0.0f)
    {
        LOG_WARNING(LogRhi, "Invalid Size [x{}, y{}].", Size.X, Size.Y)
        return;
    }

    this->Program.Use();
    this->Program.SetFloatUniform("OrthoZDepth", Context.GetFrameOrthoZLayerDepth());
    this->Program.SetVec2Uniform("BoxSize", Size);
    this->Program.SetColorUniform("BoxTint", Tint);
    this->Program.SetVec4Uniform("Radii", Radii);
    this->Program.SetColorUniform("OutlineTint", OutlineTint);
    this->Program.SetFloatUniform("OutlineThickness", OutlineThickness);

    const f32 Scale = Context.GetScaleFactor();
    f32 Vertices[]
    {
        TopLeft.X * Scale,            TopLeft.Y * Scale,            0.0f, 0.0f, /* Top    Left  */
        (TopLeft.X + Size.X) * Scale, TopLeft.Y * Scale,            1.0f, 0.0f, /* Top    Right */
        TopLeft.X * Scale,            (TopLeft.Y + Size.Y) * Scale, 0.0f, 1.0f, /* Bottom Left  */

        (TopLeft.X + Size.X) * Scale, TopLeft.Y * Scale,            1.0f, 0.0f, /* Top    Right */
        (TopLeft.X + Size.X) * Scale, (TopLeft.Y + Size.Y) * Scale, 1.0f, 1.0f, /* Bottom Right */
        TopLeft.X * Scale,            (TopLeft.Y + Size.Y) * Scale, 0.0f, 1.0f, /* Bottom Left  */
    };
    Vertices[0]  = Maths::Floor(Vertices[0]);  Vertices[1]  = Maths::Floor(Vertices[1]);
    Vertices[4]  = Maths::Floor(Vertices[4]);  Vertices[5]  = Maths::Floor(Vertices[5]);
    Vertices[8]  = Maths::Floor(Vertices[8]);  Vertices[9]  = Maths::Floor(Vertices[9]);
    Vertices[12] = Maths::Floor(Vertices[12]); Vertices[13] = Maths::Floor(Vertices[13]);
    Vertices[16] = Maths::Floor(Vertices[16]); Vertices[17] = Maths::Floor(Vertices[17]);
    Vertices[20] = Maths::Floor(Vertices[20]); Vertices[21] = Maths::Floor(Vertices[21]);

    glBindVertexArray(this->Vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);

#if WITH_DEBUG_ZERO_UNBOUND
    this->Program.Unuse();
    glBindVertexArray(0);
#endif /* WITH_DEBUG_ZERO_UNBOUND */

    return;
}
