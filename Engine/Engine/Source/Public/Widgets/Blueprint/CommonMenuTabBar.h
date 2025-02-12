// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Compound/TabBar.h"
#include "Widgets/TextBlock.h"
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
class ENGINE_API WCommonMenuTabBarButton : public WTabBarButton
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WCommonMenuTabBarButton)

public:

    virtual void Construct() override;
};

DECLARE_JAFG_WIDGET()
class ENGINE_API WCommonMenuTabBarPanel : public WTabBarPanel
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WCommonMenuTabBarPanel)

    virtual void Construct() override;

    FORCEINLINE bool IsTabBarPanelValid() const { return this->Panel != nullptr; }
    FORCEINLINE auto GetTabBarPanel() -> WWidgetNode* { return this->Panel; }
    FORCEINLINE auto GetTabBarPanel() const -> const WWidgetNode* { return this->Panel; }
    FORCEINLINE auto GetTabBarPanelChecked() -> WWidgetNode* { check( this->Panel ); return this->Panel; }
    FORCEINLINE auto GetTabBarPanelChecked() const -> const WWidgetNode* { check( this->Panel ); return this->Panel; }

private:

    WWidgetRegion* Panel = nullptr;
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryCommonMenuTabBar)
class ENGINE_API WCommonMenuTabBar : public WTabBar
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WCommonMenuTabBar)

public:

    virtual void Construct() override;

    //# Before constructing the widget only.
    FORCEINLINE void SetDoBlurBackground(const bool bInBlur) { this->bBlur = bInBlur; }
    //# Before constructing the widget only.
    FORCEINLINE void SetTabBarDepth(const int32 InDepth) { this->Depth = InDepth; }
    FORCEINLINE auto GetTabBarDepth() const -> int32 { return this->Depth; }
    FORCEINLINE int32 GetLeafDepth() const { return this->Depth + 1; }

    FORCEINLINE static constexpr uint8 GetAlphaTintBasedOfDepth(const int32 InDepth)
    {
        return Maths::ClampRet<uint8, int32>(InDepth * 32 + 128, 128, 255);
    }

private:

    bool bBlur = false;
    int32 Depth = 0;
};

} /* ~Namespace Jafg */
