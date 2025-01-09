// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Frontend/Osd/ConsoleScreen.h"
#include "Engine/Cli/CliStatics.h"
#include "Engine/Engine.h"
#include "Engine/Cli/CommandLineInterface.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/Input/UserInput.h"
#include "Widgets/EditableTextBlock.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetRegion.h"

void Jafg::WConsoleScreen::Construct()
{
    Super::Construct();

    this->SetShouldTick(true);

    MakeRootNode(WWidgetRegion)
    .SetPadding(5.0f)
    .SetAnchor(EAnchor::VBottom | EAnchor::HFill)
    [
        NewNode(WEditableTextBlock)
        .SetTextColor(LColor::Red)
        .SetTextScale(0.5f)
        >> this->EditableTextBlock
        & LPadding({ 5.0f, 4.5f })
        & LColor(0 , 0, 0, 164)
        & LEditableTextBlockCommitDelegate::CreateFunction(this, &WConsoleScreen::OnTextCommit)
    ]
    FinishWidgetStyling()

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

    if (CliStatics::IsCommand(InText))
    {
        const LString Command = CliStatics::SafelyRemoveCommandPrefix(InText);
        if (Command.IsEmpty())
        {
            return;
        }

        LCommandExecutionResponse Response;
        this->GetEngine()->GetCheckedCommandLineInterface()->Invoke(Command, &Response);

        if (Response.StdOut.IsEmpty() == false)
        {
            if (Response.Rc >= ECommandReturnCode::Failure)
            {
                LOG_ERROR(LogCli, "Command [{}] failed with return code [{}]: {}.", Command, LexToString(Response.Rc), Response.StdOut)
            }
        }

        return;
    }

    LOG_WARNING(LogTemporal, "{}", InText)

    return;
}

void Jafg::WConsoleScreen::HideConsoleScreenWithSideEffects()
{
    check( this->GetVisibility() == EWidgetVisibility::Collapsed )
    LUserInput* UserInput = this->GetLocalEgo()->GetUserInput();
    UserInput->DeactivateAllContexts();
    UserInput->ActivateContext("InMyWorld");
    this->GetLocalEgo()->GetPrimarySurface()->SetInputMode(EInputMode::InputSubSystem, HideMouseCursor);

    if (this->EditableTextBlock)
    {
        this->EditableTextBlock->ClearText();
    }

    return;
}

void Jafg::WConsoleScreen::ShowConsoleScreenWithSideEffects()
{
    check( this->GetVisibility() == EWidgetVisibility::IntransitiveHitTestInvisible )
    LUserInput* UserInput = this->GetLocalEgo()->GetUserInput();
    UserInput->DeactivateAllContexts();
    UserInput->ActivateContext("InConsole");
    this->GetLocalEgo()->GetPrimarySurface()->SetInputMode(EInputMode::Both, ShowMouseCursor);

    return;
}
