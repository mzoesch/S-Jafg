// Copyright mzoesch. All rights reserved.

#include "Rhi/ChunkShader.h"
#include "Engine/Engine.h"
#include "Framework/Eye.h"
#include "Rhi/RhiVendorInclude.h"
#include "Stats/Stats.h"
#include "System/MaterialSubsystem.h"
#include "System/EnginePath.h"

bool Jafg::LChunkShader::Make(const LName InName, TArray<LShaderCompileTimeConstant>&& InConstants)
{
    if (const bool Out = Super::Make(InName, std::move(InConstants)); Out == false)
    {
        return false;
    }

    const JMaterialSubsystem* Subsystem = GEngine->GetSubsystem<JMaterialSubsystem>();

    this->Program = LShader(LEnginePath(EEnginePaths::Shaders, "Chunk"));
    this->Program.Use();

    glGenTextures(1, &this->BlendOpaqueTex);
    glBindTexture(GL_TEXTURE_2D, this->BlendOpaqueTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB /* out */,
        static_cast<GLsizei>(Subsystem->GetBlendOpaqueAtlasTexture().GetWidth()),
        static_cast<GLsizei>(Subsystem->GetBlendOpaqueAtlasTexture().GetHeight()),
        0, GL_RGBA /* in */, GL_UNSIGNED_BYTE, Subsystem->GetBlendOpaqueAtlasTexture().GetFirstMipMap().GetBulk().GetRawBulk()
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &this->BlendersTex);
    glBindTexture(GL_TEXTURE_2D, this->BlendersTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA /* out */,
        static_cast<GLsizei>(Subsystem->GetBlendersAtlasTexture().GetWidth()),
        static_cast<GLsizei>(Subsystem->GetBlendersAtlasTexture().GetHeight()),
        0, GL_RGBA /* in */, GL_UNSIGNED_BYTE, Subsystem->GetBlendersAtlasTexture().GetFirstMipMap().GetBulk().GetRawBulk()

    );
    glGenerateMipmap(GL_TEXTURE_2D);

    this->Program.SetUIntUniform("AtlasBlendOpaqueDomainWCount", Subsystem->GetBlendOpaqueDomainWidth());
    this->Program.SetUIntUniform("AtlasBlendersDomainWCount", Subsystem->GetBlendersDomainWidth());
    this->Program.SetIntUniform("BlendOpaqueTexSampler", 0);
    this->Program.SetIntUniform("BlendersTexSampler", 1);

    glActiveTexture(GL_TEXTURE0);

    return true;
}

void Jafg::LChunkShader::OnRecompile()
{
    Super::OnRecompile();
    const JMaterialSubsystem* Subsystem = GEngine->GetSubsystem<JMaterialSubsystem>();

    this->Program.Use();
    this->Program.SetUIntUniform("AtlasBlendOpaqueDomainWCount", Subsystem->GetBlendOpaqueDomainWidth());
    this->Program.SetUIntUniform("AtlasBlendersDomainWCount", Subsystem->GetBlendersDomainWidth());
    this->Program.SetIntUniform("BlendOpaqueTexSampler", 0);
    this->Program.SetIntUniform("BlendersTexSampler", 1);

    return;
}

void Jafg::LChunkShader::UpdateWorldUniforms(const LViewport& Context, const LWorld& World, const LEye& Eye)
{
    this->Program.Use();

    const LMatrix Projection = Maths::MakePerspectiveProjectionMatrix
    (
        Maths::ToRadians(Eye.GetDegYFov()),
        static_cast<float>(Context.GetDimensions().X) / static_cast<float>(Context.GetDimensions().Y),
        Eye.GetNearFrustum(), Eye.GetFarFrustum()
    );

    this->Program.SetMatrixUniform("View", Eye.GetViewMatrix());
    this->Program.SetMatrixUniform("Projection", Projection);

    return;
}

void Jafg::LChunkShader::OnFree()
{
    Super::OnFree();

    glDeleteTextures(1, &this->BlendOpaqueTex);
    glDeleteTextures(1, &this->BlendersTex);

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

void Jafg::LChunkShaderInstance::LoadMeshToGraphicsMemory(const TArray<ChunkBoxVertex>& Vertices, const TArray<u32>& Indices)
{
    STAT_CYCLE_FUNCTION()

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
        Vertices.GetSize() * static_cast<GLsizeiptr>(sizeof(ChunkBoxVertex)),
        Vertices.GetDataPointer(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->Ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        Indices.GetSize() * static_cast<GLsizeiptr>(sizeof(u32)),
        Indices.GetDataPointer(),
        GL_STATIC_DRAW
    );

    return;
}
