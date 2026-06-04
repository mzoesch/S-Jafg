// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Button.h"
#include "CheckmarkButton.generated.h"

namespace Jafg
{

struct LFactoryWCheckmarkButton;

//# A button that automatically constructs to a checkmark.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryWCheckmarkButton)
class ENGINE_API WCheckmarkButton : public WButton // TODO: Do not inherit from region but from box button
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WCheckmarkButton)
    {
        this->MinDesiredSize = 16_spt2;
        this->MaxDesiredSize = 16_spt2;
        this->Style.ChainEverywhere<&LRegionBrush::Tint, &LRegionBrush::OutlineTint, &LRegionBrush::OutlineThickness, &LRegionBrush::Background>
            (Colors::White, LColor{0x52}, 1, LRegionBrush::LIcon{.Texture=LOptionalTexture2Ref{"Icons/Jafg.Checkmark"}.GetResolved(), .Scale=0u,});
        this->Style.Set<EStyleBits::Normal, &LRegionBrush::BorderTint>(LColor{0x14});
        this->Style.Set<EStyleBits::Hover, &LRegionBrush::BorderTint>(LColor{0x1C});
        this->Style.Set<EStyleBits::Press, &LRegionBrush::BorderTint>(LColor{0x24});
        this->Style.Set<EStyleBits::Selected, &LRegionBrush::BorderTint>(LColor{0x24});
        this->Style.Set<EStyleBits::Disabled, &LRegionBrush::BorderTint>(LColor{0x0F});
    }

public:

    FORCEINLINE bool IsChecked() const noexcept { return this->bChecked; }
    FORCEINLINE void SetChecked(bool bInChecked) noexcept
    {
        if (this->bChecked == bInChecked)
        {
            return;
        }
        this->bChecked = bInChecked;

        auto Change([bInChecked](LRegionBrush& Brush)
        {
            if (std::holds_alternative<LRegionBrush::LIcon>(Brush.Background))
            {
                std::get<LRegionBrush::LIcon>(Brush.Background).Scale = bInChecked ? 1u : 0u;
            }
        });

        Change(this->Brush);
        this->Style.ForEachEverywhere(Change);
        return;
    }

private:

    bool bChecked{};
};

struct LFactoryWCheckmarkButton : NODE_FACTORY_PARENT(WCheckmarkButton)
{
    NODE_FACTORY_BODY(WCheckmarkButton)

    decltype(auto) Checked(this auto&& Self, bool bChecked) noexcept
    {
        NODE_FACTORY_SELF().SetChecked(bChecked);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
