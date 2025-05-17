// Copyright mzoesch. All rights reserved.

#pragma once

#include "Texture2.h"
#include "Rhi/EngineShader.h"

namespace Jafg
{

class ENGINE_API LOrthographicBoxShader : public LEngineShader
{
public:

    typedef LEngineShader Super;

    virtual bool Make(const LName InName) override;
    virtual void UpdateViewportUniforms(const LViewport& Context) override;
    virtual void OnFree() override;

    void Draw
    (
        const LViewport& Context,
        const LVector2&  Size,
        const LVector2&  TopLeft,
        const LColor&    Color
    ) const;

private:

    u32 Vao { 0x0u };
    u32 Vbo { 0x0u };
};

} /* ~Namespace Jafg */
