// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "Widgets/EditableTextBoxForward.h"
#include "Core/Application.h"
#include "Cli/CliHandles.h"
#include "Cli/CliCommand.h"
#include "ConsoleScreen.generated.h"

namespace Jafg
{

class WTextBox;
class WOverlay;
class WRegion;
class WHRegion;
class WVRegion;
class WScrollRegion;
class WEditableTextBox;

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

template<typename TNode>
struct TConsoleScreenFactory : public TWidgetFactoryParentBase<TNode>
{
    GENERATED_FACTORY_BODY(TWidgetFactoryParentBase)

    FORCEINLINE TFactoryRetTy& StretchConsoleComponents(const bool bInStretchConsoleComponents) noexcept
    {
        this->This()->StretchConsoleComponents(bInStretchConsoleComponents);
        return this->Self();
    }
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(TConsoleScreenFactory, ECxxClassFlags::Config)
class WConsoleScreen final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WConsoleScreen)

public:

    virtual void BeginLifeCDR() override;
    virtual void Construct() override;
    virtual void Tick() override;
    virtual void OnGarbageDefault(ECxxRecordTearDownReason::Type Reason) override;
    virtual LReply OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;

    void OnEscape();
    ENGINE_API void SetConsoleFrontendState(const EConsoleScreenState::Type InState);
    virtual void OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility) override;

    ENGINE_API void AddToHistory(const LString& InText);
    ENGINE_API void GoHistoryBack();
    ENGINE_API void GoHistoryForward();

    ENGINE_API void AddNewMessage(const LString& InText, const bool bSwitchToPreview = true);
    ENGINE_API void ClearMessages();

    ENGINE_API void AddIntellisense(const LString& InText);
    ENGINE_API void AddIntellisense(LString&& InText);
    ENGINE_API void ClearIntellisense();
    ENGINE_API void TryHideIntellisense();
    ENGINE_API void AddIntellisensePrediction(const LString& InText);
    ENGINE_API void AddIntellisensePrediction(LString&& InText);
    ENGINE_API void ClearIntellisensePredictions();

    //# Use with care... changing things here might destroy user experience.
    FORCEINLINE auto GetImpl() -> WEditableTextBox* { return this->EditableTextBlock; }
    FORCEINLINE auto GetImpl() const -> const WEditableTextBox* { return this->EditableTextBlock; }
    FORCEINLINE auto GetImplChecked() -> WEditableTextBox* { check( this->EditableTextBlock ) return this->EditableTextBlock; }
    FORCEINLINE auto GetImplChecked() const -> const WEditableTextBox* { check(this->EditableTextBlock ) return this->EditableTextBlock; }
    FORCEINLINE auto GetImplCheckedAssert() -> WEditableTextBox* { jassert( this->EditableTextBlock ) return this->EditableTextBlock; }
    FORCEINLINE auto GetImplCheckedAssert() const -> const WEditableTextBox* { jassert( this->EditableTextBlock ) return this->EditableTextBlock; }

    FORCEINLINE i32  GetMaxHistorySize() const { return GetDefault<WConsoleScreen>()->MaxHistorySize; }
    FORCEINLINE auto GetHistory() const -> const TArray<LString>& { return GetDefault<WConsoleScreen>()->History; }
    FORCEINLINE u32  GetConsoleWidth() const { return GetDefault<WConsoleScreen>()->ConsoleWidth; }
    FORCEINLINE u32  GetMaxPreviewLines() const { return GetDefault<WConsoleScreen>()->MaxPreviewLines; }
    FORCEINLINE f64  GetPreviewMessageLifetime() const { return GetDefault<WConsoleScreen>()->PreviewMessageLifetime; }
    FORCEINLINE u32  GetMaxIntellisensePredictions() const { return GetDefault<WConsoleScreen>()->MaxIntellisensePredictions; }
    FORCEINLINE const Lal::LColor& GetIntellisenseTint() const { return GetDefault<WConsoleScreen>()->IntellisenseTint; }
    FORCEINLINE const Lal::LColor& GetIntellisenseHighlightTint() const { return GetDefault<WConsoleScreen>()->IntellisenseHighlightTint; }

    //# The current cursor of the history. Has nothing to do with the actual history array.
    FORCEINLINE i32  GetHistoryCursor() const { return this->HistoryCursor; }
    FORCEINLINE bool IsHistoryCursorValid() const;

    //# The index in the actual history array.
    FORCEINLINE i32 GetIndexInHistory() const;
    FORCEINLINE const LString* GetCurrentHistoryItem() const;
    FORCEINLINE const LString* GetCurrentHistoryItemChecked() const { check( this->IsHistoryCursorValid() ) return this->GetCurrentHistoryItem(); }
    FORCEINLINE const LString* GetCurrentHistoryItemCheckedAssert() const { jassert( this->IsHistoryCursorValid() ) return this->GetCurrentHistoryItem(); }

    FORCEINLINE void StretchConsoleComponents(const bool bStretch) noexcept { this->bStretchConsoleComponents = bStretch; }
    FORCEINLINE bool AreConsoleComponentsStretched() const noexcept { return this->bStretchConsoleComponents; }

private:

    bool OnAllowCommit();
    void OnTextCommit(const LString& InText, const ETextCommit::Type InCommitType);
    void OnTextChanged(const LString& NewContent);
    void ShredOutdatedPreviewMessages();
    void ClearMessagesDefault(const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse);

    bool IsCurrentSelectedIntellisensePredictionValid() const;
    void PrepareIntellisense(const LString& NewContent);
    void UpdateIntellisense(const LCliCommand* InTargetCommand = nullptr);
    void ResetIntellisense();
    void UpdateIntellisensePredictionsColors();
    bool TryGoIntellisensePredictionUp();
    bool TryGoIntellisensePredictionDown();
    void ApplyCurrentIntellisensePrediction();
    auto GetCurrentHighlightedIntellisenseCommand() -> LCliCommand*;

    WEditableTextBox* EditableTextBlock { nullptr };

    CLASS_FIELD(Config)
    TCdrIgnore<u32> MaxHistorySize{ 50 };

    CLASS_FIELD(Config)
    TCdrIgnore<TArray<LString>> History;

    CLASS_FIELD(Config)
    TCdrIgnore<u32> ConsoleWidth{ 250 };

    CLASS_FIELD(Config)
    TCdrIgnore<u32> MaxPreviewLines{ 10 };

    CLASS_FIELD(Config)
    TCdrIgnore<f32> PreviewMessageLifetime{ 5.0f };

    CLASS_FIELD(Config)
    TCdrIgnore<u32> MaxIntellisensePredictions{ 10 };

    CLASS_FIELD(Config)
    TCdrIgnore<Lal::LColor> IntellisenseTint{ Lal::LColor::LightSlateGray };

    CLASS_FIELD(Config)
    TCdrIgnore<Lal::LColor> IntellisenseHighlightTint{ Lal::LColor::DarkSlateGray };

    //#
    //# The cursor of the history from back to front.
    //# Meaning 0 => The most recent history entry. The last entry in the array.
    //# Meaning n => The n-th history entry. The n-th entry in the array when counting from the back.
    //#
    i32 HistoryCursor{ INDEX_NONE };

    //#
    //# The region that is used to store the preview of the console if the console itself is not visible.
    //#
    CDR_NULL_PTR(WVRegion*) ConsolePreview{ nullptr };

    //#
    //# The region that is used to store the console history only if the console itself is visible.
    //#
    CDR_NULL_PTR(WVRegion*) ConsoleHistory{ nullptr };
    CDR_NULL_PTR(WScrollRegion*) ConsoleHistoryContainer{ nullptr };

    //#
    //# The intellisense regions that are used if a command is being typed.
    //#
    CDR_NULL_PTR(WVRegion*) IntellisenseContainer { nullptr };
    CDR_NULL_PTR(WVRegion*) Intellisense { nullptr };
    CDR_NULL_PTR(WRegion*)  IntellisenseHelpContainer { nullptr };
    CDR_NULL_PTR(WTextBox*) IntellisenseHelp { nullptr };
    CDR_NULL_PTR(WTextBox*) IntellisenseText { nullptr };
    CDR_NULL_PTR(WVRegion*) IntellisensePredictions { nullptr };
    LString   CurrentIntellisensePrediction;

    struct LPreviewMessage final
    {
        Application::LHrcTimePoint AddedTime;
        const WNode* Node;
    };
    TArray<LPreviewMessage> PreviewMessages;

    CLASS_FIELD()
    TCdrIgnore<LCliCommandHandle> CommandHandle_Clear;

    bool bStretchConsoleComponents{ false };

#if !IN_SHIPPING
    void MockSomeMessages();
#endif /* !IN_SHIPPING */
};

FORCEINLINE bool WConsoleScreen::IsHistoryCursorValid() const
{
    check( this->HistoryCursor == INDEX_NONE ? true : algo::is_valid_index(GetDefault<WConsoleScreen>()->History.get(), this->HistoryCursor) )
    return this->HistoryCursor != INDEX_NONE;
}

i32 WConsoleScreen::GetIndexInHistory() const
{
    if (this->HistoryCursor == INDEX_NONE)
    {
        return INDEX_NONE;
    }

    TArray<LString> const& DefaultHistory{ GetDefault<WConsoleScreen>()->History };
    check( algo::is_valid_index(DefaultHistory, DefaultHistory.size() - 1 - this->HistoryCursor) )
    return DefaultHistory.size() - 1 - this->HistoryCursor;
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
