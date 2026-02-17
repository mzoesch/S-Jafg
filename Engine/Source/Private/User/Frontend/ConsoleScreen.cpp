// Copyright mzoesch. All rights reserved.

#include "User/Frontend/ConsoleScreen.h"
#include "Engine/Engine.h"
#include "Cli/CliStatics.h"
#include "Cli/CommandLineInterface.h"
#include "Core/CoreNames.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "User/Input/UserInput.h"
#include "Widgets/EditableTextBox.h"
#include "Widgets/Viewport.h"
#include "Widgets/Region.h"
#include "Widgets/ScrollRegion.h"
#include "Widgets/Spacer.h"
#include "Widgets/TextBox.h"
#include "Widgets/VRegion.h"

void Jafg::JConsoleScreenPreferences::BeginLife()
{
    Super::BeginLife();
    check(GEngine)

    this->CommandHandle_Clear = GEngine->GetCommandLineInterface().RegisterCommand({
        "Clear", "Clears the console messages.",
        LCommandParams{}
        .Exec([](LCommandExecutionInfo const& Info, LCommandArgs const& Args, LCommandExecutionResponse& OutResponse)->void
        {
            if (Info.Invoker && Info.Invoker->IsA<WConsoleScreen>())
            {
                Info.Invoker->AsStatic<WConsoleScreen>()->ClearMessages();
                OutResponse.Rc = ECommandReturnCode::SuccessNoResponse;
                return;
            }
            if (Info.Viewport)
            {
                if (auto* Widget{Info.Viewport->GetTopLevelWidgetByClass<WConsoleScreen>()})
                {
                    Widget->ClearMessages();
                    OutResponse.Rc = ECommandReturnCode::SuccessNoResponse;
                    return;
                }
            }
            OutResponse.Rc = ECommandReturnCode::Failure;
            OutResponse.StdErr = "Could not find console target to clear.";
            return;
        })});

    return;
}

void Jafg::WConsoleScreen::Construct()
{
    Super::Construct();

    auto& Prefs{GetSingleton<JConsoleScreenPreferences>()};

    BeginStyling(*this).Root<WRegion>()
        .Padding(1_spt)
        .Anchor(EAnchor::Fill)
    [
        NewStaticNode(WEditableTextBox).SaveTo(&this->EditableTextBlock)
            .Anchor(EAnchor::VBottom | EAnchor::HFill)
            .TextColor(Colors::White)
            .TextScale(ETextScale::Body)
            .Padding({2, 2})
            .Tint({0, 0, 0, 164})
            .OnAllowCommit(LEditableTextBoxAllowCommitDelegate::Create(this, &WConsoleScreen::OnAllowCommit))
            .OnCommit(LEditableTextBoxCommitDelegate::Create(this, &WConsoleScreen::OnTextCommit))
            .OnChanged(LEditableTextBoxChangedDelegate::Create(this, &WConsoleScreen::OnTextChanged))
        +
        NewStaticNode(WRegion)
            .Anchor(EAnchor::Fill)
            .Padding({0.0f, 0.0f, 0.0f, 50.0f})
        [
            NewStaticNode(WScrollRegion).SaveTo(&this->ConsoleHistoryContainer)
                .Anchor(EAnchor::VFill | (this->AreConsoleComponentsStretched() ? EAnchor::HFill : EAnchor::Identity))
                .MinDesiredSize(this->AreConsoleComponentsStretched() ?
                      LWidgetSize2{}
                    : LWidgetSize2{EWidgetSize::StaticPoints, static_cast<f32>(*Prefs.ConsoleWidth), 0.0f})
            [
                NewStaticNode(WVRegion).SaveTo(&this->ConsoleHistory)
                    .Anchor(EAnchor::VBottom | EAnchor::HFill)
                    .Tint({0, 0, 0, 164})
            ]
        ]
        +
        NewStaticNode(WOverlay)
            .Anchor(EAnchor::Fill)
            .Padding({0.0f, 0.0f, 0.0f, 50.0f})
        [
            NewStaticNode(WVRegion).SaveTo(&this->ConsolePreview)
                .Anchor(EAnchor::VBottom)
                .MinDesiredSize({EWidgetSize::Points, static_cast<f32>(*Prefs.ConsoleWidth), 0.0f})
                .Type(ERegionBrush::Box)
                .Tint({0, 0, 0, 164})
        ]
        +
        NewStaticNode(WVRegion).SaveTo(&this->IntellisenseContainer)
            .Anchor(EAnchor::VBottom)
            .Padding({0.0f, 0.0f, 0.0f, 30.0f})
            .VSpace(2_spt)
        [
            NewStaticNode(WVRegion).SaveTo(&this->IntellisensePredictions)
                .Padding(3.0f)
                .Type(ERegionBrush::OutlineBox)
                .Tint(*Prefs.IntellisenseTint)
                .OutlineTint(Colors::Black)
            +
            NewStaticNode(WRegion).SaveTo(&this->IntellisenseHelpContainer)
                .Padding(3.0f)
                .Type(ERegionBrush::OutlineBox)
                .Tint(*Prefs.IntellisenseTint)
                .OutlineTint(Colors::Black)
            [
                NewStaticNode(WTextBox).SaveTo(&this->IntellisenseHelp)
                    .Padding(3.0f)
                    .Brush(LTextBoxBrush::Body())
            ]
            +
            NewStaticNode(WVRegion).SaveTo(&this->Intellisense)
                .Padding(3.0f)
                .Type(ERegionBrush::OutlineBox)
                .Tint(*Prefs.IntellisenseTint)
                .OutlineTint(Colors::Black)
            [
                NewStaticNode(WTextBox).SaveTo(&this->IntellisenseText)
                    .Brush(LTextBoxBrush::Body())
            ]
        ]
    ]
    ;

    this->SetConsoleFrontendState(EConsoleScreenState::Show);
    return;
}

void Jafg::WConsoleScreen::Tick()
{
    Super::Tick();

    check(this->ConsolePreview)
    if (this->ConsolePreview->IsPainted())
    {
        this->ShredOutdatedPreviewMessages();
    }

    return;
}

Jafg::LReply Jafg::WConsoleScreen::OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    check(this->EditableTextBlock)

    if (this->EditableTextBlock->IsPainted() == false)
    {
        return LReply::HandledWithFocusLost();
    }

    if (this->IntellisenseContainer->IsPainted() && this->IntellisensePredictions->IsPainted())
    {
        if (InKeyEvent.GetKey() == EKeys::Up)
        {
            if (this->TryGoIntellisensePredictionUp())
            {
                return LReply::Handled();
            }
        }
        else if (InKeyEvent.GetKey() == EKeys::Down)
        {
            if (this->TryGoIntellisensePredictionDown())
            {
                return LReply::Handled();
            }
        }
        else if (InKeyEvent.GetKey() == EKeys::Tab)
        {
            if (this->IsCurrentSelectedIntellisensePredictionValid())
            {
                this->ApplyCurrentIntellisensePrediction();
            }
        }
    }
    else
    {
        if (InKeyEvent.GetKey() == EKeys::Up)
        {
            this->GoHistoryBack();
            return LReply::Handled();
        }
        if (InKeyEvent.GetKey() == EKeys::Down)
        {
            this->GoHistoryForward();
            return LReply::Handled();
        }
    }

    return Super::OnKeyDown(InViewport, InKeyEvent);
}

void Jafg::WConsoleScreen::OnEscape()
{
    if (this->IntellisenseContainer->IsPainted())
    {
        this->TryHideIntellisense();
    }
    else
    {
        this->SetConsoleFrontendState(EConsoleScreenState::TryPreview);
    }

    return;
}

void Jafg::WConsoleScreen::SetConsoleFrontendState(EConsoleScreenState InState)
{
    if (InState == EConsoleScreenState::Show)
    {
        this->SetVisibility(ENodeVisibility::IntransitiveHitTestInvisible);

        checkSlow(this->ConsolePreview)
        checkSlow(this->ConsoleHistory)
        checkSlow(this->EditableTextBlock)
        checkSlow(this->ConsoleHistoryContainer)

        this->ConsolePreview->SetVisibility(ENodeVisibility::Collapsed);
        this->EditableTextBlock->SetVisibility(ENodeVisibility::Visible);
        if (this->ConsoleHistory->GetChildren().empty())
        {
            this->ConsoleHistoryContainer->SetVisibility(ENodeVisibility::Collapsed);
        }
        else
        {
            this->ConsoleHistoryContainer->SetVisibility(ENodeVisibility::Visible);
        }
        this->IntellisenseContainer->SetVisibility(ENodeVisibility::Collapsed);

        this->EditableTextBlock->EmptyContent();
        this->GetViewport().FocusWidgetNode(this->EditableTextBlock);

        this->ConsoleHistoryContainer->ApplyVScroll(WScrollRegion::MaxScrollDown);

        this->HistoryCursor = INDEX_NONE;
        algo::orphan(&this->CurrentIntellisensePrediction);

        // LUserInput& UserInput{ this->GetLocalEgo().GetUserInput() };
        // UserInput.DeactivateContext(Name_UicInMyWorldFoot);
        // UserInput.DeactivateContext(Name_UicInMyWorld);
        // UserInput.ActivateContext(Name_UicInConsole);
        this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->SetInputMode(EInputMode::Both | EInputMode::ShowMouseCursor);
    }

    else if (InState == EConsoleScreenState::Preview)
    {
        this->SetVisibility(ENodeVisibility::IntransitiveHitTestInvisible);

        checkSlow( this->ConsolePreview )
        checkSlow( this->EditableTextBlock )
        checkSlow( this->ConsoleHistoryContainer )

        this->ConsolePreview->SetVisibility(ENodeVisibility::TransitiveHitTestInvisible);
        this->EditableTextBlock->SetVisibility(ENodeVisibility::Collapsed);
        this->ConsoleHistoryContainer->SetVisibility(ENodeVisibility::Collapsed);
        this->IntellisenseContainer->SetVisibility(ENodeVisibility::Collapsed);

        // LUserInput& UserInput{ this->GetLocalEgo().GetUserInput() };
        // UserInput.DeactivateContext(Name_UicInConsole);
        // UserInput.ActivateContext(Name_UicInMyWorldFoot);
        // UserInput.ActivateContext(Name_UicInMyWorld);
        this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->SetInputMode(EInputMode::InputSubSystem);
    }

    else if (InState == EConsoleScreenState::TryPreview)
    {
        this->ShredOutdatedPreviewMessages();
        if (this->ConsolePreview->GetChildren().empty())
        {
            this->SetConsoleFrontendState(EConsoleScreenState::Hide);
        }
        else
        {
            this->SetConsoleFrontendState(EConsoleScreenState::Preview);
        }
    }

    else if (InState == EConsoleScreenState::Hide)
    {
        this->SetVisibility(ENodeVisibility::Collapsed);

        // LUserInput& UserInput{ this->GetLocalEgo().GetUserInput() };
        // UserInput.DeactivateContext(Name_UicInConsole);
        // UserInput.ActivateContext(Name_UicInMyWorldFoot);
        // UserInput.ActivateContext(Name_UicInMyWorld);
        this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->SetInputMode(EInputMode::InputSubSystem);
    }

    else
    {
        checkNoEntry()
    }

    return;
}

void Jafg::WConsoleScreen::OnVisibilityChanged(ENodeVisibility OldVisibility, ENodeVisibility NewVisibility)
{
    Super::OnVisibilityChanged(OldVisibility, NewVisibility);

    // check
    // (
    //     InNewVisibility == EWidgetVisibility::IntransitiveHitTestInvisible || InNewVisibility == EWidgetVisibility::Collapsed
    //     && "This widget only allows IntransitiveHitTestInvisible and Collapsed visibility."
    // )

    return;
}

void Jafg::WConsoleScreen::AddToHistory(LString const& InText)
{
    TArray<LString>& MutableHistory{GetMutableSingleton<JConsoleScreenPreferences>().History};

    if (MutableHistory.empty() == false)
    {
        if (MutableHistory.back() == InText)
        {
            return;
        }
    }

    if (MutableHistory.size() > GetMutableSingleton<JConsoleScreenPreferences>().MaxHistorySize)
    {
        MutableHistory.erase(MutableHistory.begin());
    }

    MutableHistory.emplace_back(InText);

    return;
}

void Jafg::WConsoleScreen::GoHistoryBack()
{
    auto const& History{GetSingleton<JConsoleScreenPreferences>().History};

    const i32 LastHistoryCursor{this->HistoryCursor};

    this->HistoryCursor = maths::clamp(this->HistoryCursor + 1, static_cast<i32>(INDEX_NONE), static_cast<i32>(History.size()) - 1);
    if (this->HistoryCursor == INDEX_NONE || this->HistoryCursor == LastHistoryCursor)
    {
        return;
    }

    if (LString const& Item{*this->GetCurrentHistoryItemChecked()}; this->GetImplChecked()->GetContent() != Item)
    {
        this->GetImplChecked()->SetContent(Item);
        this->GetImplChecked()->SetCaretCursorToEnd();
    }

    return;
}

void Jafg::WConsoleScreen::GoHistoryForward()
{
    const i32 LastHistoryCursor = this->HistoryCursor;

    this->HistoryCursor = maths::clamp(this->HistoryCursor - 1, static_cast<i32>(INDEX_NONE), static_cast<i32>(GetSingleton<JConsoleScreenPreferences>().History.size()) - 1);

    if (this->HistoryCursor == INDEX_NONE || this->HistoryCursor == LastHistoryCursor)
    {
        if (this->HistoryCursor != LastHistoryCursor)
        {
            this->GetImpl()->SetContent("");
        }
        return;
    }

    this->GetImplChecked()->SetContent(*this->GetCurrentHistoryItemChecked());

    return;
}

void Jafg::WConsoleScreen::AddNewMessage(LString const& InText, const bool bSwitchToPreview /* = true */)
{
    LOG_VERBOSE(LogWidgets, "New console message: [{}].", InText)

    if (InText.empty())
    {
        LOG_ERROR(LogWidgets, "Cannot add empty console message.")
        return;
    }

    check(this->ConsolePreview)
    check(this->EditableTextBlock)
    check(this->ConsoleHistory)
    check(this->ConsoleHistoryContainer)

    /* Allow drawing. */
    if (this->ConsoleHistory->GetChildren().empty())
    {
        this->ConsoleHistory->SetType(ERegionBrush::Box);
    }

    BeginStyling(*this->ConsoleHistory).Root<WTextBox>()
        .Content(InText)
        .Brush(LTextBoxBrush::Body())
        ;

    if (bSwitchToPreview)
    {
        this->SetConsoleFrontendState(EConsoleScreenState::Preview);
        WNode* Message;
        BeginStyling(*this->ConsolePreview).Root<WTextBox>().SaveTo(&Message)
            .Content(InText)
            .Brush(LTextBoxBrush::Body())
            ;
        if (this->ConsolePreview->GetChildren().size() > GetSingleton<JConsoleScreenPreferences>().MaxPreviewLines)
        {
            this->ConsolePreview->RemoveChildAt(0);
        }
        this->PreviewMessages.emplace_back(Application::GetHighestNow(), Message);
    }

    return;
}

void Jafg::WConsoleScreen::ClearMessages()
{
    check(this->ConsolePreview)
    check(this->ConsoleHistory)

    this->ConsolePreview->RemoveChildren();
    this->ConsoleHistory->RemoveChildren();

    this->SetConsoleFrontendState(EConsoleScreenState::Hide);

    return;
}

void Jafg::WConsoleScreen::AddIntellisense(LString const& InText)
{
    BeginStyling(*this->Intellisense).At(0).Root<WTextBox>()
        .Content(InText)
        .Brush(LTextBoxBrush::Body())
        ;
}

void Jafg::WConsoleScreen::AddIntellisense(LString&& InText)
{
    BeginStyling(*this->Intellisense).At(0).Root<WTextBox>()
        .Content(std::move(InText))
        .Brush(LTextBoxBrush::Body())
        ;
}

void Jafg::WConsoleScreen::ClearIntellisense()
{
    this->Intellisense->RemoveChildren();

    return;
}

void Jafg::WConsoleScreen::TryHideIntellisense()
{
    this->IntellisenseContainer->SetVisibility(ENodeVisibility::Collapsed);
}

void Jafg::WConsoleScreen::AddIntellisensePrediction(const LString& InText)
{
    BeginStyling(*this->IntellisensePredictions).At(0).Root<WTextBox>()
        .Anchor(EAnchor::HFill)
        .Content(InText)
        .Brush(LTextBoxBrush::Body())
        .Tint(*GetSingleton<JConsoleScreenPreferences>().IntellisenseHighlightTint)
        ;
}

void Jafg::WConsoleScreen::AddIntellisensePrediction(LString&& InText)
{
    BeginStyling(*this->IntellisensePredictions).At(0).Root<WTextBox>()
        .Anchor(EAnchor::HFill)
        .Content(std::move(InText))
        .Brush(LTextBoxBrush::Body())
        .Tint(*GetSingleton<JConsoleScreenPreferences>().IntellisenseHighlightTint)
        ;
}

void Jafg::WConsoleScreen::ClearIntellisensePredictions()
{
    this->IntellisensePredictions->RemoveChildren();
}

bool Jafg::WConsoleScreen::OnAllowCommit()
{
    if (this->IntellisenseContainer->IsPainted() && this->IntellisensePredictions->IsPainted())
    {
        check(this->IsCurrentSelectedIntellisensePredictionValid())
        this->ApplyCurrentIntellisensePrediction();
        return false;
    }
    return true;
}

void Jafg::WConsoleScreen::OnTextCommit(LString const& InText, const ETextCommit InCommitType)
{
    if (InCommitType != ETextCommit::OnEnter)
    {
        return;
    }

    if (InText.empty())
    {
        this->SetConsoleFrontendState(EConsoleScreenState::TryPreview);
        return;
    }

    this->AddToHistory(InText);

    if (CliStatics::IsCommand(InText))
    {
        LString Command{CliStatics::SafelyRemoveCommandPrefix(InText)};
        if (Command.empty())
        {
            this->SetConsoleFrontendState(EConsoleScreenState::TryPreview);
            return;
        }

        LCommandExecutionResponse Response;
        this->GetEngine().GetCommandLineInterface().Invoke({.Invoker=this,.Viewport=&this->GetViewport()}, Command, &Response);

        if (Response.Rc > ECommandReturnCode::Failure)
        {
            check(Response.StdOut.empty())
            if (Response.StdErr.empty())
            {
                LOG_ERROR(LogCli, "Command [{}] failed with return code [{}] but no StdEerr was provided.", Command, LexToString(Response.Rc))
                return;
            }
            /* As this is a local error code, we can just log the StdErr. We do not have to sanitize it. */
            this->AddNewMessage(Response.StdErr);
            LOG_ERROR(LogCli, "[{}] failed RC[{}]: {}.", Command, LexToString(Response.Rc), Response.StdErr)
        }
        else
        {
            check(Response.StdErr.empty() && Response.SanitizedStdErr.empty())

            if (Response.Rc == ECommandReturnCode::Success)
            {
                check(Response.StdOut.empty() == false)
                this->AddNewMessage(Response.StdOut);
                LOG_VERBOSE(LogCli, " [{}] executed RC[{}]: {}.", Command, LexToString(Response.Rc), Response.StdOut)
            }
            else if (Response.Rc == ECommandReturnCode::SuccessNoResponse)
            {
                LOG_VERBOSE(LogCli, "Command [{}] executed successfully.", Command)
                this->SetConsoleFrontendState(EConsoleScreenState::TryPreview);
            }
            else
            {
                check(Response.StdOut.empty() == false)
                this->AddNewMessage(Response.StdOut);
                LOG_VERBOSE(LogCli, " [{}] executed RC[{}]: {}.", Command, LexToString(Response.Rc), Response.StdOut)
            }
        }
    }
    else
    {
        this->AddNewMessage(InText);
    }

    return;
}

void Jafg::WConsoleScreen::OnTextChanged(const LString& NewContent)
{
    if (CliStatics::IsCommand(NewContent))
    {
        this->PrepareIntellisense(NewContent);
        return;
    }

    this->IntellisenseContainer->SetVisibility(ENodeVisibility::Collapsed);

    return;
}

void Jafg::WConsoleScreen::ShredOutdatedPreviewMessages()
{
    for (auto It{ this->PreviewMessages.begin() }; It != this->PreviewMessages.end();)
    {
        if (Application::GetTimeDiffFromNow(It->AddedTime) > GetSingleton<JConsoleScreenPreferences>().PreviewMessageLifetime)
        {
            if (auto* const* Ref{algo::find_pointer(this->ConsolePreview->GetChildren(), It->Node, &LWidgetSlot::Content)})
            {
                this->ConsolePreview->RemoveChild(*Ref);
            }

            It = this->PreviewMessages.erase(It);
            continue;
        }

        It = algo::next(It);
        continue;
    }

    if (this->ConsolePreview->GetChildren().empty())
    {
        this->SetConsoleFrontendState(EConsoleScreenState::Hide);
    }

    return;
}

bool Jafg::WConsoleScreen::IsCurrentSelectedIntellisensePredictionValid() const
{
    for (const LWidgetSlot* Child : this->IntellisensePredictions->GetChildren())
    {
        if
        (
            const WTextBox* TextBlock { Child->Content->AsStatic<WTextBox>() };
            TextBlock->GetContent() == this->CurrentIntellisensePrediction
        )
        {
            return true;
        }
    }

    return false;
}

void Jafg::WConsoleScreen::PrepareIntellisense(const LString& NewContent)
{
    check( CliStatics::IsCommand(NewContent) )

    this->ClearIntellisense();
    this->ClearIntellisensePredictions();

    const LString CommandLine { CliStatics::SafelyRemoveCommandPrefix(NewContent) };
    LCommandLineInterface const& Cli{ this->GetEngine().GetCommandLineInterface() };

    if (CommandLine.empty())
    {
        i32 AddedCommands { 0 };
        for (const LCliCommand& Command : Cli.GetCommands())
        {
            if (AddedCommands >= static_cast<i32>(*GetSingleton<JConsoleScreenPreferences>().MaxIntellisensePredictions))
            {
                break;
            }
            ++AddedCommands;
            this->AddIntellisensePrediction(Command.GetIdentifier());
        }

        if (AddedCommands == 0)
        {
            LOG_ERROR(LogWidgets, "No commands found for intellisense.")
            this->IntellisenseContainer->SetVisibility(ENodeVisibility::Collapsed);
            this->ResetIntellisense();
            return;
        }

        this->IntellisensePredictions->SetVisibility(ENodeVisibility::IntransitiveHitTestInvisible);
        this->UpdateIntellisense();

        return;
    }

    const bool bFinishedTypingCommand { CommandLine.find(' ') != LString::npos };
    const LString CommandStr { CliStatics::GetCommandFromText(CommandLine) };

    if (bFinishedTypingCommand == false)
    {
        const LCliCommand* Command { nullptr };

        TArray<const LCliCommand*> Predictions;

        for (const LCliCommand& CliCommand : Cli.GetCommands())
        {
            if (Predictions.size() >= GetSingleton<JConsoleScreenPreferences>().MaxIntellisensePredictions)
            {
                break;
            }
            if (CliCommand.GetIdentifier().starts_with(CommandStr))
            {
                Predictions.emplace_back(&CliCommand);
            }
            continue;
        }

        if (Predictions.empty())
        {
            this->IntellisensePredictions->SetVisibility(ENodeVisibility::Collapsed);
        }
        else
        {
            if (Predictions.size() == 1 && Predictions[0]->GetIdentifier() == CommandStr)
            {
                this->IntellisensePredictions->SetVisibility(ENodeVisibility::Collapsed);
                Command = Cli.GetCommandAsserted(CommandStr);
            }
            else
            {
                this->IntellisensePredictions->SetVisibility(ENodeVisibility::IntransitiveHitTestInvisible);
                for (const LCliCommand* Prediction : Predictions)
                {
                    this->AddIntellisensePrediction(Prediction->GetIdentifier());
                    continue;
                }
            }
        }

        this->UpdateIntellisense(Command);
    }
    else if (const LCliCommand* Command { Cli.GetCommand(CommandStr) }; Command)
    {
        check(this->IntellisensePredictions->GetChildren().empty())
        if
        (
            TArray<LString> Suggestions{Cli.GetCommonSuggestions(NewContent, *GetSingleton<JConsoleScreenPreferences>().MaxIntellisensePredictions)};
            Suggestions.empty())
        {
            this->IntellisensePredictions->SetVisibility(ENodeVisibility::Collapsed);
        }
        else
        {
            this->IntellisensePredictions->SetVisibility(ENodeVisibility::IntransitiveHitTestInvisible);
            for (LString& Suggestion : Suggestions)
            {
                check(Suggestions.empty() == false)
                this->AddIntellisensePrediction(std::move(Suggestion));
                continue;
            }
        }
        this->UpdateIntellisense(Command);
    }
    else
    {
        check(this->IntellisensePredictions->GetChildren().empty())
        this->IntellisensePredictions->SetVisibility(ENodeVisibility::Collapsed);
        this->UpdateIntellisense();
    }

    return;
}

void Jafg::WConsoleScreen::UpdateIntellisense(const LCliCommand* InTargetCommand /* = nullptr */)
{
    if (this->IntellisensePredictions->IsPainted())
    {
        check(this->IntellisensePredictions->GetChildren().empty() == false)
        if (this->IsCurrentSelectedIntellisensePredictionValid() == false)
        {
            WTextBox const* ChildText{this->IntellisensePredictions->GetChildren().back()->Content->AsStatic<WTextBox>()};
            this->CurrentIntellisensePrediction = ChildText->GetContent();
            check(this->IsCurrentSelectedIntellisensePredictionValid())
        }
        this->UpdateIntellisensePredictionsColors();
    }
    else
    {
        this->ResetIntellisense();
    }

    LCliCommand const* Command{InTargetCommand};
    if (Command == nullptr)
    {
        Command = this->GetCurrentHighlightedIntellisenseCommand();
    }

    if (Command)
    {
        for (LCommandParams const& Overload : Command->GetOverloads())
        {
            if (LString Repr{Overload.GetCatRepresentation()}; Repr.empty() == false)
            {
                this->AddIntellisense(std::move(Repr));
            }
        }
        if (this->Intellisense->GetChildren().empty())
        {
            this->Intellisense->SetVisibility(ENodeVisibility::Collapsed);
        }
        else
        {
            this->Intellisense->SetVisibility(ENodeVisibility::IntransitiveHitTestInvisible);
        }
        if (Command->GetHelp().empty())
        {
            this->IntellisenseHelpContainer->SetVisibility(ENodeVisibility::Collapsed);
        }
        else
        {
            this->IntellisenseHelp->SetContent(Command->GetHelp());
            this->IntellisenseHelpContainer->SetVisibility(ENodeVisibility::TransitiveHitTestInvisible);
        }
    }
    else
    {
        this->IntellisenseHelpContainer->SetVisibility(ENodeVisibility::Collapsed);
        this->Intellisense->SetVisibility(ENodeVisibility::Collapsed);
    }

    if (this->Intellisense->IsPainted() || this->IntellisensePredictions->IsPainted())
    {
        this->IntellisenseContainer->SetVisibility(ENodeVisibility::IntransitiveHitTestInvisible);
    }
    else
    {
        check(this->IntellisenseHelpContainer->IsPainted() == false)
        this->IntellisenseContainer->SetVisibility(ENodeVisibility::Collapsed);
    }

    return;
}

void Jafg::WConsoleScreen::UpdateIntellisensePredictionsColors()
{
    for (LWidgetSlot const* Child : this->IntellisensePredictions->GetChildren())
    {
        if
        (
            WTextBox* TextBlock{Child->Content->AsStatic<WTextBox>()};
            TextBlock->GetContent() == this->CurrentIntellisensePrediction
        )
        {
            TextBlock->SetType(ERegionBrush::Box);
        }
        else
        {
            TextBlock->SetType(ERegionBrush::None);
        }

        continue;
    }

    return;
}

bool Jafg::WConsoleScreen::TryGoIntellisensePredictionUp()
{
    if
    (
        auto It{algo::find(this->IntellisensePredictions->GetChildren(), this->CurrentIntellisensePrediction, [](auto const& E){ return E->Content->template AsStatic<WTextBox>()->GetContent(); })};
        It != this->IntellisensePredictions->GetChildren().end()
    )
    {
        It = std::prev(It);
        if (It != this->IntellisensePredictions->GetChildren().begin())
        {
            this->CurrentIntellisensePrediction = (*It)->Content->AsStatic<WTextBox>()->GetContent();
            this->PrepareIntellisense(this->EditableTextBlock->GetContent());
            return true;
        }
    }

    return false;
}

bool Jafg::WConsoleScreen::TryGoIntellisensePredictionDown()
{
    if
    (
        auto It{algo::find(this->IntellisensePredictions->GetChildren(), this->CurrentIntellisensePrediction, [](auto const& E){ return E->Content->template AsStatic<WTextBox>()->GetContent(); })};
        It != this->IntellisensePredictions->GetChildren().end()
    )
    {
        It = algo::next(It);
        if (It != this->IntellisensePredictions->GetChildren().end())
        {
            this->CurrentIntellisensePrediction = (*It)->Content->AsStatic<WTextBox>()->GetContent();
            this->PrepareIntellisense(this->EditableTextBlock->GetContent());
            return true;
        }
    }

    return false;
}

void Jafg::WConsoleScreen::ApplyCurrentIntellisensePrediction()
{
    check(this->IsCurrentSelectedIntellisensePredictionValid())

    if (auto const Space{this->EditableTextBlock->GetContent().rfind(' ')}; Space == this->EditableTextBlock->GetContent().npos)
    {
        this->EditableTextBlock->SetContent(Jafg::SprintF("/{}", this->CurrentIntellisensePrediction));
    }
    else
    {
        LString Temp{ this->EditableTextBlock->GetContent().substr(0, Space + 1)};
        Temp.append(this->CurrentIntellisensePrediction);

        this->EditableTextBlock->SetContent(std::move(Temp));
    }
    this->EditableTextBlock->SetCaretCursorToEnd();

    this->PrepareIntellisense(this->EditableTextBlock->GetContent());

    return;
}

Jafg::LCliCommand* Jafg::WConsoleScreen::GetCurrentHighlightedIntellisenseCommand()
{
    if
    (
        LString const* CurPrediction{&this->CurrentIntellisensePrediction};
        algo::find(this->IntellisensePredictions->GetChildren(), *CurPrediction, [](auto const& E){ return E->Content->template AsStatic<WTextBox>()->GetContent(); }) == this->IntellisensePredictions->GetChildren().end()
    )
    {
        return nullptr;
    }

    check(GEngine)
    return GEngine->GetCommandLineInterface().GetCommandChecked(this->CurrentIntellisensePrediction);
}

#if !IN_SHIPPING
void Jafg::WConsoleScreen::MockSomeMessages()
{
    this->AddNewMessage("A");
    this->AddNewMessage("B");
    this->AddNewMessage("C");
    this->AddNewMessage("D");
    this->AddNewMessage("E");
    this->AddNewMessage("F");
    this->AddNewMessage("G");
    this->AddNewMessage("H");
    this->AddNewMessage("I");
    this->AddNewMessage("J");
    this->AddNewMessage("K");
    this->AddNewMessage("L");
    this->AddNewMessage("M");
    this->AddNewMessage("N");
    this->AddNewMessage("O");
    this->AddNewMessage("P");
    this->AddNewMessage("Q");
    this->AddNewMessage("R");
    this->AddNewMessage("S");
    this->AddNewMessage("T");
    this->AddNewMessage("U");
    this->AddNewMessage("V");
    this->AddNewMessage("W");
    this->AddNewMessage("X");
    this->AddNewMessage("Y");
    this->AddNewMessage("Z");
    this->AddNewMessage("a");
    this->AddNewMessage("b");
    this->AddNewMessage("c");
    this->AddNewMessage("d");
    this->AddNewMessage("e");
    this->AddNewMessage("f");
    this->AddNewMessage("g");
    this->AddNewMessage("h");
    this->AddNewMessage("i");
    this->AddNewMessage("j");
    this->AddNewMessage("k");
    this->AddNewMessage("l");
    this->AddNewMessage("m");
    this->AddNewMessage("n");
    this->AddNewMessage("o");
    this->AddNewMessage("p");
    this->AddNewMessage("q");
    this->AddNewMessage("r");
    this->AddNewMessage("s");
    this->AddNewMessage("t");
    this->AddNewMessage("u");
    this->AddNewMessage("v");
    this->AddNewMessage("w");
    this->AddNewMessage("x");
    this->AddNewMessage("y");
    this->AddNewMessage("z");

    this->AddNewMessage("123456789A");
    this->AddNewMessage("123456789B");
    this->AddNewMessage("123456789C");
    this->AddNewMessage("123456789D");
    this->AddNewMessage("123456789E");
    this->AddNewMessage("123456789F");
    this->AddNewMessage("123456789G");
    this->AddNewMessage("123456789H");
    this->AddNewMessage("123456789I");
    this->AddNewMessage("123456789J");
    this->AddNewMessage("123456789K");
    this->AddNewMessage("123456789L");
    this->AddNewMessage("123456789M");
    this->AddNewMessage("123456789N");
    this->AddNewMessage("123456789O");
    this->AddNewMessage("123456789P");
    this->AddNewMessage("123456789Q");
    this->AddNewMessage("123456789R");
    this->AddNewMessage("123456789S");
    this->AddNewMessage("123456789T");
    this->AddNewMessage("123456789U");
    this->AddNewMessage("123456789V");
    this->AddNewMessage("123456789W");
    this->AddNewMessage("123456789X");
    this->AddNewMessage("123456789Y");
    this->AddNewMessage("123456789Z");
    this->AddNewMessage("123456789a");
    this->AddNewMessage("123456789b");
    this->AddNewMessage("123456789c");
    this->AddNewMessage("123456789d");
    this->AddNewMessage("123456789e");
    this->AddNewMessage("123456789f");
    this->AddNewMessage("123456789g");
    this->AddNewMessage("123456789h");
    this->AddNewMessage("123456789i");
    this->AddNewMessage("123456789j");
    this->AddNewMessage("123456789k");
    this->AddNewMessage("123456789l");
    this->AddNewMessage("123456789m");
    this->AddNewMessage("123456789n");
    this->AddNewMessage("123456789o");
    this->AddNewMessage("123456789p");
    this->AddNewMessage("123456789q");
    this->AddNewMessage("123456789r");
    this->AddNewMessage("123456789s");
    this->AddNewMessage("123456789t");
    this->AddNewMessage("123456789u");
    this->AddNewMessage("123456789v");
    this->AddNewMessage("123456789w");
    this->AddNewMessage("123456789x");
    this->AddNewMessage("123456789y");
    this->AddNewMessage("123456789z");

    return;
}
#endif /* !IN_SHIPPING */
