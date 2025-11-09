// Copyright mzoesch. All rights reserved.

#include "Framework/Skybox.h"
#include "Engine/Engine.h"
#include "Rhi/RhiVendorInclude.h"
#include "Framework/Eye.h"
#include "Rhi/Texture2.h"
#include "Stats/Stats.h"

#if 0

namespace
{

constexpr f32 Vertices[]
{
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

f32 BillboardVertices[] = {
    // pos        // tex
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,
 };

unsigned i32 BillboardIndices[] = { 0, 1, 2, 2, 3, 0 };

} /* ~Namespace Jafg */

Jafg::LSkybox::LSkybox(const LString& DefaultName, const TArray<LEnginePath>& InDefaultSkybox)
{
    this->Maps.emplace_back(DefaultName, LCubemap{InDefaultSkybox});
    return;
}

Jafg::LSkybox::LSkybox(const TArray<LLevelSkyboxMap>& InDefaultSkybox)
{
    for (const auto& [Identifier, Textures, DefaultLoad] : InDefaultSkybox)
    {
        this->Maps.emplace_back(Identifier, LCubemap{Textures}, DefaultLoad);
        continue;
    }

    return;
}

void Jafg::LSkybox::Upload()
{
    STAT_CYCLE_FUNCTION()

    LOG_VERBOSE(LogRhi, "Uploading new skybox.")

    check( this->Vao.has_value() == false && this->Vbo.has_value() == false )

    this->Shader = LShader{{EEnginePaths::Shaders, "Skybox"}};
    this->BillboardShader = LShader{{EEnginePaths::Shaders, "Billboard"}};

    this->Vao.emplace(0);
    glGenVertexArrays(1, &this->Vao.value());
    this->Vbo.emplace(0);
    glGenBuffers(1, &this->Vbo.value());

    glBindVertexArray(this->Vao.value());
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo.value());

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), &Vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), static_cast<void*>(nullptr));

    glBindVertexArray(0);

    for (LLoadedCubemap& LoadedMap : this->Maps)
    {
        LoadedMap.Map.Reload();
    }

    this->Shader.Use();
    this->Shader.SetIntUniform("SkyboxSampler0", 0);

    this->BillboardVao.emplace(0);
    glGenVertexArrays(1, &this->BillboardVao.value());
    this->BillboardVbo.emplace(0);
    glGenBuffers(1, &this->BillboardVbo.value());
    this->BillboardEbo.emplace(0);
    glGenBuffers(1, &this->BillboardEbo.value());

    glBindVertexArray(this->BillboardVao.value());
    glBindBuffer(GL_ARRAY_BUFFER, this->BillboardVbo.value());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->BillboardEbo.value());

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), reinterpret_cast<void*>(2 * sizeof(f32)));
    glEnableVertexAttribArray(1);

    glBufferData
    (
        GL_ARRAY_BUFFER,
        sizeof(::BillboardVertices),
        &::BillboardVertices,
        GL_STATIC_DRAW
    );

    glBufferData
    (
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(::BillboardIndices),
        &::BillboardIndices,
        GL_STATIC_DRAW
    );

    glBindVertexArray(0);

    this->BillboardShader.Use();
    this->BillboardShader.SetIntUniform("Sampler", 0);

    return;
}

void Jafg::LSkybox::Draw(const LViewport& InViewport, const LEye& InEye) const
{
    glBindVertexArray(this->Vao.value());
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo.value());

    this->Shader.Use();

    const LMatrix4 Projection { Maths::MakePerspectiveProjectionMatrix
    (
        Maths::ToRadians(InEye.GetDegYFov()),
        InViewport.GetWidthF() / InViewport.GetHeightF(),
        //
        // Do not use the near and fare z planes defined in the user preferences, as we are looking at a unit
        // cube with a translation of zero.
        //
        0.1f, 10.0f
    ) };
    this->Shader.SetMatrixUniform("Projection", Projection);

    LMatrix View { InEye.GetViewMatrix() };
    View.Matrix[3][0] = 0.0f; /* X Translation. */
    View.Matrix[3][1] = 0.0f; /* Y Translation. */
    View.Matrix[3][2] = 0.0f; /* Z Translation. */
    View.Matrix[3][3] = 1.0f; /* Preserve unit. */
    this->Shader.SetMatrixUniform("View", View);

    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);

    for (TArray<LLoadedCubemap>::size_type Idx { 0 }; Idx < this->Maps.size(); ++Idx)
    {
        const LLoadedCubemap& Map { this->Maps[Idx] };

        glActiveTexture(GL_TEXTURE0 + Idx);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Map.Map);

        this->Shader.SetFloatUniform(Lal::SprintF("SkyboxSampler{}", Idx), Map.Load);

        continue;
    }

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    glActiveTexture(GL_TEXTURE0);

    this->BillboardShader.Use();
    this->BillboardShader.SetMatrixUniform("View", View);
    this->BillboardShader.SetMatrixUniform("Projection", Maths::MakePerspectiveProjectionMatrix
    (
        Maths::ToRadians(InEye.GetDegYFov()),
        static_cast<f32>(InViewport.GetDimensions().X) / static_cast<f32>(InViewport.GetDimensions().Y),
        InEye.GetNearFrustum(), InEye.GetFarFrustum()
    ));

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glBindVertexArray(this->BillboardVao.value());
    glBindBuffer(GL_ARRAY_BUFFER, this->BillboardVbo.value());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->BillboardEbo.value());

    for (const LAstron& Astron : this->Astra)
    {
        if (Maths::IsNearlyZero(Astron.Load))
        {
            continue;
        }

        check( Astron.Direction.IsNormalized() )
        check( Astron.Scale.IsNearlyZero() == false )

        const LVector Position { (Astron.Direction * Astron.Magnitude) };
        const LVector Forward { (-Position).GetUnsafeNormalized() };
        const LVector Right { LVector::UpVector.Cross(Forward).GetUnsafeNormalized() };
        const LVector Up { Forward.Cross(Right) };
        LMatrix Rotation;
        Rotation.Matrix[0][0] = Right.X;
        Rotation.Matrix[0][1] = Right.Y;
        Rotation.Matrix[0][2] = Right.Z;
        Rotation.Matrix[0][3] = 0.0f;
        Rotation.Matrix[1][0] = Up.X;
        Rotation.Matrix[1][1] = Up.Y;
        Rotation.Matrix[1][2] = Up.Z;
        Rotation.Matrix[1][3] = 0.0f;
        Rotation.Matrix[2][0] = -Forward.X; /* Negativ because OpenGl does OpenGl things... */
        Rotation.Matrix[2][1] = -Forward.Y;
        Rotation.Matrix[2][2] = -Forward.Z;
        Rotation.Matrix[2][3] = 0.0f;
        LMatrix Translation;
        Translation.InlineTranslate(Position);
        LMatrix Model = Translation * Rotation;
        Model.InlineScale(Astron.Scale);
        this->BillboardShader.SetMatrixUniform("Model", Model);

        glBindTexture(GL_TEXTURE_2D, Astron.Texture.GetHandle());

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        continue;
    }

    glDisable(GL_BLEND);

    return;
}

void Jafg::LSkybox::Free()
{
    if (this->Vao.has_value())
    {
        check( this->Vbo.has_value() )

        glDeleteVertexArrays(1, &this->Vao.value());
        this->Vao.reset();

        glDeleteBuffers(1, &this->Vbo.value());
        this->Vbo.reset();
    }

    return;
}

#endif
