// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "Widgets/EditableTextBoxForward.h"
#include "Core/Application.h"
#include "Cli/CommandLineInterface.h"
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
class WConsoleScreen;
class JConsoleScreenPreferences;
struct LFactoryConsoleScreen;

enum struct EConsoleScreenState : u8
{
    Show,
    Preview,
    TryPreview,
    Hide,
};

DECLARE_JAFG_CLASS(ECxxClassFlags::Config, ECxxClassFlags::Singleton)
class ENGINE_API JConsoleScreenPreferences : public JCxxClass
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JConsoleScreenPreferences)

public:

    virtual void BeginLife() override;

    CLASS_FIELD(Config)
    TPreference<u32> MaxHistorySize{ 50, 5, {} };

    CLASS_FIELD(Config)
    TArray<LString> History;

    CLASS_FIELD(Config)
    TPreference<u32> ConsoleWidth{ 250, 100, {} };

    CLASS_FIELD(Config)
    TPreference<u32> MaxPreviewLines{ 10, 2, {} };

    CLASS_FIELD(Config)
    TPreference<f32> PreviewMessageLifetime{ 5.0f, 0.5f, {} };

    CLASS_FIELD(Config)
    TPreference<u32> MaxIntellisensePredictions{ 10 };

    CLASS_FIELD(Config)
    TPreference<LColor> IntellisenseTint{ Colors::LightSlateGray };

    CLASS_FIELD(Config)
    TPreference<LColor> IntellisenseHighlightTint{ Colors::DarkSlateGray };

private:

    friend WConsoleScreen;
    LCliCommandRaiiHandle CommandHandle_Clear;
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryConsoleScreen)
class WConsoleScreen final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WIDGET_CONSTRUCTORS_BODY(WConsoleScreen)
    {
        this->SetShouldTick(true);
    }

public:

    virtual void Construct() override;
    virtual void Tick() override;
    virtual LReply OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;

    void OnEscape();
    ENGINE_API void SetConsoleFrontendState(EConsoleScreenState InState);
    virtual void OnVisibilityChanged(ENodeVisibility OldVisibility, ENodeVisibility NewVisibility) override;

    ENGINE_API void AddToHistory(LString const& InText);
    ENGINE_API void GoHistoryBack();
    ENGINE_API void GoHistoryForward();

    ENGINE_API void AddNewMessage(LString const& InText, bool bSwitchToPreview = true);
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

    //# The current cursor of the history. Has nothing to do with the actual history array.
    FORCEINLINE i32  GetHistoryCursor() const { return this->HistoryCursor; }
    FORCEINLINE bool IsHistoryCursorValid() const;

    //# The index in the actual history array.
    FORCEINLINE i32 GetIndexInHistory() const;
    FORCEINLINE LString const* GetCurrentHistoryItem() const;
    FORCEINLINE LString const* GetCurrentHistoryItemChecked() const { check(this->IsHistoryCursorValid()) return this->GetCurrentHistoryItem(); }
    FORCEINLINE LString const* GetCurrentHistoryItemCheckedAssert() const { jassert(this->IsHistoryCursorValid()) return this->GetCurrentHistoryItem(); }

    FORCEINLINE void StretchConsoleComponents(bool bStretch) noexcept { this->bStretchConsoleComponents = bStretch; }
    FORCEINLINE bool AreConsoleComponentsStretched() const noexcept { return this->bStretchConsoleComponents; }

private:

    bool OnAllowCommit();
    void OnTextCommit(const LString& InText, const ETextCommit InCommitType);
    void OnTextChanged(const LString& NewContent);
    void ShredOutdatedPreviewMessages();

    bool IsCurrentSelectedIntellisensePredictionValid() const;
    void PrepareIntellisense(const LString& NewContent);
    void UpdateIntellisense(const LCliCommand* InTargetCommand = nullptr);
    void ResetIntellisense() { algo::orphan(&this->CurrentIntellisensePrediction); }
    void UpdateIntellisensePredictionsColors();
    bool TryGoIntellisensePredictionUp();
    bool TryGoIntellisensePredictionDown();
    void ApplyCurrentIntellisensePrediction();
    auto GetCurrentHighlightedIntellisenseCommand() -> LCliCommand*;

    WEditableTextBox* EditableTextBlock{};

    //#
    //# The cursor of the history from back to front.
    //# Meaning 0 => The most recent history entry. The last entry in the array.
    //# Meaning n => The n-th history entry. The n-th entry in the array when counting from the back.
    //#
    i32 HistoryCursor{ INDEX_NONE };

    //#
    //# The region that is used to store the preview of the console if the console itself is not visible.
    //#
    WVRegion* ConsolePreview{};

    //#
    //# The region that is used to store the console history only if the console itself is visible.
    //#
    WVRegion* ConsoleHistory{};
    WScrollRegion* ConsoleHistoryContainer{};

    //#
    //# The intellisense regions that are used if a command is being typed.
    //#
    WVRegion* IntellisenseContainer{};
    WVRegion* Intellisense{};
    WRegion*  IntellisenseHelpContainer{};
    WTextBox* IntellisenseHelp{};
    WTextBox* IntellisenseText{};
    WVRegion* IntellisensePredictions{};
    LString   CurrentIntellisensePrediction;

    struct LPreviewMessage final
    {
        Application::LHrcTimePoint AddedTime;
        WNode const* Node;
    };
    TArray<LPreviewMessage> PreviewMessages;

    bool bStretchConsoleComponents{};

#if !IN_SHIPPING
    void MockSomeMessages();
#endif /* !IN_SHIPPING */
};

struct LFactoryConsoleScreen : NODE_FACTORY_PARENT(WConsoleScreen)
{
    NODE_FACTORY_BODY(WConsoleScreen)

    decltype(auto) StretchConsoleComponents(this auto&& Self, bool bInStretchConsoleComponents) noexcept
    {
        NODE_FACTORY_SELF().StretchConsoleComponents(bInStretchConsoleComponents);
        return NODE_FACTORY_RESULT();
    }
};

FORCEINLINE bool WConsoleScreen::IsHistoryCursorValid() const
{
    check(this->HistoryCursor == INDEX_NONE
        ? true
        : algo::is_valid_index(GetSingleton<JConsoleScreenPreferences>().History, this->HistoryCursor)
        )
    return this->HistoryCursor != INDEX_NONE;
}

i32 WConsoleScreen::GetIndexInHistory() const
{
    if (this->HistoryCursor == INDEX_NONE)
    {
        return INDEX_NONE;
    }

    check(this->IsHistoryCursorValid())
    TArray<LString> const& History{GetSingleton<JConsoleScreenPreferences>().History};
    check(algo::is_valid_index(History, History.size() - 1 - this->HistoryCursor))
    return History.size() - 1 - this->HistoryCursor;
}

LString const* WConsoleScreen::GetCurrentHistoryItem() const
{
    if (this->HistoryCursor == INDEX_NONE)
    {
        return nullptr;
    }
    return &(GetSingleton<JConsoleScreenPreferences>().History[this->GetIndexInHistory()]);
}

} /* ~Namespace Jafg */
