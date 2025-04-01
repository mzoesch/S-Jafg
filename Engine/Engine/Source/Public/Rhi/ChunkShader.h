// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/EngineShader.h"
#include "Rhi/ChunkBoxVertex.h"

namespace Jafg
{

class LChunkShader;
class LChunkShaderInstance;

class LChunkShader final : public LEngineShader
{
public:

    typedef LEngineShader Super;

    virtual u32 Make() override;
    virtual void   UpdateUniforms(const LViewport& Viewport, const LWorld& World, const LEye& Eye) override;
    virtual void   OnFree() override;

    FORCEINLINE auto GetBlendOpaqueTextureLocation() const noexcept -> u32 { return this->BlendOpaqueTex; }
    FORCEINLINE auto GetBlendersTextureLocation()    const noexcept -> u32 { return this->BlendersTex; }

private:

    u32 BlendOpaqueTex = NULL;
    u32 BlendersTex    = NULL;
};

//#
//# Instance of a shader context to use by the chunk shader context per chunk basis.
//#
class LChunkShaderInstance final
{
public:

    LChunkShaderInstance() = default;
    DEFAULT_REALLOC_OF_ANY_FORM(LChunkShaderInstance)
    ~LChunkShaderInstance();

    void LoadMeshToGraphicsMemory(const TArray<ChunkBoxVertex>& Vertices, const TArray<u32>& Indices);

    FORCEINLINE u32 GetVertexArrayObject()   const noexcept { return this->Vao; }
    FORCEINLINE u32 GetVertexBufferObject()  const noexcept { return this->Vbo; }
    FORCEINLINE u32 GetElementBufferObject() const noexcept { return this->Ebo; }

private:

    u32 Vao = NULL;
    u32 Vbo = NULL;
    u32 Ebo = NULL;
    bool   bLoaded = false;
};

} /* ~Namespace Jafg */
