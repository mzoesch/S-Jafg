// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/EngineShader.h"
#include "Rhi/Image.h"
#include "Widgets/RegionForward.h"

namespace Jafg
{

class LOrthographicRoundedOutlineImageBoxShader : public LEngineShader
{
public:

    typedef LEngineShader Super;

    virtual TArray<LShaderCompileTimeConstant> GetDefaultConstants() override { return {}; }
    virtual bool Make(const LName InName, TArray<LShaderCompileTimeConstant>&& InConstants = {}) override { return true; }
    virtual void UpdateViewportUniforms(const LViewport& Context) override {}
    virtual void OnFree() override {}

    ENGINE_API void Draw
    (
        const LViewport&           Context,
        const LVector2D&            TopLeft,
        const LVector2D&            Size,
        const Lal::LColor&         Tint,
        const f64                  OutlineThickness,
        const Lal::LColor&         OutlineTint,
        const LVector4D&            Radii,
        const LImage&              Image,
        const Lal::LColor&         ImageTint,
        const f64                  ImageScale,
        const EImageBehavior::Type ImageBehavior,
        const EImageOobm::Type     ImageOobm,
        const f64                  ImagePadding
    ) const {}

private:

    u32 Vao { 0x0u };
    u32 Vbo { 0x0u };
};

} /* ~Namespace Jafg */
