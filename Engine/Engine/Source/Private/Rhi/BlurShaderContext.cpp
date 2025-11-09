// Copyright mzoesch. All rights reserved.

#include "Rhi/BlurShaderContext.h"
#include "System/EnginePath.h"
#include "Rhi/RhiVendorInclude.h"
#include "Widgets/Viewport.h"

#if 0

void Jafg::LBlurShaderContext::Make()
{
    LGenericShaderContext::Make();

    this->Shader = LShader(LEnginePath(EEnginePaths::Shaders, "BlurTex"));
    this->Shader.Use();

    glGenVertexArrays(1, &this->Vao);
    glBindVertexArray(this->Vao);
    glGenBuffers(1, &this->Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    return;
}

void Jafg::LBlurShaderContext::Draw(const LViewport& Context, LGenericShaderContextDrawArgs& InArgs) const
{
    GENERIC_SHADER_DRAW_BODY(LBlurShaderContextDrawArgs)

    check( Args.Size.X > 0.0f )
    check( Args.Size.Y > 0.0f )

    if (static_cast<float>(Context.GetDimensions().X) < Args.TopLeft.X + Args.Size.X)
    {
        LOG_ERROR(LogWidgetFramework, "BlurBox X-Axis constraint: {} < {} + {}.", Context.GetDimensions().X, Args.TopLeft.X, Args.Size.X)
    }
    if (static_cast<float>(Context.GetDimensions().Y) < Args.TopLeft.Y + Args.Size.Y)
    {
        LOG_ERROR(LogWidgetFramework, "BlurBox Y-Axis constraint: {} < {} + {}.", Context.GetDimensions().Y, Args.TopLeft.Y, Args.Size.Y)
    }

    const LVector2 Size = Args.Size;
    const LVector2 TopLeft = Args.TopLeft;

    this->Shader.Use();
    glBindVertexArray(this->Vao);

    const f32 Scale { Context.GetScaleFactor() };
    const LIntVector2 WindowDimensions = Context.GetDimensions();
    const LMatrix Projection = Maths::MakeOrthographicProjectionMatrix(
        LVector2{static_cast<f32>(WindowDimensions.X), static_cast<f32>(WindowDimensions.Y)}
    );

    this->Shader.SetMatrixUniform("Projection", Projection);
    this->Shader.SetFloatUniform("OrthoZDepth", Context.GetFrameOrthoZLayerDepth());
    this->Shader.SetFloatUniform("BlurStrength", Args.Strength);

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

    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_DYNAMIC_DRAW);

    check( Context.IsIntermediateBufferValid() )
    Context.GetIntermediateBuffer().ReadToActive();

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);

    return;
}

void Jafg::LBlurShaderContext::OnFree()
{
    LGenericShaderContext::OnFree();

    glDeleteVertexArrays(1, &this->Vao);
    glDeleteBuffers(1, &this->Vbo);

    this->Vao = 0;
    this->Vbo = 0;
    this->Shader.Free();

    return;
}

#endif
