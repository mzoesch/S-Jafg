// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Compound/TabBar.h"
#include "Widgets/TextBox.h"
#include "CommonMenuTabBar.generated.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryCommonMenuTabBar : public TWidgetFactoryTabBar<TNode>
{
public:

    using Super         = TWidgetFactoryTabBar<TNode>;
    using TFactoryRetTy = typename Super::TFactoryRetTy;

    FORCEINLINE TFactoryRetTy& BlurBackground(const bool bInBlur) { this->This()->SetDoBlurBackground(bInBlur); return this->Self(); }
};

DECLARE_JAFG_WIDGET()
class WCommonMenuTabBarButton : public WTabBarButton
{
    GENERATED_CLASS_BODY()

protected:

    explicit WCommonMenuTabBarButton(LCxxObjectInitializer const& ObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(WCommonMenuTabBarButton)

public:

    virtual void Construct() override;
};

DECLARE_JAFG_WIDGET()
class WCommonMenuTabBarPanel : public WTabBarPanel
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WCommonMenuTabBarPanel)

    virtual void Construct() override;

    FORCEINLINE bool IsTabBarPanelValid() const { return this->Panel != nullptr; }
    FORCEINLINE auto GetTabBarPanel() -> WNode* { return this->Panel; }
    FORCEINLINE auto GetTabBarPanel() const -> const WNode* { return this->Panel; }
    FORCEINLINE auto GetTabBarPanelChecked() -> WNode* { check( this->Panel ); return this->Panel; }
    FORCEINLINE auto GetTabBarPanelChecked() const -> const WNode* { check( this->Panel ); return this->Panel; }

private:

    WRegion* Panel{ nullptr };
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryCommonMenuTabBar)
class WCommonMenuTabBar : public WTabBar
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WCommonMenuTabBar)

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
        return Maths::ClampRet<u8, i32>(InDepth * 55 + 128, 128, 255);
    }

private:

    bool bBlur{ false };
    i32 Depth{ 0 };
};

} /* ~Namespace Jafg */
