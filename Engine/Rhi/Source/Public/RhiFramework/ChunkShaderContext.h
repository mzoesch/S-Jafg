// Copyright mzoesch. All rights reserved.

#pragma once

#include "GenericShaderContext.h"
#include "JustTemp.h"

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

    FORCEINLINE LShader* GetProgram() { return this->Program; }
    FORCEINLINE const LShader* GetProgram() const { return this->Program; }

    FORCEINLINE uint32 GetTexture() const { return this->Texture; }

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

    void LoadMeshToGraphicsMemory(const TdhArray<Vertex>& Vertices, const TdhArray<uint32>& Indices);

    uint32 VertexArrayObject = 0;
    uint32 Vbo               = 0;
    uint32 Ebo               = 0;
};

} /* ~Namespace Jafg */
