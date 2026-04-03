// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Overlay.h"
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

    enum { NoActiveNodeIndex = INDEX_NONE };

    virtual void AddChildAt(u64 InIndex, TJxxUnique<WNode> InChild) override;

    void SetActiveNode(WNode const& Node);
    void SetActiveNodeByIndex(i64 Index);

    FORCEINLINE constexpr bool IsActiveNodeValid() const noexcept
    {
        checkCode(if (algo::is_valid_index(this->GetChildren(), this->ActiveNodeIndex)) { check(this->GetChildren()[this->ActiveNodeIndex].get()) })
        return algo::is_valid_index(this->GetChildren(), this->ActiveNodeIndex);
    }
    FORCEINLINE constexpr i64 GetActiveNodeIndex() const noexcept { return this->ActiveNodeIndex; }
    FORCEINLINE WNode* GetActiveNode() noexcept
    {
        if (this->IsActiveNodeValid())
        {
            return &*this->GetChildren()[this->ActiveNodeIndex];
        }
        return nullptr;
    }
    FORCEINLINE WNode const* GetActiveNode() const noexcept
    {
        if (this->IsActiveNodeValid())
        {
            return &*this->GetChildren()[this->ActiveNodeIndex];
        }
        return nullptr;
    }
    FORCEINLINE WNode* GetActiveNodeChecked() noexcept { auto* Widget{this->GetActiveNode()}; check(Widget) return Widget; }
    FORCEINLINE WNode const* GetActiveNodeChecked() const noexcept { auto const* Widget{this->GetActiveNode()}; check(Widget) return Widget; }
    FORCEINLINE WNode* GetActiveNodeAsserted() noexcept { auto* Widget{this->GetActiveNode()}; jassert(Widget) return Widget; }
    FORCEINLINE WNode const* GetActiveNodeAsserted() const noexcept { auto const* Widget{this->GetActiveNode()}; jassert(Widget) return Widget; }

private:

    i64 ActiveNodeIndex{ NoActiveNodeIndex };
    TArray<std::pair<void const*, ENodeVisibility>> RecentVisibilities;
};

} /* ~Namespace Jafg */
