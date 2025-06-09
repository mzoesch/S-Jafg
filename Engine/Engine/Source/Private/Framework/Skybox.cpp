// Copyright mzoesch. All rights reserved.

#include "Framework/Skybox.h"
#include "Engine/Engine.h"
#include "Rhi/RhiVendorInclude.h"
#include "Framework/Eye.h"
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

Jafg::LSkybox::~LSkybox()
{
    if (this->Vao.IsValid())
    {
        glDeleteVertexArrays(1, &this->Vao.GetValue());
        this->Vao.Reset();
    }

    if (this->Vbo.IsValid())
    {
        glDeleteBuffers(1, &this->Vbo.GetValue());
        this->Vbo.Reset();
    }

    return;
}

void Jafg::LSkybox::Upload()
{
    STAT_CYCLE_FUNCTION()

    LOG_VERBOSE(LogRhi, "Uploading new skybox.")

    check( this->Vao.IsValid() == false && this->Vbo.IsValid() == false )

    this->Shader = LShader{LEnginePath{EEnginePaths::Shaders, "Skybox"}};

    this->Vao.Emplace(0);
    glGenVertexArrays(1, &this->Vao.GetValue());
    this->Vbo.Emplace(0);
    glGenBuffers(1, &this->Vbo.GetValue());

    glBindVertexArray(this->Vao.GetValue());
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo.GetValue());

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), &Vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));

    glBindVertexArray(0);

    for (LLoadedCubemap& LoadedMap : this->Maps)
    {
        LoadedMap.Map.Reload();
    }

    this->Shader.Use();
    this->Shader.SetIntUniform("SkyboxSampler0", 0);

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

    return;
}
