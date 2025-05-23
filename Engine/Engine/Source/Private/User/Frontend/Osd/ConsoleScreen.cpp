// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/ConsoleScreen.h"
#include "Engine/Engine.h"
#include "Cli/CliStatics.h"
#include "Cli/CommandLineInterface.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/Input/UserInput.h"
#include "Widgets/EditableTextBlock.h"
#include "Widgets/Viewport.h"
#include "Widgets/Region.h"

void Jafg::WConsoleScreen::Construct()
{
    Super::Construct();

    this->SetShouldTick(true);

    MakeRootNode(WRegion)
        .Padding(5.0f)
        .Anchor(EAnchor::VBottom | EAnchor::HFill)
    [
        NewNode(WEditableTextBlock).SaveTo(&this->EditableTextBlock)
            .TextColor(LColor::Red)
            .TextScale(0.5f)
            .SetPadding({ 5.0f, 4.5f })
            .Tint({0, 0, 0, 164 })
            .OnCommit(LEditableTextBlockCommitDelegate::CreateFunction(this, &WConsoleScreen::OnTextCommit))
    ]
    FinishWidgetStyling()

    return;
}

void Jafg::WConsoleScreen::Tick()
{
    Super::Tick();

    this->GetViewportChecked()->GetCachedContextChecked()->ForEachNewKeyDown([this](const LRawInput& InKey)
    {
        if (InKey.Key == EKeys::Up)
        {
            this->GoHistoryBack();
        }
        else if (InKey.Key == EKeys::Down)
        {
            this->GoHistoryForward();
        }
    });

    return;
}

void Jafg::WConsoleScreen::SetConsoleFrontendState(const EConsoleScreenState::Type InState)
{
    if (InState == EConsoleScreenState::Show)
    {
        this->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
    }
    else if (InState == EConsoleScreenState::Hide)
    {
        this->SetVisibility(EWidgetVisibility::Collapsed);
    }
    else
    {
        checkNoEntry()
    }

    return;
}

void Jafg::WConsoleScreen::OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility)
{
    Super::OnVisibilityChanged(InOldVisibility, InNewVisibility);

    check
    (
        InNewVisibility == EWidgetVisibility::IntransitiveHitTestInvisible || InNewVisibility == EWidgetVisibility::Collapsed
        && "This widget only allows IntransitiveHitTestInvisible and Collapsed visibility."
    )

    if (EWidgetVisibility::IsDrawn(InNewVisibility))
    {
        checkSlow( this->EditableTextBlock )
        this->GetViewport()->FocusWidgetNode(this->EditableTextBlock);
    }

    if (InNewVisibility == EWidgetVisibility::IntransitiveHitTestInvisible)
    {
        this->ShowConsoleScreenWithSideEffects();
    }
    else if (InNewVisibility == EWidgetVisibility::Collapsed)
    {
        this->HideConsoleScreenWithSideEffects();
    }
    else
    {
        checkNoEntry()
    }

    return;
}

void Jafg::WConsoleScreen::AddToHistory(const LString& InText)
{
    TArray<LString>& MutableHistory = GetMutableDefault<WConsoleScreen>()->History;

    if (const LString* Last = MutableHistory.Peek(); Last)
    {
        if (*Last == InText)
        {
            return;
        }
    }

    if (MutableHistory.GetSize() > GetMutableDefault<WConsoleScreen>()->MaxHistorySize)
    {
        MutableHistory.RemoveAt(0);
    }

    MutableHistory.Add(InText);

    return;
}

void Jafg::WConsoleScreen::GoHistoryBack()
{
    const i32 LastHistoryCursor = this->HistoryCursor;

    const TArray<LString>& DefaultHistory = GetDefault<WConsoleScreen>()->History;

    this->HistoryCursor = Maths::Clamp(this->HistoryCursor + 1, static_cast<i32>(INDEX_NONE), DefaultHistory.GetSize() - 1);

    if (this->HistoryCursor == INDEX_NONE || this->HistoryCursor == LastHistoryCursor)
    {
        return;
    }

    this->GetImplChecked()->SetText(*this->GetCurrentHistoryItemChecked());

    return;
}

void Jafg::WConsoleScreen::GoHistoryForward()
{
    const i32 LastHistoryCursor = this->HistoryCursor;

    this->HistoryCursor = Maths::Clamp(this->HistoryCursor - 1, static_cast<i32>(INDEX_NONE), GetDefault<WConsoleScreen>()->History.GetSize() - 1);

    if (this->HistoryCursor == INDEX_NONE || this->HistoryCursor == LastHistoryCursor)
    {
        if (this->HistoryCursor != LastHistoryCursor)
        {
            this->GetImpl()->SetText("");
        }

        return;
    }

    this->GetImplChecked()->SetText(*this->GetCurrentHistoryItemChecked());

    return;
}

void Jafg::WConsoleScreen::HideConsoleScreenWithSideEffects()
{
    check( this->GetVisibility() == EWidgetVisibility::Collapsed )
    LUserInput* UserInput = this->GetLocalEgo()->GetUserInput();
    UserInput->DeactivateAllContexts();
    UserInput->ActivateContext("InMyWorld");
    this->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->SetInputMode(EInputMode::InputSubSystem, HideMouseCursor);

    if (this->EditableTextBlock)
    {
        this->EditableTextBlock->ClearText();
    }

    this->HistoryCursor = INDEX_NONE;

    return;
}

void Jafg::WConsoleScreen::ShowConsoleScreenWithSideEffects()
{
    check( this->GetVisibility() == EWidgetVisibility::IntransitiveHitTestInvisible )
    LUserInput* UserInput = this->GetLocalEgo()->GetUserInput();
    UserInput->DeactivateAllContexts();
    UserInput->ActivateContext("InConsole");
    this->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->SetInputMode(EInputMode::Both, ShowMouseCursor);

    this->HistoryCursor = INDEX_NONE;

    return;
}

void Jafg::WConsoleScreen::OnTextCommit(const LString& InText, const ETextCommit::Type InCommitType)
{
    if (InCommitType != ETextCommit::OnEnter)
    {
        return;
    }

    struct LOnTextCommitScope
    {
        LOnTextCommitScope(WConsoleScreen* InMe) : Me(InMe) { }
        ~LOnTextCommitScope()
        {
            Me->SetConsoleFrontendState(EConsoleScreenState::Hide);
        }
        WConsoleScreen* Me = nullptr;
    } OnTextCommitScope(this);

    if (InText.IsEmpty())
    {
        return;
    }

    this->AddToHistory(InText);

    if (CliStatics::IsCommand(InText))
    {
        const LString Command = CliStatics::SafelyRemoveCommandPrefix(InText);
        if (Command.IsEmpty())
        {
            return;
        }

        LCommandExecutionResponse Response;
        this->GetEngine()->GetCommandLineInterface()->Invoke(Command, &Response);

        if (Response.StdOut.IsEmpty() == false)
        {
            if (Response.Rc >= ECommandReturnCode::Failure)
            {
                LOG_ERROR(LogCli, "Command [{}] failed with return code [{}]: {}.", Command, LexToString(Response.Rc), Response.StdOut)
            }
            else
            {
                LOG_INFO(LogCli, "{}.", Response.StdOut)
            }
        }

        if (Response.StdErr.IsEmpty() == false)
        {
            LOG_ERROR(LogCli, "Command [{}] failed with return code [{}]: {}.", Command, LexToString(Response.Rc), Response.StdErr)
        }

        if (Response.SanitizedStdErr.IsEmpty() == false)
        {
            LOG_ERROR(LogCli, "Command [{}] failed with return code [{}]. User feedback: {}.", Command, LexToString(Response.Rc), Response.SanitizedStdErr)
        }

        // If the cmd did not give feedback on failure, we log a general error.
        if (Response.Rc >= ECommandReturnCode::Failure && Response.StdOut.IsEmpty() && Response.StdErr.IsEmpty())
        {
            LOG_ERROR(LogCli, "Command [{}] failed with return code [{}].", Command, LexToString(Response.Rc))
        }

        return;
    }

    LOG_WARNING(LogTemporal, "{}", InText)

    return;
}
