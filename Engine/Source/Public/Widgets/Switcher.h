// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Overlay.h"
#include "Switcher.generated.h"

namespace Jafg
{

//#
//# A widget switcher has widgets that can be switched between like tabs - at maximum one widget is visible at a time.
//#
DECLARE_JAFG_WIDGET()
class WSwitcher : public WOverlay
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WSwitcher)

public:

    enum { NoActiveWidgetIndex = INDEX_NONE };

    void SetActiveWidgetIndex(const i32 Index);
    FORCEINLINE void ResetWidgetIndex() { this->SetActiveWidgetIndex(NoActiveWidgetIndex); }
    FORCEINLINE i32  GetActiveWidgetIndex() const { return this->ActiveIndex; }

    void SetActiveWidget(WNode* Widget);
    FORCEINLINE WNode* GetActiveWidget() const noexcept
    {
        if (algo::is_valid_index(this->GetChildren(), this->ActiveIndex))
        {
            return this->GetChildren()[this->ActiveIndex]->Content;
        }
        return nullptr;
    }
    FORCEINLINE WNode* GetActiveWidgetChecked() const { const bool bOut = this->GetActiveWidget(); check( bOut ) return this->GetActiveWidget(); }
    FORCEINLINE WNode* GetActiveWidgetAsserted() const { const bool bOut = this->GetActiveWidget(); jassert( bOut ) return this->GetActiveWidget(); }

    FORCEINLINE bool IsIndexValid() const { return algo::is_valid_index(this->GetChildren(), this->ActiveIndex); }
    FORCEINLINE auto GetActiveNode() -> WNode* { return this->IsIndexValid() ? this->GetChildren()[this->ActiveIndex]->Content : nullptr; }
    FORCEINLINE auto GetActiveNode() const -> const WNode* { return this->IsIndexValid() ? this->GetChildren()[this->ActiveIndex]->Content : nullptr; }
    FORCEINLINE auto GetActiveNodeChecked() -> WNode* { check(this->IsIndexValid()) return this->GetChildren()[this->ActiveIndex]->Content; }
    FORCEINLINE auto GetActiveNodeChecked() const -> const WNode* { check(this->IsIndexValid()) return this->GetChildren()[this->ActiveIndex]->Content; }

    // WParent implementation
    virtual LWidgetSlot* AddChild(WNode* InChild) override;
    virtual LWidgetSlot* AddChildAt(const i32 InIndex, WNode* InChild) override;
    // ~WParent implementation

private:

    i32 ActiveIndex{ NoActiveWidgetIndex };
    struct LRecentVisibility
    {
        const void* Target{};
        ENodeVisibility Visibility{ ENodeVisibility::Visible };
    };
    TArray<LRecentVisibility> RecentVisibilities;
};

} /* ~Namespace Jafg */
