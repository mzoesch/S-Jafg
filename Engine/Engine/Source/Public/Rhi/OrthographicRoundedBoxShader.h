// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/EngineShader.h"

namespace Jafg
{

class LOrthographicRoundedBoxShader : public LEngineShader
{
public:

    typedef LEngineShader Super;

    virtual TArray<LShaderCompileTimeConstant> GetDefaultConstants() override { return {}; }
    virtual bool Make(const LName InName, TArray<LShaderCompileTimeConstant>&& InConstants = {}) override { return true; }
    virtual void UpdateViewportUniforms(const LViewport& Context) override {}
    virtual void OnFree() override {}

    ENGINE_API void Draw
    (
        const LViewport&   Context,
        const LVector2D&    Size,
        const LVector2D&    TopLeft,
        const Lal::LColor& Tint,
        const LVector4D&    Radii
    ) const {}

private:

    u32 Vao { 0x0u };
    u32 Vbo { 0x0u };
};

} /* ~Namespace Jafg */
