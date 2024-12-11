// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetNode.h"

namespace Jafg
{

class LViewport;
class WUserWidget;
struct LSubsystemCollection;

/**
 * The hud is owned by the local ego and shares its lifetime.
 * The hud is the main hub for all user interface elements. Create hud subsystems to automatically
 * add multiple widgets of a type to the viewport.
 */
class ENGINE_API LHud final
{
public:

    LHud() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LHud)
    ~LHud() = default;

    void Initialize(Private::LObjectContext* InOuter);
    void Tick(const LSurface& Surface, const bool bCheckInput);
    void TearDown();

    auto GetMainViewport() const -> LViewport* ;

    void AddWidget(WUserWidget* Widget) const;
    void RemoveWidget(WUserWidget* Widget) const;

    auto GetTopLevelWidgetByClass(const LObjectClass* WidgetClass) const -> WWidgetNode*;
    auto GetCheckedTopLevelWidgetByClass(const LObjectClass* WidgetClass) const -> WWidgetNode*;
    template <typename TNode> FORCEINLINE auto GetTopLevelWidgetByClass() const -> TNode*;
    template <typename TNode> FORCEINLINE auto GetCheckedTopLevelWidgetByClass() const -> TNode*;

    /**
     * Change the visibility of a top level widget.
     * @param InVisibility   The new visibility state of the widget.
     * @param bAllowNotFound If true, nothing happens if the widget was not found. If false, the program will panic.
     * @return True, if widget was found and made visible. False if widget was not found or was already visible.
     */
    template <typename TNode>
    UNUSED FORCEINLINE bool ChangeWidgetVisibility(const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound = false) const
    {
        static_assert(std::derived_from<TNode, WWidgetNode>, "TNode must derive from WWidgetNode.");
        return this->ChangeWidgetVisibility(TNode::StaticClass(), InVisibility, bAllowNotFound);
    }
    UNUSED bool ChangeWidgetVisibility(const LObjectClass* WidgetClass, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound = false) const;

private:

    Private::LObjectContext* Outer;
    LSubsystemCollection*    Collection;
};

template <typename TNode>
TNode* LHud::GetTopLevelWidgetByClass() const
{
    static_assert(std::derived_from<TNode, WWidgetNode>, "TNode must derive from WWidgetNode.");
    return CheckedStaticCast<TNode, true>(this->GetTopLevelWidgetByClass(TNode::StaticClass()));
}

template <typename TNode>
TNode* LHud::GetCheckedTopLevelWidgetByClass() const
{
    static_assert(std::derived_from<TNode, WWidgetNode>, "TNode must derive from WWidgetNode.");
    return CheckedStaticCast<TNode>(this->GetTopLevelWidgetByClass(TNode::StaticClass()));
}

} /* ~Namespace Jafg. */
