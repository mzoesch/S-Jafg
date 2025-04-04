// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/GenericShaderContext.h"
#include "Widgets/Whitespace.h"

namespace Jafg
{

struct LFontShaderContextDrawArgs final : public LGenericShaderContextDrawArgs
{
    const LString* Content = nullptr;
    LVector2 Offset;
    LPadding Padding;
    LVector2 DesiredSize = LVector2::Zero();
    LColor   Color = LColor::White;
    float    Scale = 1.0f;
};

class ENGINE_API LFontShaderContext final : public LGenericShaderContext
{
public:

    virtual void Make() override;
    virtual void Draw(const LViewport& Context, LGenericShaderContextDrawArgs& InArgs) const override;
    virtual void OnFree() override;

    static bool  GetMinimalDesiredSize(const LString& InContent, const float InScale, LVector2& OutSize);

    static LVector2 GetDesiredSize(const LString& InContent, const float InScale);
    static f32      GetDesiredWidth(const LString& InContent, const float InScale);
    static f32      GetApproximateHeight(const float InScale);

private:

    u32  Vao = 0;
    u32  Vbo = 0;
};

} /* ~Namespace Jafg */
