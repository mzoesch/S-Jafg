// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetParent.h"
#include "WidgetSwitcher.generated.h"

namespace Jafg
{

//#
//# A widget switcher has widgets that can be switched between like tabs - at maximum one widget is visible at a time.
//#
DECLARE_JAFG_WIDGET()
class ENGINE_API WWidgetSwitcher : public WWidgetParent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WWidgetSwitcher)

public:

    enum { NoActiveWidgetIndex = -1 };

    FORCEINLINE void  ResetWidgetIndex() { this->ActiveIndex = NoActiveWidgetIndex; }
    FORCEINLINE int32 GetActiveWidgetIndex() const { return this->ActiveIndex; }
                void  SetActiveWidgetIndex(const int32 Index);

    FORCEINLINE auto GetActiveWidget() const -> WWidgetNode*;
    FORCEINLINE auto GetCheckedActiveWidget() const -> WWidgetNode*;
    FORCEINLINE auto GetPanickedActiveWidget() const -> WWidgetNode*;
                void SetActiveWidget(WWidgetNode* Widget);

    // WWidgetParentBase implementation
    virtual void Tick() override;
    virtual auto SweepMouse(LViewport& Context, const LVector2& InLocation) -> LCursorReply override;
    virtual auto SweepFocusTest(LViewport& Context, const LVector2& InLocation) -> LReply override;
    virtual bool IsFocusWidgetTransitive(const LViewport* InViewport) const override;
    virtual void UpdateDesiredSize() const override;
    virtual void UpdateAnchoredSize(const LViewport& Context) const override;
    // ~WWidgetParentBase implementation

    // WWidgetParent implementation
    virtual void Draw(LViewport& Context) const override;
    virtual void RemoveChild(WWidgetNode* InSlot) override;
    virtual auto AddChild(WWidgetNode* InChild) -> LWidgetSlot* override;
    // ~WWidgetParent implementation

private:

    int32 ActiveIndex = NoActiveWidgetIndex;
};

WWidgetNode* WWidgetSwitcher::GetActiveWidget() const
{
    return this->GetChildren().IsValidIndex(NoActiveWidgetIndex)
        ? this->GetChildren()[this->ActiveIndex]->Content
        : nullptr;
}

WWidgetNode* WWidgetSwitcher::GetCheckedActiveWidget() const
{
    WWidgetNode* Out = this->GetActiveWidget();
    check( Out )
    return Out;
}

WWidgetNode* WWidgetSwitcher::GetPanickedActiveWidget() const
{
    if (WWidgetNode* Out = this->GetActiveWidget(); Out)
    {
        return Out;
    }
    panic( "No active widget found." )
    return nullptr;
}

} /* ~Namespace Jafg */
