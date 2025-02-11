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

//#
//# Development only. Do not use.
//#
DECLARE_JAFG_WIDGET()
class ENGINE_API WDevelopmentTabBarPanelPlaceholder : public WTabBarPanel
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WDevelopmentTabBarPanelPlaceholder)

    virtual void Construct() override;

    WWidgetNode* Panel = nullptr;
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

private:

    bool bBlur = false;
    int32 Depth = 0;
};

} /* ~Namespace Jafg */
