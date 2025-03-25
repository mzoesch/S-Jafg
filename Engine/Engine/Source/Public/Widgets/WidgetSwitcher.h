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

    enum { NoActiveWidgetIndex = INDEX_NONE };

    FORCEINLINE void  ResetWidgetIndex() { this->SetActiveWidgetIndex(NoActiveWidgetIndex); }
    FORCEINLINE i32 GetActiveWidgetIndex() const { return this->ActiveIndex; }
                void  SetActiveWidgetIndex(const i32 Index);

    FORCEINLINE auto GetActiveWidget() const -> WWidgetNode*;
    FORCEINLINE auto GetCheckedActiveWidget() const -> WWidgetNode*;
    FORCEINLINE auto GetPanickedActiveWidget() const -> WWidgetNode*;
                void SetActiveWidget(WWidgetNode* Widget);

    FORCEINLINE bool IsIndexValid() const { return this->GetChildren().IsValidIndex(this->ActiveIndex); }
    FORCEINLINE auto GetActiveNode() -> WWidgetNode* { return this->IsIndexValid() ? this->GetChildren()[this->ActiveIndex]->Content : nullptr; }
    FORCEINLINE auto GetActiveNode() const -> const WWidgetNode* { return this->IsIndexValid() ? this->GetChildren()[this->ActiveIndex]->Content : nullptr; }
    FORCEINLINE auto GetActiveNodeChecked() -> WWidgetNode* { check( this->IsIndexValid() ) return this->GetChildren()[this->ActiveIndex]->Content; }
    FORCEINLINE auto GetActiveNodeChecked() const -> const WWidgetNode* { check( this->IsIndexValid() ) return this->GetChildren()[this->ActiveIndex]->Content; }

    // WWidgetParent implementation
    virtual auto AddChild(WWidgetNode* InChild) -> LWidgetSlot* override;
    virtual auto AddChildAt(const i32 InIndex, WWidgetNode* InChild) -> LWidgetSlot* override;
    virtual void UpdateDesiredSize() const override;
    // ~WWidgetParent implementation

private:

    i32 ActiveIndex = NoActiveWidgetIndex;
    struct LRecentVisibility
    {
        const void* Target = nullptr;
        EWidgetVisibility::Type Visibility = EWidgetVisibility::Visible;
    };
    TdhArray<LRecentVisibility> RecentVisibilities;
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
