// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/GenericShaderContext.h"
#include "Rhi/Shader.h"

namespace Jafg
{

struct LDebugTraceLineShaderContextDrawArgs final : public LGenericShaderContextDrawArgs
{
    float DegYFov = 0.0f;
    LMatrix ViewMatrix = LMatrix(Matrix::Identity);
    LVector Start = { };
    LVector End = { };
    LColor  Color = { };
};

class LDebugTraceLineShaderContext final : public LGenericShaderContext
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
