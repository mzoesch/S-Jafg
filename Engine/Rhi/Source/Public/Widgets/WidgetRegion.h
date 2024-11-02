// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetParent.h"
#include "RhiFramework/SimpleShaderContext.h"
#include "Misc/Optional.h"
#include "WidgetRegion.generated.h"

namespace Jafg
{

struct LRegionBrush
{
    LColor Tint = LColor::Black;
};

/**
 * WWidgetRegion is a resizable Parent node. Every region has its layout bounds defined as (0, 0, width, height).
 * A region might still draw outside these bounds.
 */
DECLARE_JAFG_CLASS()
class RHI_API WWidgetRegion : public WWidgetParent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WWidgetRegion)

public:

    virtual void Draw(LViewport* Context) const override;

    FORCEINLINE auto SetBrush(const LRegionBrush& InBrush) -> WWidgetRegion& { this->Brush = InBrush; return *this; }
    FORCEINLINE auto HasBrush() const -> bool { return this->Brush.IsSet(); }
    FORCEINLINE auto GetBrush() const -> const LRegionBrush& { return this->Brush.GetValue(); }

    FORCEINLINE auto SetRegionPadding(const LPadding& InPadding) -> WWidgetRegion& { Super::SetPadding(InPadding); return *this; }

    FORCEINLINE auto operator&(const LRegionBrush& InBrush) -> WWidgetRegion& { return this->SetBrush(InBrush); }

private:

    void CreateNewShaderContext() const;

    TOptional<LRegionBrush> Brush;
    mutable TOptional<LSimpleShaderContext> ShaderContext;
};

} /* ~Namespace Jafg. */
