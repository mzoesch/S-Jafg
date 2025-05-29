// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/EngineShader.h"
#include "Rhi/Image.h"

namespace Jafg
{

class LOrthographicImageBoxShader : public LEngineShader
{
public:

    typedef LEngineShader Super;

    virtual bool Make(const LName InName) override;
    virtual void UpdateViewportUniforms(const LViewport& Context) override;
    virtual void OnFree() override;

    ENGINE_API void Draw
    (
        const LViewport& Context,
        const LVector2&  Size,
        const LVector2&  TopLeft,
        const LColor&    Tint,
        const LImage&    Image
    ) const;

private:

    u32 Vao { 0x0u };
    u32 Vbo { 0x0u };
};

} /* ~Namespace Jafg */
