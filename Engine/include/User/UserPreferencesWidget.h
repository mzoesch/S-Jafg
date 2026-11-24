// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/GenericTabInfos.h"
#include "User/PreferenceValue.h"
#include "UserPreferencesWidget.generated.h"

namespace Jafg
{

class WTextButton;
class WText;
class WBox;
class WHParent;
class WVParent;
class WHButton;
class WScrollRegion;
class WEditableTextButton;

DECLARE_JAFG_WIDGET()
class ENGINE_API WUserPreferencesWidget : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WUserPreferencesWidget)
    {
        this->SetShouldTick(true);
    }

public:

    JAFG_DEFAULT_TAB_CANDIDATE("Preferences", JafgInfo, "Icons/Preferences")

    virtual void Construct() override;
    virtual void Destruct() override;
    virtual void Tick(f32 Dt) override;

    TArray<LPreferenceValueChangeRequest> Changes;

private:

    //# A node that the search may filter out.
    struct LSearchEntry
    {
        WNode* Node{};
        //# Set if the entry is a value.
        LPreferenceValue* Value{};
        //# The row of a value, or the header row of a collection. Painted with the proximity palette.
        WHButton* Row{};
        //# Collections only: the icon that shows whether the collection is collapsed.
        WBox* CollapseIcon{};
        //# Collections only: identifies the collection in #CollapsedGroups.
        LString CollapseKey;
        //# The visibility of #Node when it is not filtered out.
        ENodeVisibility ShownVisibility{};
        //# Lower case text that the search tokens are matched against.
        LString Haystack;
        //# A collection is shown if any of its children is shown. A value is shown if all tokens matched.
        bool bCollection{};
        TArray<LSearchEntry> Children;
    };

    //# A top level collection.
    struct LSection
    {
        LSearchEntry Entry;
        WTextButton* Button{};
    };

    //# Rebuilds all nodes. Keeps the selected section and the search query.
    void Rebuild();
    //# Lets a collection with dynamic content refresh itself before its nodes are built.
    void PrepareCollection(std::size_t Index);
    //# Builds the node of a section and inserts it into the content at the position.
    NODISCARD LSearchEntry BuildSectionNode(std::size_t Index, std::size_t Position);
    //#
    //# Adds the nodes for a preference to the target. Depth is one for the direct children of a section.
    //# The parent key is the collapse key of the collection the preference is in.
    //#
    NODISCARD LSearchEntry AddPreference(WParent& Target, LPreference& Preference, std::size_t Depth, LStringView ParentKey);
    //# Refreshes the collection of a section and rebuilds its nodes. Keeps the pending changes of the section.
    void RebuildSection(std::size_t Index);
    //# Removes all nodes and rebuilds them, while keeping the current scroll position.
    void RebuildInPlace();
    //# Must not be called while searching.
    void SelectSection(std::size_t Index);
    //# Called when the user changed the search query.
    void OnSearchQueryChanged(LStringView Query);
    //# Applies the selected section, the search tokens and the collapsed collections to the visibility of all nodes.
    void RefreshVisibility();
    //# Alternates the colors of the visible rows of each section.
    void RepaintRows();
    //# Shows a note next to the apply button, if any pending change requires a restart.
    void UpdateRestartNote();

    NODISCARD FORCEINLINE bool IsSearching() const noexcept { return !this->SearchTokens.empty(); }

    TArray<LSection> Sections;
    //# The scroll position of each section when it was last left. Survives rebuilds.
    TArray<LVec2F> SectionScrollPositions;
    std::size_t SelectedSection{};

    //# The query as typed by the user. Survives rebuilds.
    LString SearchQuery;
    //# The lower case, whitespace separated tokens of #SearchQuery. Empty if not searching.
    TArray<LString> SearchTokens;

    //# The collapse keys of the collections whose children are hidden. Survives rebuilds.
    TArray<LString> CollapsedGroups;

    //# Sections whose collections requested a rebuild. Processed at the end of the tick.
    TArray<std::size_t> PendingSectionRebuilds;

    WScrollRegion* ScrollRegion{};
    WVParent* Content{};
    WEditableTextButton* SearchBox{};
    WText* NoResultsText{};
    WHParent* RestartNote{};
    WBox* RestartNoteIcon{};
    WText* RestartNoteText{};
    WTextButton* ApplyButton{};
    WTextButton* DiscardButton{};
};

} /* ~Namespace Jafg */
