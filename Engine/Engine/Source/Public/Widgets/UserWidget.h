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
class WUserWidget : public WOverlay
{
    GENERATED_CLASS_BODY()

protected:

    explicit WUserWidget(LCxxObjectInitializer const& CxxObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(WUserWidget)

public:

    virtual void OnGarbage(ECxxRecordTearDownReason::Type Reason) override;
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
    TParent* ReplaceRoot(TParent* InRoot) { return StaticCastChecked<TParent>(this->ReplaceRootImpl(InRoot)); }
    FORCEINLINE bool IsRootValid() const { return this->Root != nullptr; }
    FORCEINLINE WNode* GetRoot() const { return this->Root->Content;  }
    template <typename TNode>
    FORCEINLINE TNode* GetRoot() const { return StaticCastChecked<TNode>(this->Root->Content); }

private:

    WParentBase* ReplaceRootImpl(WParentBase* InRoot);

    //# The absolute root of this widget. Attach everything to this widget. Weak pointer.
    LWidgetSlot* Root { nullptr };
    //# Where this widget resides in. Can be null if attached to another widget. So do not use without checking.
    LViewport* AttachedViewport { nullptr };
};

} /* ~Namespace Jafg */
