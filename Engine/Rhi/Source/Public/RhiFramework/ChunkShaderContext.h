// Copyright mzoesch. All rights reserved.

#pragma once

#include "GenericShaderContext.h"
#include "RhiFramework/ChunkBoxVertex.h"

namespace Jafg
{

class LChunkShaderInstance;

struct LChunkShaderDrawArgs : public LGenericShaderContextDrawArgs
{
    float                 DegYFov       = 0.0f;
    LMatrix               ViewMatrix    = LMatrix(Matrix::Identity);
    LVector               WorldLocation = { };
    int32                 NumTriangles  = 0;
    LChunkShaderInstance* Instance      = nullptr;
};

class RHI_API LChunkShaderContext final : public LGenericShaderContext
{
public:

    virtual void Make() override;
    virtual void OnFree() override;
    virtual void Draw(const LViewport& Context, LGenericShaderContextDrawArgs& InArgs) const override;

    FORCEINLINE auto GetProgram()       ->       LShader* { return this->Program; }
    FORCEINLINE auto GetProgram() const -> const LShader* { return this->Program; }

    FORCEINLINE auto GetTextureLocation() const -> uint32 { return this->Texture; }

private:

    LShader* Program = nullptr;
    uint32   Texture = 0;
};

class RHI_API LChunkShaderInstance final
{
public:

    LChunkShaderInstance() = default;
    DEFAULT_REALLOC_OF_ANY_FROM(LChunkShaderInstance)
    ~LChunkShaderInstance();

    void LoadMeshToGraphicsMemory(const TdhArray<ChunkBoxVertex>& Vertices, const TdhArray<uint32>& Indices);

    FORCEINLINE auto GetVertexArrayObject()   const -> uint32 { return this->Vao; }
    FORCEINLINE auto GetVertexBufferObject()  const -> uint32 { return this->Vbo; }
    FORCEINLINE auto GetElementBufferObject() const -> uint32 { return this->Ebo; }

private:

    uint32 Vao = 0;
    uint32 Vbo = 0;
    uint32 Ebo = 0;
};

} /* ~Namespace Jafg */
