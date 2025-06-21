// Copyright mzoesch. All rights reserved.

#include "Framework/Skybox.h"
#include "Engine/Engine.h"
#include "Rhi/RhiVendorInclude.h"
#include "Framework/Eye.h"
#include "Rhi/Texture2.h"
#include "Stats/Stats.h"

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

float BillboardVertices[] = {
    // pos        // tex
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,
 };

unsigned int BillboardIndices[] = { 0, 1, 2, 2, 3, 0 };

} /* ~Namespace Jafg */

Jafg::LSkybox::LSkybox(const LString& DefaultName, const TArray<LEnginePath>& InDefaultSkybox)
{
    this->Maps.Emplace(DefaultName, LCubemap{InDefaultSkybox});
    return;
}

Jafg::LSkybox::LSkybox(const TArray<LLevelSkyboxMap>& InDefaultSkybox)
{
    for (const auto& [Identifier, Textures, DefaultLoad] : InDefaultSkybox)
    {
        this->Maps.Emplace(Identifier, LCubemap{Textures}, DefaultLoad);
        continue;
    }

    return;
}

void Jafg::LSkybox::Upload()
{
    STAT_CYCLE_FUNCTION()

    LOG_VERBOSE(LogRhi, "Uploading new skybox.")

    check( this->Vao.IsValid() == false && this->Vbo.IsValid() == false )

    this->Shader = LShader{{EEnginePaths::Shaders, "Skybox"}};
    this->BillboardShader = LShader{{EEnginePaths::Shaders, "Billboard"}};

    this->Vao.Emplace(0);
    glGenVertexArrays(1, &this->Vao.GetValue());
    this->Vbo.Emplace(0);
    glGenBuffers(1, &this->Vbo.GetValue());

    glBindVertexArray(this->Vao.GetValue());
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo.GetValue());

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
    this->Shader.Unuse();

    this->BillboardVao.Emplace(0);
    glGenVertexArrays(1, &this->BillboardVao.GetValue());
    this->BillboardVbo.Emplace(0);
    glGenBuffers(1, &this->BillboardVbo.GetValue());
    this->BillboardEbo.Emplace(0);
    glGenBuffers(1, &this->BillboardEbo.GetValue());

    glBindVertexArray(this->BillboardVao.GetValue());
    glBindBuffer(GL_ARRAY_BUFFER, this->BillboardVbo.GetValue());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->BillboardEbo.GetValue());

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
    this->Shader.Unuse();

    return;
}

void Jafg::LSkybox::Draw(const LViewport& InViewport, const LEye& InEye) const
{
    glBindVertexArray(this->Vao.GetValue());
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo.GetValue());

    this->Shader.Use();

    const LMatrix4 Projection { Maths::MakePerspectiveProjectionMatrix
    (
        Maths::ToRadians(InEye.GetDegYFov()),
        InViewport.GetWidthF() / InViewport.GetHeightF(),
        /*
         * Do not use the near and fare z planes defined in the user preferences, as we are looking at a unit
         * cube with a translation of zero.
         */
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

    check( this->Maps.GetSize() <= 1 && "Currently only at most one map is supported." )
    for (i32 Idx { 0 }; Idx < this->Maps.GetSize(); ++Idx)
    {
        const LLoadedCubemap& Map = this->Maps[Idx];
        glActiveTexture(GL_TEXTURE0 + Idx);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Map.Map);
        this->Shader.SetFloatUniform("CubeLoad0", Map.Load);

        continue;
    }

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    this->BillboardShader.Use();
    this->BillboardShader.SetMatrixUniform("View", View);
    this->BillboardShader.SetMatrixUniform("Projection", Maths::MakePerspectiveProjectionMatrix
    (
        Maths::ToRadians(InEye.GetDegYFov()),
        static_cast<float>(InViewport.GetDimensions().X) / static_cast<float>(InViewport.GetDimensions().Y),
        InEye.GetNearFrustum(), InEye.GetFarFrustum()
    ));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(this->BillboardVao.GetValue());
    glBindBuffer(GL_ARRAY_BUFFER, this->BillboardVbo.GetValue());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->BillboardEbo.GetValue());

    for (const LAstron& Astron : this->Astra)
    {
        if (Maths::IsNearlyZero(Astron.Load))
        {
            continue;
        }

        check( Astron.Direction.IsNormalized() )
        check( Astron.Scale.IsNearlyZero() == false )

        LMatrix Model { Matrix::Identity };
        Model.InlineTranslate(Astron.Direction * Astron.Magnitude);
        // Model.InlineTranslate(Astron.Direction * 10);

        Model.Matrix[0][0] = View.Matrix[0][0];
        Model.Matrix[0][1] = View.Matrix[1][0];
        Model.Matrix[0][2] = View.Matrix[2][0];
        Model.Matrix[0][3] = 0.0f;

        Model.Matrix[1][0] = View.Matrix[0][1];
        Model.Matrix[1][1] = View.Matrix[1][1];
        Model.Matrix[1][2] = View.Matrix[2][1];
        Model.Matrix[1][3] = 0.0f;

        Model.Matrix[2][0] = View.Matrix[0][2];
        Model.Matrix[2][1] = View.Matrix[1][2];
        Model.Matrix[2][2] = View.Matrix[2][2];
        Model.Matrix[2][3] = 0.0f;

        Model.InlineScale(Astron.Scale);

        this->BillboardShader.SetMatrixUniform("Model", Model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Astra[0].Texture.GetHandle());

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        continue;
    }

    glDisable(GL_BLEND);

    return;
}

void Jafg::LSkybox::Free()
{
    if (this->Vao.IsValid())
    {
        check( this->Vbo.IsValid() )

        glDeleteVertexArrays(1, &this->Vao.GetValue());
        this->Vao.Reset();

        glDeleteBuffers(1, &this->Vbo.GetValue());
        this->Vbo.Reset();

        this->Shader.Free();
    }

    return;
}
