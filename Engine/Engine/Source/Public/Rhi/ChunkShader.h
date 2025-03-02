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

    virtual uint32 Make() override;
    virtual void   UpdateUniforms(const LViewport& Viewport, const LWorld& World, const LEye& Eye) override;
    virtual void   OnFree() override;

    FORCEINLINE auto GetBlendOpaqueTextureLocation() const noexcept -> uint32 { return this->BlendOpaqueTex; }
    FORCEINLINE auto GetBlendersTextureLocation()    const noexcept -> uint32 { return this->BlendersTex; }

private:

    uint32 BlendOpaqueTex = NULL;
    uint32 BlendersTex    = NULL;
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

    void LoadMeshToGraphicsMemory(const TdhArray<ChunkBoxVertex>& Vertices, const TdhArray<uint32>& Indices);

    FORCEINLINE uint32 GetVertexArrayObject()   const noexcept { return this->Vao; }
    FORCEINLINE uint32 GetVertexBufferObject()  const noexcept { return this->Vbo; }
    FORCEINLINE uint32 GetElementBufferObject() const noexcept { return this->Ebo; }

private:

    uint32 Vao = NULL;
    uint32 Vbo = NULL;
    uint32 Ebo = NULL;
    bool   bLoaded = false;
};

} /* ~Namespace Jafg */
