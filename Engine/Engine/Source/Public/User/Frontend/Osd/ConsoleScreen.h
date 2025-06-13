// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "Widgets/EditableTextBlockForward.h"
#include "Core/Application.h"
#include "Cli/CliHandles.h"
#include "Cli/CliCommand.h"
#include "ConsoleScreen.generated.h"

namespace Jafg
{

class WVRegion;
class WScrollRegion;
class WEditableTextBlock;

namespace EConsoleScreenState
{

enum Type : u8
{
    Show,
    Preview,
    TryPreview,
    Hide,
};

} /* ~Namespace EConsoleScreenState */

DECLARE_JAFG_WIDGET(EClassFlags::Config)
class WConsoleScreen final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WConsoleScreen)

public:

    virtual void BeginLifeDefault() override;
    virtual void Construct() override;
    virtual void Tick() override;
    virtual void OnGarbageDefault() override;

    void SetConsoleFrontendState(const EConsoleScreenState::Type InState);
    virtual void OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility) override;

    ENGINE_API void AddToHistory(const LString& InText);
    ENGINE_API void GoHistoryBack();
    ENGINE_API void GoHistoryForward();

    ENGINE_API void AddNewMessage(const LString& InText, const bool bSwitchToPreview = true);
    ENGINE_API void ClearMessages();

    //# Use with care... changing things here might destroy user experience.
    FORCEINLINE auto GetImpl() -> WEditableTextBlock* { return this->EditableTextBlock; }
    FORCEINLINE auto GetImpl() const -> const WEditableTextBlock* { return this->EditableTextBlock; }
    FORCEINLINE auto GetImplChecked() -> WEditableTextBlock* { check( this->EditableTextBlock ) return this->EditableTextBlock; }
    FORCEINLINE auto GetImplChecked() const -> const WEditableTextBlock* { check(this->EditableTextBlock ) return this->EditableTextBlock; }
    FORCEINLINE auto GetImplCheckedAssert() -> WEditableTextBlock* { jassert( this->EditableTextBlock ) return this->EditableTextBlock; }
    FORCEINLINE auto GetImplCheckedAssert() const -> const WEditableTextBlock* { jassert( this->EditableTextBlock ) return this->EditableTextBlock; }

    FORCEINLINE i32  GetMaxHistorySize() const { return GetDefault<WConsoleScreen>()->MaxHistorySize; }
    FORCEINLINE auto GetHistory() const -> const TArray<LString>& { return GetDefault<WConsoleScreen>()->History; }
    FORCEINLINE u32  GetConsoleWidth() const { return GetDefault<WConsoleScreen>()->ConsoleWidth; }
    FORCEINLINE u32  GetMaxPreviewLines() const { return GetDefault<WConsoleScreen>()->MaxPreviewLines; }
    FORCEINLINE f64  GetPreviewMessageLifetime() const { return GetDefault<WConsoleScreen>()->PreviewMessageLifetime; }

    //# The current cursor of the history. Has nothing to do with the actual history array.
    FORCEINLINE i32  GetHistoryCursor() const { return this->HistoryCursor; }
    FORCEINLINE bool IsHistoryCursorValid() const;

    //# The index in the actual history array.
    FORCEINLINE i32 GetIndexInHistory() const;
    FORCEINLINE const LString* GetCurrentHistoryItem() const;
    FORCEINLINE const LString* GetCurrentHistoryItemChecked() const { check( this->IsHistoryCursorValid() ) return this->GetCurrentHistoryItem(); }
    FORCEINLINE const LString* GetCurrentHistoryItemCheckedAssert() const { jassert( this->IsHistoryCursorValid() ) return this->GetCurrentHistoryItem(); }

private:

    void OnTextCommit(const LString& InText, const ETextCommit::Type InCommitType);
    void ShredOutdatedPreviewMessages();
    void ClearMessagesDefault(const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse);

    WEditableTextBlock* EditableTextBlock { nullptr };

    CLASS_FIELD(Config, DefaultOnly)
    i32 MaxHistorySize { 50 };

    CLASS_FIELD(Config, DefaultOnly)
    TArray<LString> History;

    CLASS_FIELD(Config, DefaultOnly)
    u32 ConsoleWidth { 500 };

    CLASS_FIELD(Config, DefaultOnly)
    u32 MaxPreviewLines { 10 };

    CLASS_FIELD(Config, DefaultOnly)
    f32 PreviewMessageLifetime { 5.0f };

    //#
    //# The cursor of the history from back to front.
    //# Meaning 0 => The most recent history entry. The last entry in the array.
    //# Meaning n => The n-th history entry. The n-th entry in the array when counting from the back.
    //#
    i32 HistoryCursor { INDEX_NONE };

    //#
    //# The region that is used to store the preview of the console if the console itself is not visible.
    //#
    WVRegion* ConsolePreview { nullptr };

    //#
    //# The region that is used to store the console history only if the console itself is visible.
    //#
    WVRegion* ConsoleHistory { nullptr };
    WScrollRegion* ConsoleHistoryContainer { nullptr };

    //#
    //# The intellisense region that is used if a command is being typed.
    //#
    WVRegion* Intellisense { nullptr };

    struct LPreviewMessage final
    {
        Application::LHrcTimePoint AddedTime;
        const WNode* Node;
    };
    TArray<LPreviewMessage> PreviewMessages;

    CLASS_FIELD(DefaultOnly)
    LCliCommandHandle CommandHandle_Clear;

#if !IN_SHIPPING
    void MockSomeMessages();
#endif /* !IN_SHIPPING */
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
