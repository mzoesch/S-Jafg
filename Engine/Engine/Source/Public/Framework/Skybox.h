// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/Uuid.h"
#include "Engine/Level.h"
#include "Rhi/Cubemap.h"
#include "Rhi/Shader.h"
#include "Rhi/Texture2.h"

namespace Jafg
{

class LSkybox;
struct LLoadedCubemap;

struct LLoadedCubemap final
{
    LString Identifier;
    LCubemap Map;

    //#
    //# The percentage of the cubemap to load.
    //#
    f32 Load { 1.0f };
};

struct LAstron final
{
    LUuid Identifier;

    //#
    //# The texture to use for this astron.
    //#
    LTexture2 Texture;

    //#
    //# The direction of the astron in world space.
    //# This vector has to be normalized.
    //#
    LVector Direction;

    //#
    //# The magnitude of the directional vector for this astron.
    //#
    //# As far as it gets, because of the far plane of the default eye. If you
    //# use a different eye the max value might differ.
    //#
    f32 Magnitude { 1950.0f };

    //#
    //# The scaling vector for the astron.
    //#
    LVector Scale { 1.0f };

    //# @see #LLoadedCubemap::Load.
    f32 Load { 1.0f };
};

//#
//# Represents a skybox.
//#
class LSkybox final
{
public:

    LSkybox() = default;
    /*ENGINE_API*/ explicit LSkybox(const LString& DefaultName, const TArray<LEnginePath>& InDefaultSkybox) {}
    /*ENGINE_API*/ explicit LSkybox(const TArray<LLevelSkyboxMap>& InDefaultSkybox) {}
    PROHIBIT_COPY(LSkybox)
    DEFAULT_MOVE(LSkybox)
    ~LSkybox() { this->Free(); }

    /*ENGINE_API*/ void Upload() {}
    /*ENGINE_API*/ void Draw(const LViewport& InViewport, const LEye& InEye) const {}
    /*ENGINE_API*/ void Free() {}

    FORCEINLINE const Lal::LColor& GetBackgroundColor() const { return this->BackgroundColor; }
    FORCEINLINE void SetBackgroundColor(const Lal::LColor& InColor) { this->BackgroundColor = InColor; }

    FORCEINLINE TArray<LLoadedCubemap>& GetMutableMaps() { return this->Maps; }
    FORCEINLINE const TArray<LLoadedCubemap>& GetMaps() const { return this->Maps; }

    FORCEINLINE TArray<LAstron>& GetMutableAstra() noexcept { return this->Astra; }
    FORCEINLINE const TArray<LAstron>& GetAstra() const noexcept { return this->Astra; }

private:

    Lal::LColor BackgroundColor { Lal::LColor::Black };
    TArray<LLoadedCubemap> Maps;
    TArray<LAstron> Astra;
    TOptional<u32> Vao;
    TOptional<u32> Vbo;
    LShader Shader;
    LShader BillboardShader;
    TOptional<u32> BillboardVao;
    TOptional<u32> BillboardVbo;
    TOptional<u32> BillboardEbo;
};

} /* ~Namespace Jafg */
