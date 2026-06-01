// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Overlay.h"
#include "Switcher.generated.h"

namespace Jafg
{

//# A widget switcher has widgets that can be switched between like tabs - at maximum one widget is visible at a time.
DECLARE_JAFG_WIDGET()
class WSwitcher : public WOverlay
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WSwitcher)

public:

    virtual WNode& OnAddChild(std::size_t Index, TJxxUnique<WNode> Child, bool bConstructed) override;
    virtual void OnRemoveChildPrepare(WNode& Child) override;
    virtual void RemoveChildren() override { this->ResetActiveNode(); Super::RemoveChildren(); }

    void SetActiveNode(WNode& Node);
    void SetActiveNodeByIndex(u64 Index)
    {
        check(algo::valid_index(this->GetChildren(), Index))
        this->SetActiveNode(*this->GetChildren()[Index]);
    }
    void ResetActiveNode();

    FORCEINLINE constexpr bool IsActiveNodeValid() const noexcept { return !!this->ActiveNode; }

    FORCEINLINE std::optional<std::size_t> GetActiveNodeIndex() const noexcept
    {
        if (this->ActiveNode)
        {
            return algo::distance_to(this->GetChildren(), this->ActiveNode, algo::unique_raw);
        }
        return {};
    }
    FORCEINLINE WNode* GetActiveNode() noexcept { return this->ActiveNode; }
    FORCEINLINE WNode const* GetActiveNode() const noexcept { return this->ActiveNode; }
    FORCEINLINE WNode* GetActiveNodeChecked() noexcept { check(this->ActiveNode) return this->ActiveNode; }
    FORCEINLINE WNode const* GetActiveNodeChecked() const noexcept { check(this->ActiveNode) return this->ActiveNode; }
    FORCEINLINE WNode* GetActiveNodeAsserted() noexcept {jassert(this->ActiveNode) return this->ActiveNode; }
    FORCEINLINE WNode const* GetActiveNodeAsserted() const noexcept { jassert(this->ActiveNode) return this->ActiveNode; }

private:

    WNode* ActiveNode{};
    TArray<std::pair<WNode*, ENodeVisibility>> RecentVisibilities;
};

} /* ~Namespace Jafg */
