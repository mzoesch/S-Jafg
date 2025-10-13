// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/GenericShaderContext.h"
#include "Rhi/Shader.h"

namespace Jafg
{

struct LBlurShaderContextDrawArgs final : public LGenericShaderContextDrawArgs
{
    LVector2 Size;
    LVector2 TopLeft;
    float Strength = 0.0f;
};

class LBlurShaderContext final : public LGenericShaderContext
{
public:

    virtual void Make() override;
    virtual void Draw(const LViewport& Context, LGenericShaderContextDrawArgs& InArgs) const override;
    virtual void OnFree() override;

private:

    mutable LShader Shader = {};
    mutable u32  Vao    = 0x0u;
    mutable u32  Vbo    = 0x0u;
};

} /* ~Namespace Jafg */
