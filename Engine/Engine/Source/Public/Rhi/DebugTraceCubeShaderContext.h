// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/GenericShaderContext.h"
#include "Rhi/Shader.h"

namespace Jafg
{

struct LDebugTraceCubeShaderContextDrawArgs final : public LGenericShaderContextDrawArgs
{
    float DegYFov = 0.0f;
    LMatrix ViewMatrix = LMatrix(Matrix::Identity);
    LVector BottomNearLeft = { };
    LVector RelTopFarRight = { };
    LColor Color = { };
    /* Depending on the hardware, the max value may vary (but usually it's not more than 10). */
    u8 Thickness = 1;
};

class LDebugTraceCubeShaderContext final : public LGenericShaderContext
{
public:

    virtual void Make() override;
    virtual void Draw(const LViewport& Context, LGenericShaderContextDrawArgs& InArgs) const override;
    virtual void OnFree() override;

private:

    LShader Program = { };
    u32  Vao = 0;
    u32  Vbo = 0;
};

} /* ~Namespace Jafg */
