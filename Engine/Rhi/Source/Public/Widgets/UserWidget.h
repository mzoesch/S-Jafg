// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetParentBase.h"
#include "UserWidget.generated.h"

namespace Jafg
{

class LViewport;
class WWidgetParent;

/**
 * A user widget is a widget node that can be added to the local ego widget viewport.
 * A user widget can consist of multiple widget nodes and can be used to create complex
 * user interfaces.
 */
DECLARE_JAFG_CLASS()
class RHI_API WUserWidget : public WWidgetParentBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WUserWidget)

public:

    // WWidgetNode implementation
    virtual void Draw(LViewport& Context) const override;
    virtual auto GetViewportSize() const -> LIntVector2 override;
    virtual auto RemoveFromParent(const bool bDestroy = true) -> void override;
    // ~WWidgetNode implementation

    // WWidgetParentBase implementation
    FORCEINLINE virtual auto GetChildren() const -> const TdhArray<LWidgetSlot*>& override { return this->SingleRootChild; }
                virtual auto RemoveChild(WWidgetNode* InChild) -> void override;
                virtual auto RemoveChild(LWidgetSlot* InSlot) -> void override;
    FORCEINLINE virtual auto AddChild(WWidgetNode* InChild) -> LWidgetSlot* override NON_CALLABLE_MEMBER(return nullptr)
    FORCEINLINE virtual auto GetPaddingPtr() const -> const LPadding* override { return &this->Padding; }
    FORCEINLINE virtual auto GetPaddingPtr() -> LPadding* override { return &this->Padding; }
    FORCEINLINE virtual auto SetPadding(const LPadding& InPadding) -> WWidgetParentBase& override { this->Padding = InPadding; return *this; }
    // ~WWidgetParentBase implementation

    /** Add this widget to the main viewport of the current active local ego. */
    void AddToViewport(LViewport* InViewport);

    /** @return The new root. */
    template <typename TParent>
    auto ReplaceRoot(TParent& InRoot) -> TParent* { return static_cast<TParent*>(this->ReplaceRootImpl(InRoot)); }
    FORCEINLINE auto HasRoot() const -> bool { return this->Root != nullptr; }
    FORCEINLINE auto GetRoot() const -> WWidgetNode* { return this->Root->Content;  }
    template <typename TRootTy>
    FORCEINLINE auto GetRoot() const -> TRootTy* { return static_cast<TRootTy*>(this->Root->Content); }

private:

    WWidgetParent* ReplaceRootImpl(WWidgetParent& InRoot);

    /** The absolute root of this widget. Attach everything to this widget. */
    LWidgetSlot* Root = nullptr;
    TdhArray<LWidgetSlot*> SingleRootChild;
    /** Where this widget resides in. Can be null if attached to another widget. So do not use without checking. */
    LViewport* AttachedViewport = nullptr;

    /** The padding area between the slot and the content it contains. */
    LPadding Padding;
};

template <typename TWidget>
FORCEINLINE auto ConstructUserWidget(Private::LObjectContext* InContext) -> TWidget*
{
    return NewObject<TWidget>(InContext);
}

} /* ~Namespace Jafg */
