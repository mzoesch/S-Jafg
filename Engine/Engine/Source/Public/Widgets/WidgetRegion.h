// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetParent.h"
#include "Rhi/BoxShaderContext.h"
#include "Misc/Optional.h"
#include "Widgets/Image.h"
#include "WidgetRegion.generated.h"

namespace Jafg
{

struct LRegionBrush
{
    LColor Tint  = LColor::White;
    LImage Image = LImage();
};

/**
 * WWidgetRegion is a resizable Parent node. Every region has its layout bounds defined as (0, 0, width, height).
 * A region might still draw outside these bounds.
 */
DECLARE_JAFG_CLASS()
class ENGINE_API WWidgetRegion : public WWidgetParent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WWidgetRegion)

public:

    virtual void Draw(LViewport& Context) const override;
    virtual void UpdateDesiredSize() const override;

    FORCEINLINE auto SetBrush(const LRegionBrush& InBrush) -> WWidgetRegion& { this->Brush = InBrush; return *this; }
    FORCEINLINE auto HasBrush() const -> bool { return this->Brush.IsSet(); }
    FORCEINLINE auto GetBrush() const -> const LRegionBrush& { return this->Brush.GetValue(); }
    auto SetTint(const LColor& InTint) -> WWidgetRegion&;
    auto SetTexture(const LTexture2* InTexture) -> WWidgetRegion&;
    auto SetImage(const LImage& InImage) -> WWidgetRegion&;

    FORCEINLINE auto SetRegionPadding(const LPadding& InPadding) -> WWidgetRegion& { Super::SetPadding(InPadding); return *this; }

    ///////////////////////////////////////////////////////////////////////////////
    // Wsdsml
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE auto operator&(const LColor&        InTint) -> WWidgetRegion& { return this->SetTint(InTint);       }
    FORCEINLINE auto operator&(const LColor&&       InTint) -> WWidgetRegion& { return this->SetTint(InTint);       }
    FORCEINLINE auto operator&(const LTexture2*  InTexture) -> WWidgetRegion& { return this->SetTexture(InTexture); }
    FORCEINLINE auto operator&(const LImage&       InImage) -> WWidgetRegion& { return this->SetImage(InImage);     }
    FORCEINLINE auto operator&(const LRegionBrush& InBrush) -> WWidgetRegion& { return this->SetBrush(InBrush);     }

private:

    void CreateNewShaderContext() const;

    TOptional<LRegionBrush> Brush;
    mutable TOptional<LBoxShaderContext> ShaderContext;
};

} /* ~Namespace Jafg. */
