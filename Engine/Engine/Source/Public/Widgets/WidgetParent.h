// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetParentBase.h"
#include "WidgetParent.generated.h"

namespace Jafg
{

/**
 * The base class for all nodes that can possess children.
 */
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API WWidgetParent : public WWidgetParentBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WWidgetParent)

public:

    virtual void OnGarbage() override;
    virtual void Construct() override;
    virtual void Draw(LViewport& Context) const override;
    virtual void Destruct() override;

    FORCEINLINE virtual auto GetChildren() const -> const TdhArray<LWidgetSlot*>& override { return this->Children; }
    virtual auto RemoveChild(WWidgetNode* Child) -> void override;
    virtual auto RemoveChild(LWidgetSlot* Child) -> void override;
    virtual auto AddChild(WWidgetNode* Child) -> LWidgetSlot* override;

    FORCEINLINE         auto GetPadding()    const -> const LPadding& { return this->Padding; }
    FORCEINLINE virtual auto GetPaddingPtr() const -> const LPadding* override { return &this->Padding; }
    FORCEINLINE virtual auto GetPaddingPtr()       ->       LPadding* override { return &this->Padding; }

    FORCEINLINE auto SetPadding(const LPadding& InPadding) -> WWidgetParentBase& override { this->Padding = InPadding; return *this; }

private:

    TdhArray<LWidgetSlot*> Children;

    /** The padding area between the slot and the content it contains. */
    LPadding               Padding;
};

} /* ~Namespace Jafg. */
