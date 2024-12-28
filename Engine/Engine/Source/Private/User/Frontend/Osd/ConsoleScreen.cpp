// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Frontend/Osd/ConsoleScreen.h"

#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/Input/UserInput.h"
#include "Widgets/EditableTextBlock.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetRegion.h"

void Jafg::WConsoleScreen::Construct()
{
    Super::Construct();

    this->SetPadding({ 5.0f });

    this->SetShouldTick(true);

    MakeRootNode(WWidgetRegion)
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

    if (InText.IsEmpty())
    {
        this->SetConsoleFrontendState(EConsoleScreenState::Hide);
        return;
    }

    LOG_WARNING(LogTemporal, "{}", InText)

    this->SetConsoleFrontendState(EConsoleScreenState::Hide);
    check( InText.IsEmpty() )

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
