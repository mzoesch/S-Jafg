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
    virtual void Tick(f32 Dt) override;

    TArray<LPreferenceValueChangeRequest> Changes;

private:

    //# A node that the search may filter out.
    struct LSearchEntry
    {
        WNode* Node{};
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
    //# Removes all nodes and rebuilds them, while keeping the current scroll position.
    void RebuildInPlace();
    //# Must not be called while searching.
    void SelectSection(std::size_t Index);
    //# Called when the user changed the search query.
    void OnSearchQueryChanged(LStringView Query);
    //# Applies the selected section and the search tokens to the visibility of all nodes.
    void RefreshVisibility();
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

    WScrollRegion* ScrollRegion{};
    WEditableTextButton* SearchBox{};
    WText* NoResultsText{};
    WHParent* RestartNote{};
    WBox* RestartNoteIcon{};
    WText* RestartNoteText{};
    WTextButton* ApplyButton{};
    WTextButton* DiscardButton{};
};

} /* ~Namespace Jafg */
