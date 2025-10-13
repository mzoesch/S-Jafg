// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Overlay.h"
#include "Rhi/Image.h"
#include "Widgets/RegionForward.h"
#include "Widgets/RegionFactory.h"
#include "Region.generated.h"

namespace Jafg
{

//#
//# WRegion is an overlay node that can be customized with a #LRegionBrush.
//# A region might still draw outside these bounds.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryRegion)
class WRegion : public WOverlay
{
    GENERATED_CLASS_BODY()

protected:

    explicit WRegion(LCxxObjectInitializer const& CxxObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(WRegion)

public:

    virtual void BeginLifeCDR() override;
    virtual void Draw(LViewport& Context) const override;

    void SetBrush(const LRegionBrush& InBrush) { this->Brush = InBrush; }
    void SetBrush(LRegionBrush&& InBrush) { this->Brush = std::move(InBrush); }
    LRegionBrush& GetMutableBrush() { return this->Brush; }
    const LRegionBrush&  GetBrush() const { return this->Brush; }

    FORCEINLINE void SetType(const ERegionBrush::Type InType) { this->Brush.Type = InType; }
    FORCEINLINE void SetTint(const Lal::LColor& InTin) { this->Brush.Tint = InTin; }
    FORCEINLINE void SetTexture(const LTexture2* InTexture) { this->Brush.Image.SetTexture(InTexture); }
    FORCEINLINE void SetImage(const LImage& InImage) { this->Brush.Image = InImage; }
    FORCEINLINE void SetImageTint(const Lal::LColor& InColor) { this->Brush.ImageTint = InColor; }
    FORCEINLINE void SetImageScale(const f32 InScale) { this->Brush.ImageScale = InScale; }
    FORCEINLINE void SetImageBehavior(const EImageBehavior::Type InType) { this->Brush.ImageBehavior = InType; }
    FORCEINLINE void SetImageOobm(const EImageOobm::Type InType) { this->Brush.ImageOobm = InType; }
    FORCEINLINE void SetImagePadding(const f32 InPadding) { this->Brush.ImagePadding = InPadding; }
    FORCEINLINE void SetOutlineThickness(const f32 InOutlineThickness) { this->Brush.OutlineThickness = InOutlineThickness; }
    FORCEINLINE void SetOutlineRadii(const LVector4& InOutlineRadii) { this->Brush.Radii = InOutlineRadii; }
    FORCEINLINE void SetOutlineTint(const Lal::LColor& InOutlineTint) { this->Brush.OutlineTint = InOutlineTint; }

private:

    void RegisterShaders();

    LRegionBrush Brush;
};

} /* ~Namespace Jafg */
