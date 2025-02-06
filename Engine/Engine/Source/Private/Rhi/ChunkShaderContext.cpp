// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Rhi/ChunkShaderContext.h"
#include "Widgets/Viewport.h"
#include <glm/glm.hpp>
#include "Engine/ObjectBaseUtility.h"
#include "User/UserPreferences.h"
#include "Rhi/RhiVendorInclude.h"
#include "Engine/Engine.h"
#include "Engine/Framework/ApplicationInstance.h"
#include "System/EnginePath.h"
#include "System/MaterialSubsystem.h"

void Jafg::LChunkShaderContext::Make()
{
    LGenericShaderContext::Make();

    const JMaterialSubsystem* Subsystem = GEngine->GetApplicationInstance()->GetSubsystem<JMaterialSubsystem>();

    this->Program = LShader(LEnginePath(EEnginePaths::Shaders, "Chunk"));
    this->Program.Use();

    glGenTextures(1, &this->BlendOpaqueTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->BlendOpaqueTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB /* out */,
        static_cast<GLsizei>(Subsystem->GetBlendOpaqueAtlasTexture().GetWidth()),
        static_cast<GLsizei>(Subsystem->GetBlendOpaqueAtlasTexture().GetHeight()),
        0, GL_RGBA /* in */, GL_UNSIGNED_BYTE, Subsystem->GetBlendOpaqueAtlasTexture().GetFirstMipMap().Bulk.GetBulk()
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    glGenTextures(1, &this->BlendersTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->BlendersTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA /* out */,
        static_cast<GLsizei>(Subsystem->GetBlendersAtlasTexture().GetWidth()),
        static_cast<GLsizei>(Subsystem->GetBlendersAtlasTexture().GetHeight()),
        0, GL_RGBA /* in */, GL_UNSIGNED_BYTE, Subsystem->GetBlendersAtlasTexture().GetFirstMipMap().Bulk.GetBulk()

    );
    glGenerateMipmap(GL_TEXTURE_2D);

    glUniform1i(glGetUniformLocation(this->Program.GetId(), "BlendOpaqueTexSampler"), 0);
    glUniform1i(glGetUniformLocation(this->Program.GetId(), "BlendersTexSampler"),    1);

    this->Program.SetUIntUniform("AtlasBlendOpaqueDomainWCount", Subsystem->GetBlendOpaqueDomainWidth());
    this->Program.SetUIntUniform("AtlasBlendersDomainWCount", Subsystem->GetBlendersDomainWidth());

    return;
}

void Jafg::LChunkShaderContext::OnFree()
{
    LGenericShaderContext::OnFree();

    this->Program.Free();

    glDeleteTextures(1, &this->BlendOpaqueTex);
    glDeleteTextures(1, &this->BlendersTex);

    return;
}

void Jafg::LChunkShaderContext::OnReload()
{
    LGenericShaderContext::OnReload();
}

void Jafg::LChunkShaderContext::Draw(const LViewport& Context, LGenericShaderContextDrawArgs& InArgs) const
{
    GENERIC_SHADER_DRAW_BODY(LChunkShaderDrawArgs)

    this->Program.Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->BlendOpaqueTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->BlendersTex);

    glBindVertexArray(Args.Instance->GetVertexArrayObject());

    const TMatrix Projection = Maths::MakePerspectiveProjectionMatrix(
        Maths::ToRadians(Args.DegYFov),
        static_cast<float>(Context.GetDimensions().X) / static_cast<float>(Context.GetDimensions().Y),
        0.1f, 2000.0f
    );
    LMatrix Model; Model.InlineTranslate(Args.WorldLocation);

    this->Program.SetMatrixUniform("View", Args.ViewMatrix);
    this->Program.SetMatrixUniform("Projection", Projection);
    this->Program.SetMatrixUniform("Model", Model);

    glDrawElements(GL_TRIANGLES, Args.NumTriangles, GL_UNSIGNED_INT, nullptr);

    return;
}

Jafg::LChunkShaderInstance::~LChunkShaderInstance()
{
    if (this->bLoaded)
    {
        glDeleteBuffers(1, &this->Vbo);
        glDeleteBuffers(1, &this->Ebo);
        glDeleteVertexArrays(1, &this->Vao);
    }

    return;
}

void Jafg::LChunkShaderInstance::LoadMeshToGraphicsMemory(const TdhArray<ChunkBoxVertex>& Vertices, const TdhArray<uint32>& Indices)
{
    if (this->bLoaded == false)
    {
        glGenVertexArrays(1, &this->Vao);
        glBindVertexArray(this->Vao);
        glGenBuffers(1, &this->Vbo);
        glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
        glGenBuffers(1, &this->Ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->Ebo);

        glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(ChunkBoxVertex), reinterpret_cast<void*>(offsetof(::Jafg::ChunkBoxVertex, LocationX)));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, sizeof(ChunkBoxVertex), reinterpret_cast<void*>(offsetof(::Jafg::ChunkBoxVertex, TextureGridX)));
        glEnableVertexAttribArray(1);
        glVertexAttribIPointer(2, 1, GL_BYTE, sizeof(ChunkBoxVertex), reinterpret_cast<void*>(offsetof(::Jafg::ChunkBoxVertex, Normal)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 2, GL_BYTE, GL_FALSE, sizeof(ChunkBoxVertex), reinterpret_cast<void*>(offsetof(::Jafg::ChunkBoxVertex, BlendTextureGridX)));
        glEnableVertexAttribArray(3);

        this->bLoaded = true;
    }

    glBindVertexArray(this->Vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        Vertices.GetSize() * static_cast<GLsizeiptr>(sizeof(::Jafg::ChunkBoxVertex)),
        Vertices.GetData(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->Ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        Indices.GetSize() * static_cast<GLsizeiptr>(sizeof(uint32)),
        Indices.GetData(),
        GL_STATIC_DRAW
    );

    return;
}
