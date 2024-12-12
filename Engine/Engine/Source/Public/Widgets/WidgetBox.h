// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetNode.h"
#include "Rhi/BoxShaderContext.h"
#include "Widgets/Image.h"
#include "WidgetBox.generated.h"

namespace Jafg
{

struct LBoxBrush
{
    LColor Tint = LColor::White;
    LImage Image = LImage();
};

/**
 * A widget box is a widget that behaves roughly the same as a region but with
 * the major difference for not allowing children.
 * @see Widgets/WidgetRegion.h
 */
DECLARE_JAFG_CLASS()
class ENGINE_API WWidgetBox : public WWidgetNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WWidgetBox)

public:

    virtual void Draw(LViewport& Context) const override;
    virtual void UpdateDesiredSize() const override;

    FORCEINLINE auto SetBrush(const LBoxBrush& InBrush) -> WWidgetBox& { this->Brush = InBrush; return *this; }
    FORCEINLINE auto HasBrush() const -> bool { return this->Brush.IsSet(); }
    FORCEINLINE auto GetBrush() const -> const LBoxBrush& { return this->Brush.GetValue(); }
    FORCEINLINE auto GetPadding() const -> const LPadding& { return this->Padding; }
                auto SetTint(const LColor& InTint) -> WWidgetBox&;
                auto SetTexture(const LTexture2* InTexture) -> WWidgetBox&;
                auto SetImage(const LImage& InImage) -> WWidgetBox&;
    FORCEINLINE auto SetPadding(const LPadding& InPadding) -> WWidgetBox& { this->Padding = InPadding; return *this; }
    FORCEINLINE auto SetAnchor(const LAnchor& InAnchor) -> WWidgetBox& { Super::SetAnchor(InAnchor); return *this; }
    FORCEINLINE auto SetAnchor(const EAnchor::Type InAnchor) -> WWidgetBox& { Super::SetAnchor(InAnchor); return *this; }
    FORCEINLINE auto SetVisibility(const EWidgetVisibility::Type InVisibility) -> WWidgetBox& { Super::SetVisibility(InVisibility); return *this; }

    ///////////////////////////////////////////////////////////////////////////////
    // Wsdsml
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE auto operator&(const LColor&         InTint) -> WWidgetBox& { return this->SetTint(InTint);       }
    FORCEINLINE auto operator&(const LColor&&        InTint) -> WWidgetBox& { return this->SetTint(InTint);       }
    FORCEINLINE auto operator&(const LTexture2*   InTexture) -> WWidgetBox& { return this->SetTexture(InTexture); }
    FORCEINLINE auto operator&(const LImage&        InImage) -> WWidgetBox& { return this->SetImage(InImage);     }
    FORCEINLINE auto operator&(const LBoxBrush&     InBrush) -> WWidgetBox& { return this->SetBrush(InBrush);     }
    FORCEINLINE auto operator&(const LPadding&    InPadding) -> WWidgetBox& { return this->SetPadding(InPadding); }
    FORCEINLINE auto operator&(const LAnchor&      InAnchor) -> WWidgetBox& { return this->SetAnchor(InAnchor);   }
    FORCEINLINE auto operator&(const EAnchor::Type InAnchor) -> WWidgetBox& { return this->SetAnchor(InAnchor);   }
    FORCEINLINE auto operator&(const EWidgetVisibility::Type InVisibility) -> WWidgetBox& { return this->SetVisibility(InVisibility); }

private:

    void CreateNewShaderContext() const;

    LPadding Padding = LPadding();
    TOptional<LBoxBrush> Brush;
    mutable TOptional<LBoxShaderContext> ShaderContext;
};

} /* ~Namespace Jafg */
