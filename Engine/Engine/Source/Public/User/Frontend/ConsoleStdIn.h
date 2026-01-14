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

DECLARE_JAFG_WIDGET(ECxxClassFlags::Config)
class WConsoleStdIn : public WOverlay
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WConsoleStdIn)

public:

    virtual void Construct() override;
    LReply OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;

    //# @return True, whether to prevent default. Default is to clear the text box.
    EVENT_DECL(OnTextCommit, bool, LString const& Text, ETextCommit::Type CommitType)

    CLASS_FIELD(Config)
    TCdrIgnore<u32> MaxHistorySize{ 50 };

    CLASS_FIELD(Config)
    TCdrIgnore<TArray<LString>> History;

    CLASS_FIELD(Config)
    TCdrIgnore<u32> MaxIntellisensePredictions{ 500 };

    CLASS_FIELD(Config)
    TCdrIgnore<Lal::LColor> IntellisenseTint{ Lal::LColor::LightSlateGray };

    CLASS_FIELD(Config)
    TCdrIgnore<Lal::LColor> IntellisenseHighlightTint{ Lal::LColor::DarkSlateGray };

    void AddToHistory(LString const& Text);

    FORCEINLINE WEditableTextBox* GetEditableTextBox() noexcept { return this->EditableTextBox; }
    FORCEINLINE WEditableTextBox const* GetEditableTextBox() const noexcept { return this->EditableTextBox; }

    FORCEINLINE void ResetHistoryCursor() noexcept { this->HistoryCursor = INDEX_NONE; }

private:

    void OnCommit(LString const& Text, ETextCommit::Type CommitType);
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

    bool bIgnoreNonEnterCommits : 1 { false };
    bool bIgnoreEmptyEnterCommits : 1 { false };
    bool bPrefixCommand : 1 { false };
    bool bIgnoreOnChangedCall : 1 { false };

    //#
    //# The cursor of the history from back to front.
    //# Meaning 0 => The most recent history entry. The last entry in the array.
    //# Meaning n => The n-th history entry. The n-th entry in the array when counting from the back.
    //#
    i32 HistoryCursor{ INDEX_NONE };
    FORCEINLINE bool IsHistoryCursorValid() const
    {
        check( this->HistoryCursor == INDEX_NONE ? true : algo::is_valid_index(this->GetCDR()->History.get(), this->HistoryCursor) )
        return this->HistoryCursor != INDEX_NONE;
    }
    FORCEINLINE i32 GetIndexInHistory() const
    {
        if (this->HistoryCursor == INDEX_NONE)
        {
            return INDEX_NONE;
        }

        TArray<LString> const& DefaultHistory{ this->GetCDR()->History };
        check( algo::is_valid_index(DefaultHistory, DefaultHistory.size() - 1 - this->HistoryCursor) )
        return DefaultHistory.size() - 1 - this->HistoryCursor;
    }
    FORCEINLINE LString const* GetCurrentHistoryItem() const
    {
        if (this->HistoryCursor == INDEX_NONE)
        {
            return nullptr;
        }

        return &(this->GetCDR()->History[this->GetIndexInHistory()]);
    }
    FORCEINLINE LString const* GetCurrentHistoryItemChecked() const { check( this->IsHistoryCursorValid() ) return this->GetCurrentHistoryItem(); }
    FORCEINLINE LString const* GetCurrentHistoryItemCheckedAssert() const { jassert( this->IsHistoryCursorValid() ) return this->GetCurrentHistoryItem(); }

    //# The intellisense regions that are used if a command is being typed.
    CDR_NULL_PTR(WVRegion*) IntellisenseContainer{ nullptr };
    CDR_NULL_PTR(WVRegion*) Intellisense { nullptr };
    CDR_NULL_PTR(WRegion*)  IntellisenseHelpContainer { nullptr };
    CDR_NULL_PTR(WTextBox*) IntellisenseHelp { nullptr };
    CDR_NULL_PTR(WTextBox*) IntellisenseText { nullptr };
    CDR_NULL_PTR(WVRegion*) IntellisensePredictions { nullptr };
    LString CurrentIntellisensePrediction;

    CDR_NULL_PTR(WEditableTextBox*) EditableTextBox{ nullptr };
};

} /* ~Namespace Jafg */
