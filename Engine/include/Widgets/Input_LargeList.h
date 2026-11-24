// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/TextButton.h"
#include "Widgets/Input_ListForward.h"
#include "Input_LargeList.generated.h"

namespace Jafg
{

class WParent;
class WScrollRegion;
class WText;
struct LFactoryInput_LargeList;

//#
//# Like #WInput_List, but for many items: The items of the selection window are in a scroll region, and a search bar
//# at the top of the selection window filters them.
//#
//# Every whitespace separated word of the search has to fuzzy match the display text or the identifier of an item
//# (@see TextSearch::FuzzyScore). E.g., "mt" finds ".../MetalPlate". The best matches are listed first. Matches in the
//# last path segment of the display text (the name of the item) rank higher.
//#
//# The search bar has the focus when the selection window opens:
//# - Enter selects the first (best) match.
//# - Escape clears the search. If the search is already empty, it closes the selection window.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryInput_LargeList)
class ENGINE_API WInput_LargeList : public WTextButtonIconizedDouble
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WInput_LargeList)
    {
        this->CtorLogic();
    }

public:

    virtual LNodeReply OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override;

    TArray<LInputListItem> Items;
    //# The list of the selection window is at most this high and scrolls beyond. Zero means unbound.
    LNodeSize1 MaxListHeight{ 240_spt };

    //# Called if the selected item changed.
    EVENT_DECL(OnValueChanged, void(LInputListItem const& Selected))

private:

    void CtorLogic();
    void OpenSelectionWindow(LNodeKeyEventInfo const& Info);
    void CloseSelectionWindow();
    //# Shows only the rows that match the query, the best first. Noop, if the selection window is not open.
    void Filter(LStringView Query);
    //# Closes the selection window and selects the item.
    void Select(LInputListItem Item);
    //# @return Whether there was a selectable match to select.
    bool SelectFirstMatch();

    TClassStorage<WNode> DeferredWidget;

    //# The nodes below are only valid while #DeferredWidget is.
    TClassStorage<WNode> SearchBar;
    WScrollRegion* ScrollRegion{};
    WParent* List{};
    WText* NoMatches{};
    struct LRow final
    {
        WNode* Node{};
        LInputListItem Item;
        //# What the row shows.
        LString Display;
        //# The last path segment of #Display.
        LString Name;
    };
    //# In the order of #Items.
    TArray<LRow> Rows;
    //# The indices of the rows that match the search, the best first.
    TArray<std::size_t> Order;
};

struct LFactoryInput_LargeList: NODE_FACTORY_PARENT(WInput_LargeList)
{
    NODE_FACTORY_BODY(WInput_LargeList)

    decltype(auto) Selected(this auto&& Self, LString Selected) noexcept
    {
        return std::forward<decltype(Self)>(Self).Content(std::move(Selected));
    }
    decltype(auto) Items(this auto&& Self, TArray<LInputListItem> Items) noexcept
    {
        NODE_FACTORY_SELF().Items = std::move(Items);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) MaxListHeight(this auto&& Self, LNodeSize1 Height) noexcept
    {
        NODE_FACTORY_SELF().MaxListHeight = Height;
        return NODE_FACTORY_RESULT();
    }

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnValueChanged, OnValueChanged)
};

} /* ~Namespace Jafg */
