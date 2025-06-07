// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/Cubemap.h"
#include "Rhi/Shader.h"

namespace Jafg
{

//#
//# Represents a skybox.
//#
class LSkybox final
{
public:

    LSkybox() = default;
    ENGINE_API explicit LSkybox(const TArray<LEnginePath>& InDefaultSkybox);
    PROHIBIT_COPY(LSkybox)
    DEFAULT_MOVE(LSkybox)
    ~LSkybox();

    ENGINE_API void Upload();
    ENGINE_API void Draw(const LViewport& InViewport, const LEye& InEye) const;

private:

    LCubemap DefaultCube;
    TOptional<u32> Vao;
    TOptional<u32> Vbo;
    LShader Shader;
};

} /* ~Namespace Jafg */
