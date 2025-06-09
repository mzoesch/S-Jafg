// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Level.h"
#include "Rhi/Cubemap.h"
#include "Rhi/Shader.h"

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

//#
//# Represents a skybox.
//#
class LSkybox final
{
public:

    LSkybox() = default;
    ENGINE_API explicit LSkybox(const LString& DefaultName, const TArray<LEnginePath>& InDefaultSkybox);
    ENGINE_API explicit LSkybox(const TArray<LLevelSkyboxMap>& InDefaultSkybox);
    PROHIBIT_COPY(LSkybox)
    DEFAULT_MOVE(LSkybox)
    ~LSkybox();

    ENGINE_API void Upload();
    ENGINE_API void Draw(const LViewport& InViewport, const LEye& InEye) const;

private:

    TArray<LLoadedCubemap> Maps;
    TOptional<u32> Vao;
    TOptional<u32> Vbo;
    LShader Shader;
};

} /* ~Namespace Jafg */
