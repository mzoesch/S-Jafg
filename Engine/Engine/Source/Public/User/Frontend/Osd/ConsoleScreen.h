// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "Widgets/EditableTextBlockForward.h"
#include "ConsoleScreen.generated.h"

namespace Jafg
{

class WEditableTextBlock;

namespace EConsoleScreenState
{

enum Type : u8
{
    Show,
    Hide,
};

} /* ~Namespace EConsoleScreenState */

DECLARE_JAFG_WIDGET(EClassFlags::Config)
class ENGINE_API WConsoleScreen final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WConsoleScreen)

public:

    virtual void Construct() override;
    virtual void Tick() override;

    void SetConsoleFrontendState(const EConsoleScreenState::Type InState);
    virtual void OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility) override;

    void AddToHistory(const LString& InText);
    void GoHistoryBack();
    void GoHistoryForward();

    //# Use with care... changing things here might destroy user experience.
    FORCEINLINE auto GetImpl() -> WEditableTextBlock* { return this->EditableTextBlock; }
    FORCEINLINE auto GetImpl() const -> const WEditableTextBlock* { return this->EditableTextBlock; }
    FORCEINLINE auto GetImplChecked() -> WEditableTextBlock* { check( this->EditableTextBlock ) return this->EditableTextBlock; }
    FORCEINLINE auto GetImplChecked() const -> const WEditableTextBlock* { check(this->EditableTextBlock ) return this->EditableTextBlock; }
    FORCEINLINE auto GetImplCheckedAssert() -> WEditableTextBlock* { jassert( this->EditableTextBlock ) return this->EditableTextBlock; }
    FORCEINLINE auto GetImplCheckedAssert() const -> const WEditableTextBlock* { jassert( this->EditableTextBlock ) return this->EditableTextBlock; }

    FORCEINLINE i32  GetMaxHistorySize() const { return GetDefault<WConsoleScreen>()->MaxHistorySize; }
    FORCEINLINE auto GetHistory() const -> const TArray<LString>& { return GetDefault<WConsoleScreen>()->History; }

    //# The current cursor of the history. Has nothing to do with the actual history array.
    FORCEINLINE i32  GetHistoryCursor() const { return this->HistoryCursor; }
    FORCEINLINE bool IsHistoryCursorValid() const;

    //# The index in the actual history array.
    FORCEINLINE i32 GetIndexInHistory() const;
    FORCEINLINE const LString* GetCurrentHistoryItem() const;
    FORCEINLINE const LString* GetCurrentHistoryItemChecked() const { check( this->IsHistoryCursorValid() ) return this->GetCurrentHistoryItem(); }
    FORCEINLINE const LString* GetCurrentHistoryItemCheckedAssert() const { jassert( this->IsHistoryCursorValid() ) return this->GetCurrentHistoryItem(); }

private:

    void HideConsoleScreenWithSideEffects();
    void ShowConsoleScreenWithSideEffects();

    void OnTextCommit(const LString& InText, const ETextCommit::Type InCommitType);

    WEditableTextBlock* EditableTextBlock = nullptr;

    CLASS_FIELD(Config, DefaultOnly)
    i32 MaxHistorySize { 50 };

    CLASS_FIELD(Config, DefaultOnly)
    TArray<LString> History;

    //#
    //# The cursor of the history from back to front.
    //# Meaning 0 => The most recent history entry. The last entry in the array.
    //# Meaning n => The n-th history entry. The n-th entry in the array when counting from the back.
    //#
    i32 HistoryCursor { INDEX_NONE };
};

FORCEINLINE bool WConsoleScreen::IsHistoryCursorValid() const
{
    check( this->HistoryCursor == INDEX_NONE ? true : GetDefault<WConsoleScreen>()->History.IsValidIndex(this->HistoryCursor) )
    return this->HistoryCursor != INDEX_NONE;
}

i32 WConsoleScreen::GetIndexInHistory() const
{
    if (this->HistoryCursor == INDEX_NONE)
    {
        return INDEX_NONE;
    }

    const TArray<LString>& DefaultHistory = GetDefault<WConsoleScreen>()->History;
    check( DefaultHistory.IsValidIndex(DefaultHistory.GetSize() - 1 - this->HistoryCursor) )
    return DefaultHistory.GetSize() - 1 - this->HistoryCursor;
}

const LString* WConsoleScreen::GetCurrentHistoryItem() const
{
    if (this->HistoryCursor == INDEX_NONE)
    {
        return nullptr;
    }

    return &(GetDefault<WConsoleScreen>()->History[this->GetIndexInHistory()]);
}

} /* ~Namespace Jafg */
