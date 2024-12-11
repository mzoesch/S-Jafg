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
 * Optional Wsdsml declarations for a subclass of WWidgetRegion.
 * This allows the subclass to use the Wsdsml methods with themselves as the return type.
 */
#define WIDGET_REGION_SUBCLASS_WSDSML_DECLARATIONS(ClassName)                                                                                      \
    FORCEINLINE auto SetBrush(const LRegionBrush& InBrush) -> ClassName& { Super::SetBrush(InBrush); return *this; }                               \
    FORCEINLINE auto SetTint(const LColor& InTint) -> ClassName& { Super::SetTint(InTint); return *this; }                                         \
    FORCEINLINE auto SetTexture(const LTexture2* InTexture) -> ClassName& { Super::SetTexture(InTexture); return *this; }                          \
    FORCEINLINE auto SetImage(const LImage& InImage) -> ClassName& { Super::SetImage(InImage); return *this; }                                     \
    FORCEINLINE auto SetPadding(const LPadding& InPadding) -> ClassName& { Super::SetPadding(InPadding); return *this; }                           \
    FORCEINLINE auto SetAnchor(const LAnchor& InAnchor) -> ClassName& { Super::SetAnchor(InAnchor); return *this; }                                \
    FORCEINLINE auto SetAnchor(const EAnchor::Type InAnchor) -> ClassName& { Super::SetAnchor(InAnchor); return *this; }                           \
    FORCEINLINE auto SetVisibility(const EWidgetVisibility::Type InVisibility) -> ClassName& { Super::SetVisibility(InVisibility); return *this; } \
    FORCEINLINE auto operator&(const LColor&         InTint) -> ClassName& { return this->SetTint(InTint);       }                                 \
    FORCEINLINE auto operator&(const LColor&&        InTint) -> ClassName& { return this->SetTint(InTint);       }                                 \
    FORCEINLINE auto operator&(const LTexture2*   InTexture) -> ClassName& { return this->SetTexture(InTexture); }                                 \
    FORCEINLINE auto operator&(const LImage&        InImage) -> ClassName& { return this->SetImage(InImage);     }                                 \
    FORCEINLINE auto operator&(const LRegionBrush&  InBrush) -> ClassName& { return this->SetBrush(InBrush);     }                                 \
    FORCEINLINE auto operator&(const LPadding&    InPadding) -> ClassName& { return this->SetPadding(InPadding); }                                 \
    FORCEINLINE auto operator&(const LAnchor&      InAnchor) -> ClassName& { return this->SetAnchor(InAnchor);   }                                 \
    FORCEINLINE auto operator&(const EAnchor::Type InAnchor) -> ClassName& { return this->SetAnchor(InAnchor);   }                                 \
    FORCEINLINE auto operator&(const EWidgetVisibility::Type InVisibility) -> ClassName& { return this->SetVisibility(InVisibility); }

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
    FORCEINLINE auto SetPadding(const LPadding& InPadding) -> WWidgetRegion& { Super::SetPadding(InPadding); return *this; }
    FORCEINLINE auto SetAnchor(const LAnchor& InAnchor) -> WWidgetRegion& { Super::SetAnchor(InAnchor); return *this; }
    FORCEINLINE auto SetAnchor(const EAnchor::Type InAnchor) -> WWidgetRegion& { Super::SetAnchor(InAnchor); return *this; }
    FORCEINLINE auto SetVisibility(const EWidgetVisibility::Type InVisibility) -> WWidgetRegion& { Super::SetVisibility(InVisibility); return *this; }

    ///////////////////////////////////////////////////////////////////////////////
    // Wsdsml
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE auto operator&(const LColor&         InTint) -> WWidgetRegion& { return this->SetTint(InTint);       }
    FORCEINLINE auto operator&(const LColor&&        InTint) -> WWidgetRegion& { return this->SetTint(InTint);       }
    FORCEINLINE auto operator&(const LTexture2*   InTexture) -> WWidgetRegion& { return this->SetTexture(InTexture); }
    FORCEINLINE auto operator&(const LImage&        InImage) -> WWidgetRegion& { return this->SetImage(InImage);     }
    FORCEINLINE auto operator&(const LRegionBrush&  InBrush) -> WWidgetRegion& { return this->SetBrush(InBrush);     }
    FORCEINLINE auto operator&(const LPadding&    InPadding) -> WWidgetRegion& { return this->SetPadding(InPadding); }
    FORCEINLINE auto operator&(const LAnchor&      InAnchor) -> WWidgetRegion& { return this->SetAnchor(InAnchor);   }
    FORCEINLINE auto operator&(const EAnchor::Type InAnchor) -> WWidgetRegion& { return this->SetAnchor(InAnchor);   }
    FORCEINLINE auto operator&(const EWidgetVisibility::Type InVisibility) -> WWidgetRegion& { return this->SetVisibility(InVisibility); }

private:

    void CreateNewShaderContext() const;

    TOptional<LRegionBrush> Brush;
    mutable TOptional<LBoxShaderContext> ShaderContext;
};

} /* ~Namespace Jafg */
