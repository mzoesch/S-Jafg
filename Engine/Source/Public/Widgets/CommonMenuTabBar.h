// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Compound/TabBar.h"
#include "Nodes/TextBox.h"
#include "CommonMenuTabBar.generated.h"

namespace Jafg
{

struct LFactoryCommonMenuTabBar;

DECLARE_JAFG_WIDGET()
class WCommonMenuTabBarPanel : public WTabBarPanel
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WCommonMenuTabBarPanel)

    virtual void Construct() override;

    FORCEINLINE bool IsTabBarPanelValid() const { return this->Panel != nullptr; }
    FORCEINLINE auto GetTabBarPanel() -> WNode* { return this->Panel; }
    FORCEINLINE auto GetTabBarPanel() const -> const WNode* { return this->Panel; }
    FORCEINLINE auto GetTabBarPanelChecked() -> WNode* { check( this->Panel ); return this->Panel; }
    FORCEINLINE auto GetTabBarPanelChecked() const -> const WNode* { check( this->Panel ); return this->Panel; }

private:

    WRegion* Panel{};
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryCommonMenuTabBar)
class WCommonMenuTabBar : public WTabBar
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WCommonMenuTabBar)

public:

    virtual void Construct() override;

    //# Before constructing the widget only.
    FORCEINLINE void SetDoBlurBackground(const bool bInBlur) { this->bBlur = bInBlur; }
    //# Before constructing the widget only.
    FORCEINLINE void SetTabBarDepth(const i32 InDepth) { this->Depth = InDepth; }
    FORCEINLINE i32  GetTabBarDepth() const { return this->Depth; }
    FORCEINLINE i32  GetLeafDepth() const { return this->Depth + 1; }

    FORCEINLINE static constexpr u8 GetAlphaTintBasedOfDepth(const i32 InDepth)
    {
        return static_cast<u8>(maths::clamp<i32>(InDepth * 55 + 128, 128, 255));
    }

private:

    bool bBlur{};
    i32 Depth{};
};

DECLARE_JAFG_WIDGET()
class WCommonMenuTabBarButton : public WTabBarButton
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WCommonMenuTabBarButton)
    {
        this->SetNormalBrush({});
        this->SetHoverBrush({{0, 0, 0, 128}});
        this->SetPressBrush({{0, 0, 0, 192}});

        this->SetPadding({2, 4});
        this->SetMinDesiredSize(5_pt2);
        this->SetAnchor(EAnchor::HFill);
    }

public:

    virtual void Construct() override;
};

struct LFactoryCommonMenuTabBar : NODE_FACTORY_PARENT(WCommonMenuTabBar)
{
    NODE_FACTORY_BODY(WCommonMenuTabBar)

    decltype(auto) BlurBackground(this auto&& Self, const bool bInBlur)
    {
        NODE_FACTORY_SELF().SetDoBlurBackground(bInBlur);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
