// Copyright mzoesch. All rights reserved.

#include "Rhi/OrthographicBoxShader.h"
#include "System/EnginePath.h"
#include "Rhi/RhiVendorInclude.h"
#include "Widgets/Viewport.h"

TArray<Jafg::LShaderCompileTimeConstant> Jafg::LOrthographicBoxShader::GetDefaultConstants()
{
    return Super::GetDefaultConstants();
}

bool Jafg::LOrthographicBoxShader::Make(const LName InName, TArray<LShaderCompileTimeConstant>&& InConstants /* = {} */)
{
    if (const bool bOut = Super::Make(InName, std::move(InConstants)); bOut == false)
    {
        return false;
    }

    LOG_VERBOSE(LogRhi, "Creating OrthographicBoxShader at [{}].", InName)

    this->Program = LShader(LEnginePath(EEnginePaths::Shaders, "VisualBox"), this->Constants);
    this->Program.Use();

    glGenVertexArrays(1, &this->Vao);
    glGenBuffers(1, &this->Vbo);

    glBindVertexArray(this->Vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    return true;
}

void Jafg::LOrthographicBoxShader::UpdateViewportUniforms(const LViewport& Context)
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

void Jafg::LOrthographicBoxShader::OnFree()
{
    glDeleteVertexArrays(1, &this->Vao);
    glDeleteBuffers(1, &this->Vbo);

    Super::OnFree();

    return;
}

void Jafg::LOrthographicBoxShader::Draw
(
    const LViewport& Context,
    const LVector2&  Size,
    const LVector2&  TopLeft,
    const LColor&    Tint
) const
{
    check( this->IsValid() )

    if (Size.X <= 0.0f || Size.Y <= 0.0f)
    {
        LOG_WARNING(LogRhi, "Invalid Size [x{}, y{}].", Size.X, Size.Y)
        return;
    }

    this->Program.Use();
    this->Program.SetFloatUniform("OrthoZDepth", Context.GetFrameOrthoZLayerDepth());
    this->Program.SetColorUniform("BoxTint", Tint);

    const f32 Scale = Context.GetScaleFactor();
    f32 Vertices[]
    {
        TopLeft.X * Scale,            TopLeft.Y * Scale,            /* Top    Left  */
        (TopLeft.X + Size.X) * Scale, TopLeft.Y * Scale,            /* Top    Right */
        TopLeft.X * Scale,            (TopLeft.Y + Size.Y) * Scale, /* Bottom Left  */

        (TopLeft.X + Size.X) * Scale, TopLeft.Y * Scale,            /* Top    Right */
        (TopLeft.X + Size.X) * Scale, (TopLeft.Y + Size.Y) * Scale, /* Bottom Right */
        TopLeft.X * Scale,            (TopLeft.Y + Size.Y) * Scale  /* Bottom Left  */
    };
    for (f32& Vertex : Vertices) { Vertex = Maths::Floor(Vertex); }

    glBindVertexArray(this->Vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    return;
}

