// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/GenericShaderContext.h"
#include "Rhi/Shader.h"

namespace Jafg
{

struct LDebugTraceSphereShaderContextDrawArgs final : public LGenericShaderContextDrawArgs
{
    float DegYFov = 0.0f;
    LMatrix ViewMatrix = LMatrix(Matrix::Identity);
    LVector Center = { };
    float Radius = 0.0f;
    int32 Slices = 20;
    int32 Stacks = 20;
    LColor  Color = { };
};

class LDebugTraceSphereShaderContext final : public LGenericShaderContext
{
public:

    virtual void Make() override;
    virtual void Draw(const LViewport& Context, LGenericShaderContextDrawArgs& InArgs) const override;
    virtual void OnFree() override;

private:

    LShader Program = { };
    uint32  Vao = 0;
    uint32  Vbo = 0;
};

} /* ~Namespace Jafg */
