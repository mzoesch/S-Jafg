// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/EngineShader.h"

namespace Jafg
{

class LOrthographicOutlineBoxShader : public LEngineShader
{
public:

    typedef LEngineShader Super;

    virtual TArray<LShaderCompileTimeConstant> GetDefaultConstants() override;
    virtual bool Make(const LName InName, TArray<LShaderCompileTimeConstant>&& InConstants = {}) override;
    virtual void UpdateViewportUniforms(const LViewport& Context) override;
    virtual void OnFree() override;

    ENGINE_API void Draw
    (
        const LViewport& Context,
        const LVector2&  Size,
        const LVector2&  TopLeft,
        const LColor&    Tint,
        const f32        OutlineThickness,
        const LColor&    OutlineTint
    ) const;

private:

    u32 Vao { 0x0u };
    u32 Vbo { 0x0u };
};

} /* ~Namespace Jafg */
