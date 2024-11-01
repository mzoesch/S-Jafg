// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetNode.h"
#include "WidgetParentBase.generated.h"

namespace Jafg
{

/**
 * Pure virtual abstraction of a widget parent.
 * To let other widgets implement their own data structure for children.
 * TODO: Please think of a better name for this class.
 */
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class RHI_API WWidgetParentBase : public WWidgetNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WWidgetParentBase)

public:

    virtual auto GetChildren()             const   -> const TdhArray<LWidgetSlot*>& PURE_VIRTUAL(return TdhArray<LWidgetSlot*>())
    virtual auto RemoveChild(WWidgetNode* InChild) -> void PURE_VIRTUAL()
    virtual auto RemoveChild(LWidgetSlot* InSlot)  -> void PURE_VIRTUAL()
    virtual auto AddChild(WWidgetNode* InChild)    -> LWidgetSlot* PURE_VIRTUAL(return nullptr)

    virtual auto GetPaddingPtr() const -> const LPadding* PURE_VIRTUAL(return nullptr)
    virtual auto GetPaddingPtr()       ->       LPadding* PURE_VIRTUAL(return nullptr)

    virtual auto SetPadding(const LPadding& InPadding) -> WWidgetParentBase& PURE_VIRTUAL(return *this)

    ///////////////////////////////////////////////////////////////////////////////
    // Wsdsml
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE auto operator[](WWidgetNode* InChild) -> WWidgetParentBase& { this->AddChild( InChild); return *this; }
    FORCEINLINE auto operator[](WWidgetNode& InChild) -> WWidgetParentBase& { this->AddChild(&InChild); return *this; }

    FORCEINLINE auto operator&(const LPadding& InPadding) -> WWidgetParentBase& { this->SetPadding(InPadding); return *this; }

    ///////////////////////////////////////////////////////////////////////////////
    // ~Wsdsml
    ///////////////////////////////////////////////////////////////////////////////
};

} /* ~Namespace Jafg. */
