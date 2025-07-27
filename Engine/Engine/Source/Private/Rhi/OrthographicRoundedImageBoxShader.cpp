// Copyright mzoesch. All rights reserved.

#include "Rhi/OrthographicRoundedImageBoxShader.h"
#include "System/EnginePath.h"
#include "Rhi/RhiVendorInclude.h"
#include "Widgets/Viewport.h"
#include "Widgets/RegionForward.h"

TArray<Jafg::LShaderCompileTimeConstant> Jafg::LOrthographicRoundedImageBoxShader::GetDefaultConstants()
{
    TArray<LShaderCompileTimeConstant> SuperConstants = Super::GetDefaultConstants();

    SuperConstants.Append
    (TArray<LShaderCompileTimeConstant>{
        LShaderCompileTimeConstant{
            "WITH_UV", "1"
        },
        LShaderCompileTimeConstant{
            "WITH_TEXTURE", "1"
        },
        LShaderCompileTimeConstant{
            "WITH_BOX_SIZE", "1"
        },
        LShaderCompileTimeConstant{
            "WITH_RADII", "1"
        },
    });

    return SuperConstants;}

bool Jafg::LOrthographicRoundedImageBoxShader::Make(const LName InName, TArray<LShaderCompileTimeConstant>&& InConstants /* = {} */)
{
    if (const bool bOut = Super::Make(InName, std::move(InConstants)); bOut == false)
    {
        return false;
    }

    LOG_VERBOSE(LogRhi, "Creating OrthographicRoundedImageBoxShader at [{}].", InName)

    this->Program = LShader(LEnginePath(EEnginePaths::Shaders, "VisualBox"), this->Constants);

    this->Program.Use();

    glGenVertexArrays(1, &this->Vao);
    glGenBuffers(1, &this->Vbo);

    glBindVertexArray(this->Vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return true;
}

void Jafg::LOrthographicRoundedImageBoxShader::UpdateViewportUniforms(const LViewport& Context)
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

void Jafg::LOrthographicRoundedImageBoxShader::OnFree()
{
    glDeleteVertexArrays(1, &this->Vao);
    glDeleteBuffers(1, &this->Vbo);

    Super::OnFree();

    return;
}

void Jafg::LOrthographicRoundedImageBoxShader::Draw
(
    const LViewport&           Context,
    const LVector2&            Size,
    const LVector2&            TopLeft,
    const LColor&              Tint,
    const LVector4&            Radii,
    const LImage&              Image,
    const LColor&              ImageTint,
    const f32                  ImageScale,
    const EImageBehavior::Type ImageBehavior,
    const EImageOobm::Type     ImageOobm,
    const f32                  ImagePadding
) const
{
    check( this->IsValid() )

    if (Size.X <= 0.0f || Size.Y <= 0.0f)
    {
        LOG_WARNING(LogRhi, "Invalid Size [x{}, y{}].", Size.X, Size.Y)
        return;
    }

    const Lu32Vector2 ImageSize = Image.GetTexture()->GetSize();

    this->Program.Use();
    this->Program.SetFloatUniform("OrthoZDepth", Context.GetFrameOrthoZLayerDepth());
    this->Program.SetVec2Uniform("BoxSize", Size);
    this->Program.SetColorUniform("BoxTint", Tint);
    this->Program.SetVec4Uniform("Radii", Radii);
    this->Program.SetVec2Uniform("TexSize", {static_cast<f32>(ImageSize.X), static_cast<f32>(ImageSize.Y)});
    this->Program.SetFloatUniform("TexScale", ImageScale);
    this->Program.SetColorUniform("ImageTint", ImageTint);
    this->Program.SetBoolUniform("bImageAspect", ImageBehavior == EImageBehavior::Aspect);
    this->Program.SetIntUniform("ImageOobm", ImageOobm);
    this->Program.SetFloatUniform("TexPadding", ImagePadding);

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

    glBindTexture(GL_TEXTURE_2D, Image.GetTextureHandle());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    return;
}
