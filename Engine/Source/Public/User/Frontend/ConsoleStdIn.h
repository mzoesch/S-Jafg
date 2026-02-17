// Copyright mzoesch. All rights reserved.

#pragma once

#include "Cli/CliCommand.h"
#include "Widgets/EditableTextBoxForward.h"
#include "Widgets/Overlay.h"
#include "ConsoleStdIn.generated.h"

namespace Jafg
{

class WEditableTextBox;
class WScrollRegion;
class WVRegion;
class WRegion;
class JConsoleStdInPreferences;
class WConsoleStdIn;

DECLARE_JAFG_CLASS(ECxxClassFlags::Config, ECxxClassFlags::Singleton)
class JConsoleStdInPreferences : public JCxxClass
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JConsoleStdInPreferences)

public:

    CLASS_FIELD(Config)
    TPreference<u32> MaxHistorySize{ 50, 5, {} };

    CLASS_FIELD(Config)
    TArray<LString> History;

    CLASS_FIELD(Config)
    TPreference<u32> MaxIntellisensePredictions{ 500, 5, {} };

    CLASS_FIELD(Config)
    TPreference<LColor> IntellisenseTint { Colors::LightSlateGray };

    CLASS_FIELD(Config)
    TPreference<LColor> IntellisenseHighlightTint{ Colors::DarkSlateGray };

};

DECLARE_JAFG_WIDGET(ECxxClassFlags::Config)
class WConsoleStdIn : public WOverlay
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WConsoleStdIn)

public:

    virtual void Construct() override;
    LReply OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;

    //# @return True, whether to prevent default. Default is to clear the text box.
    EVENT_DECL(OnTextCommit, bool, LString const& Text, ETextCommit CommitType)

    void AddToHistory(LString const& Text);

    FORCEINLINE WEditableTextBox* GetEditableTextBox() noexcept { return this->EditableTextBox; }
    FORCEINLINE WEditableTextBox const* GetEditableTextBox() const noexcept { return this->EditableTextBox; }

    FORCEINLINE void ResetHistoryCursor() noexcept { this->HistoryCursor = INDEX_NONE; }

private:

    void OnCommit(LString const& Text, ETextCommit CommitType);
    void OnChanged(LString const& Text);

    void AddIntellisensePrediction(LString Text);
    void AddIntellisense(LString Text);

    void UpdateIntellisense(LCliCommand const* TargetCommand = nullptr);

    bool TryGoIntellisensePredictionUp();
    bool TryGoIntellisensePredictionDown();
    void ApplyCurrentIntellisensePrediction();
    void GoHistoryBack();
    void GoHistoryForward();


    bool IsCurrentSelectedIntellisensePredictionValid() const;

    bool bIgnoreNonEnterCommits:1{};
    bool bIgnoreEmptyEnterCommits:1{};
    bool bPrefixCommand:1{};
    bool bIgnoreOnChangedCall:1{};

    //#
    //# The cursor of the history from back to front.
    //# Meaning 0 => The most recent history entry. The last entry in the array.
    //# Meaning n => The n-th history entry. The n-th entry in the array when counting from the back.
    //#
    i32 HistoryCursor{ INDEX_NONE };
    FORCEINLINE bool IsHistoryCursorValid() const noexcept
    {
        check(this->HistoryCursor == INDEX_NONE ? true : algo::is_valid_index(GetSingleton<JConsoleStdInPreferences>().History, this->HistoryCursor))
        return this->HistoryCursor != INDEX_NONE;
    }
    FORCEINLINE i32 GetIndexInHistory() const noexcept
    {
        if (this->HistoryCursor == INDEX_NONE)
        {
            return INDEX_NONE;
        }
        TArray<LString> const& DefaultHistory{GetSingleton<JConsoleStdInPreferences>().History};
        check(algo::is_valid_index(DefaultHistory, DefaultHistory.size() - 1 - this->HistoryCursor))
        return DefaultHistory.size() - 1 - this->HistoryCursor;
    }
    FORCEINLINE LString const* GetCurrentHistoryItem() const noexcept
    {
        if (this->HistoryCursor == INDEX_NONE)
        {
            return nullptr;
        }
        return &(GetSingleton<JConsoleStdInPreferences>().History[this->GetIndexInHistory()]);
    }
    FORCEINLINE LString const* GetCurrentHistoryItemChecked() const { check( this->IsHistoryCursorValid() ) return this->GetCurrentHistoryItem(); }
    FORCEINLINE LString const* GetCurrentHistoryItemCheckedAssert() const { jassert( this->IsHistoryCursorValid() ) return this->GetCurrentHistoryItem(); }

    //# The intellisense regions that are used if a command is being typed.
    WVRegion* IntellisenseContainer{};
    WVRegion* Intellisense{};
    WRegion*  IntellisenseHelpContainer{};
    WTextBox* IntellisenseHelp{};
    WTextBox* IntellisenseText{};
    WVRegion* IntellisensePredictions{};
    LString CurrentIntellisensePrediction;

    WEditableTextBox* EditableTextBox{};
};

} /* ~Namespace Jafg */
