// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Overlay.h"
#include "UserWidget.generated.h"

namespace Jafg
{

class LViewport;
class WParent;

//#
//# A user widget is a widget node that can be added to the local ego widget viewport.
//# A user widget can consist of multiple widget nodes and can be used to create complex
//# user interfaces.
//#
DECLARE_JAFG_WIDGET()
class ENGINE_API WUserWidget : public WOverlay
{
    GENERATED_CLASS_BODY()

protected:

    explicit WUserWidget(const LObjectInitializer& ObjectInitializer);

public:

    virtual void OnGarbage() override;
    virtual void Destruct() override;

    virtual auto GetViewport() const -> LViewport* override;
    virtual void RemoveFromParent(const bool bDestroy = true) override;

    virtual void RemoveChild(WNode* InChild) override;
    using Super::RemoveChild;
    virtual auto AddChild(WNode* InChild) -> LWidgetSlot* override;
    virtual auto AddChildAt(const i32 InIndex, WNode* InChild) -> LWidgetSlot* override;

    //# Add this widget to the main viewport of the current active local ego.
    void AddToViewport(LViewport* InViewport);
    void AddToViewportAt(const i32 InIndex, LViewport* InViewport);

    //# @return The new root.
    template <typename TParent>
    auto ReplaceRoot(TParent& InRoot) -> TParent* { return CheckedStaticCast<TParent>(this->ReplaceRootImpl(InRoot)); }
    FORCEINLINE bool IsRootValid() const { return this->Root != nullptr; }
    FORCEINLINE auto GetRoot() const -> WNode* { return this->Root->Content;  }
    template <typename TRootTy>
    FORCEINLINE auto GetRoot() const -> TRootTy* { return static_cast<TRootTy*>(this->Root->Content); }

private:

    WParentBase* ReplaceRootImpl(WParentBase& InRoot);

    //# The absolute root of this widget. Attach everything to this widget. Weak pointer.
    LWidgetSlot* Root = nullptr;
    //# Where this widget resides in. Can be null if attached to another widget. So do not use without checking.
    LViewport* AttachedViewport = nullptr;
};

} /* ~Namespace Jafg */
