// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/ConsoleScreen.h"
#include "Engine/Engine.h"
#include "Cli/CliStatics.h"
#include "Cli/CommandLineInterface.h"
#include "Core/CoreNames.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/Input/UserInput.h"
#include "Widgets/EditableTextBlock.h"
#include "Widgets/Viewport.h"
#include "Widgets/Region.h"
#include "Widgets/ScrollRegion.h"
#include "Widgets/Spacer.h"
#include "Widgets/TextBlock.h"
#include "Widgets/VRegion.h"

void Jafg::WConsoleScreen::BeginLifeDefault()
{
    Super::BeginLifeDefault();

    if (this->MaxHistorySize < 5)
    {
        LOG_ERROR(LogWidgets, "MaxHistorySize is set to [{}] which is less than the minimum of 5. Setting it to 5.", this->MaxHistorySize)
        this->MaxHistorySize = 5;
    }

    if (this->ConsoleWidth < 200)
    {
        LOG_ERROR(LogWidgets, "ConsoleWidth is set to [{}] which is less than the minimum of 200. Setting it to 200.", this->ConsoleWidth)
        this->ConsoleWidth = 200;
    }

    if (this->MaxPreviewLines < 2)
    {
        LOG_ERROR(LogWidgets, "MaxPreviewLines is set to [{}] which is less than the minimum of 2. Setting it to 2.", this->MaxPreviewLines)
        this->MaxPreviewLines = 2;
    }

    if (this->PreviewMessageLifetime < 0.5)
    {
        LOG_ERROR(LogWidgets, "PreviewMessageLifetime is set to [{}] which is less than the minimum of 0.5. Setting it to 0.5.", this->PreviewMessageLifetime)
        this->PreviewMessageLifetime = 0.5;
    }

    if (GEngine)
    {
        this->CommandHandle_Clear = GEngine->GetCommandLineInterface()->RegisterCommand(
        {
            "Clear", "Clears the console messages.",
            LCommandParams{}
            .Exec(LOnCommandInvokation::CreateMemberDelegate(this, &WConsoleScreen::ClearMessagesDefault))
        });
    }
    else
    {
        Tasks::Make(ENamedThreads::Master, ETaskTime::AfterEngineInit, [this](void) -> void
        {
            this->CommandHandle_Clear = GEngine->GetCommandLineInterface()->RegisterCommand(
            {
                "Clear", "Clears the console messages.",
                LCommandParams{}
                .Exec(LOnCommandInvokation::CreateMemberDelegate(this, &WConsoleScreen::ClearMessagesDefault))
            });
            return;
        });
    }

    return;
}

void Jafg::WConsoleScreen::Construct()
{
    Super::Construct();

    this->SetShouldTick(true);

    MakeRootNode(WRegion)
        .Padding(5.0f)
        .Anchor(EAnchor::Fill)
    [
        NewNode(WEditableTextBlock).SaveTo(&this->EditableTextBlock)
            .Anchor(EAnchor::VBottom | EAnchor::HFill)
            .TextTint(LColor::White)
            .TextScale(0.5f)
            .Padding({5.0f, 4.5f})
            .Tint({0, 0, 0, 164})
            .OnAllowCommit(LEditableTextBlockAllowCommitDelegate::CreateFunction(this, &WConsoleScreen::OnAllowCommit))
            .OnCommit(LEditableTextBlockCommitDelegate::CreateFunction(this, &WConsoleScreen::OnTextCommit))
            .OnChanged(LEditableTextBlockChangedDelegate::CreateFunction(this, &WConsoleScreen::OnTextChanged))
        +
        NewNode(WRegion)
            .Anchor(EAnchor::Fill)
            .Padding({0.0f, 0.0f, 0.0f, 50.0f})
        [
            NewNode(WScrollRegion).SaveTo(&this->ConsoleHistoryContainer)
                .Anchor(EAnchor::VFill)
                .MinDesiredSize({static_cast<f32>(this->GetConsoleWidth()), 0.0f})
            [
                NewNode(WVRegion).SaveTo(&this->ConsoleHistory)
                    .Anchor(EAnchor::VBottom | EAnchor::HFill)
                    .Tint({0, 0, 0, 164})
            ]
        ]
        +
        NewNode(WOverlay)
            .Anchor(EAnchor::Fill)
            .Padding({0.0f, 0.0f, 0.0f, 50.0f})
        [
            NewNode(WVRegion).SaveTo(&this->ConsolePreview)
                .Anchor(EAnchor::VBottom)
                .MinDesiredSize({static_cast<f32>(this->GetConsoleWidth()), 0.0f})
                .Type(ERegionBrush::Box)
                .Tint({0, 0, 0, 164})
        ]
        +
        NewNode(WVRegion).SaveTo(&this->IntellisenseContainer)
            .Anchor(EAnchor::VBottom)
            .Padding({0.0f, 0.0f, 0.0f, 30.0f})
            .VSpace(2.0f)
        [
            NewNode(WVRegion).SaveTo(&this->IntellisensePredictions)
                .Padding(3.0f)
                .Type(ERegionBrush::OutlineBox)
                .Tint(this->GetIntellisenseTint())
                .OutlineTint(LColor::Black)
            +
            NewNode(WRegion).SaveTo(&this->IntellisenseHelpContainer)
                .Padding(3.0f)
                .Type(ERegionBrush::OutlineBox)
                .Tint(this->GetIntellisenseTint())
                .OutlineTint(LColor::Black)
            [
                NewNode(WTextBlock).SaveTo(&this->IntellisenseHelp)
                    .Padding(3.0f)
                    .Brush(LTextBlockBrush::Body())
            ]
            +
            NewNode(WVRegion).SaveTo(&this->Intellisense)
                .Padding(3.0f)
                .Type(ERegionBrush::OutlineBox)
                .Tint(this->GetIntellisenseTint())
                .OutlineTint(LColor::Black)
            [
                NewNode(WTextBlock).SaveTo(&this->IntellisenseText)
                    .Brush(LTextBlockBrush::Body())
            ]
        ]
    ]
    FinishWidgetStyling()

    return;
}

void Jafg::WConsoleScreen::Tick()
{
    Super::Tick();

    check( this->ConsolePreview )
    if (this->ConsolePreview->IsPainted())
    {
        this->ShredOutdatedPreviewMessages();
    }

    return;
}

void Jafg::WConsoleScreen::OnGarbageDefault()
{
    Super::OnGarbageDefault();

    if (GEngine)
    {
        GEngine->GetCommandLineInterface()->UnregisterCommand(&this->CommandHandle_Clear);
    }

    return;
}

Jafg::LReply Jafg::WConsoleScreen::OnKeyDown(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    check( this->EditableTextBlock )
    check( this->EditableTextBlock->IsVisible() )

    if (this->IntellisenseContainer->IsPainted() && this->IntellisensePredictions->IsPainted())
    {
        if (InKeyEvent.GetKey() == EKeys::Up)
        {
            if (this->TryGoIntellisensePredictionUp())
            {
                this->UpdateIntellisensePredictionsColors();
                return LReply::Handled();
            }
        }
        else if (InKeyEvent.GetKey() == EKeys::Down)
        {
            if (this->TryGoIntellisensePredictionDown())
            {
                this->UpdateIntellisensePredictionsColors();
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

void Jafg::WConsoleScreen::SetConsoleFrontendState(const EConsoleScreenState::Type InState)
{
    if (InState == EConsoleScreenState::Show)
    {
        this->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);

        checkSlow( this->ConsolePreview )
        checkSlow( this->ConsoleHistory )
        checkSlow( this->EditableTextBlock )
        checkSlow( this->ConsoleHistoryContainer )

        this->ConsolePreview->SetVisibility(EWidgetVisibility::Collapsed);
        this->EditableTextBlock->SetVisibility(EWidgetVisibility::Visible);
        if (this->ConsoleHistory->GetChildren().IsEmpty())
        {
            this->ConsoleHistoryContainer->SetVisibility(EWidgetVisibility::Collapsed);
        }
        else
        {
            this->ConsoleHistoryContainer->SetVisibility(EWidgetVisibility::Visible);
        }
        this->IntellisenseContainer->SetVisibility(EWidgetVisibility::Collapsed);

        this->EditableTextBlock->ClearContent();
        this->GetViewport()->FocusWidgetNode(this->EditableTextBlock);

        this->ConsoleHistoryContainer->ApplyVScroll(WScrollRegion::MaxScrollDown);

        this->HistoryCursor = INDEX_NONE;
        this->CurrentIntellisensePrediction.Empty();

        LUserInput* UserInput { this->GetLocalEgo()->GetUserInput() };
        UserInput->DeactivateContext(Name_UicInMyWorldFoot);
        UserInput->DeactivateContext(Name_UicInMyWorld);
        UserInput->ActivateContext(Name_UicInConsole);
        this->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->SetInputMode(EInputMode::Both, ShowMouseCursor);
    }

    else if (InState == EConsoleScreenState::Preview)
    {
        this->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);

        checkSlow( this->ConsolePreview )
        checkSlow( this->EditableTextBlock )
        checkSlow( this->ConsoleHistoryContainer )

        this->ConsolePreview->SetVisibility(EWidgetVisibility::TransitiveHitTestInvisible);
        this->EditableTextBlock->SetVisibility(EWidgetVisibility::Collapsed);
        this->ConsoleHistoryContainer->SetVisibility(EWidgetVisibility::Collapsed);
        this->IntellisenseContainer->SetVisibility(EWidgetVisibility::Collapsed);

        LUserInput* UserInput { this->GetLocalEgo()->GetUserInput() };
        UserInput->DeactivateContext(Name_UicInConsole);
        UserInput->ActivateContext(Name_UicInMyWorldFoot);
        UserInput->ActivateContext(Name_UicInMyWorld);
        this->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->SetInputMode(EInputMode::InputSubSystem, HideMouseCursor);
    }

    else if (InState == EConsoleScreenState::TryPreview)
    {
        this->ShredOutdatedPreviewMessages();
        if (this->ConsolePreview->GetChildren().IsEmpty())
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
        this->SetVisibility(EWidgetVisibility::Collapsed);

        LUserInput* UserInput { this->GetLocalEgo()->GetUserInput() };
        UserInput->DeactivateContext(Name_UicInConsole);
        UserInput->ActivateContext(Name_UicInMyWorldFoot);
        UserInput->ActivateContext(Name_UicInMyWorld);
        this->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->SetInputMode(EInputMode::InputSubSystem, HideMouseCursor);
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

    this->GetImplChecked()->SetContent(*this->GetCurrentHistoryItemChecked());

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
            this->GetImpl()->SetContent("");
        }

        return;
    }

    this->GetImplChecked()->SetContent(*this->GetCurrentHistoryItemChecked());

    return;
}

void Jafg::WConsoleScreen::AddNewMessage(const LString& InText, const bool bSwitchToPreview /* = true */)
{
    LOG_VERBOSE(LogWidgets, "New console message: [{}].", InText)

    if (InText.IsEmpty())
    {
        LOG_ERROR(LogWidgets, "Cannot add empty console message.")
        return;
    }

    WTextBlock* Message;
    NewNode(WTextBlock).SaveTo(&Message)
        .Content(InText)
        .Brush(LTextBlockBrush::Body())
    FinishWidget(Message);

    checkSlow( this->ConsolePreview )
    checkSlow( this->EditableTextBlock )
    checkSlow( this->ConsoleHistory )
    checkSlow( this->ConsoleHistoryContainer )

    /* Allow drawing. */
    if (this->ConsoleHistory->GetChildren().IsEmpty())
    {
        this->ConsoleHistory->SetType(ERegionBrush::Box);
    }

    this->ConsoleHistory->AddChild(Message);

    if (bSwitchToPreview)
    {
        this->SetConsoleFrontendState(EConsoleScreenState::Preview);

        WTextBlock* PreviewMessage;
        NewNode(WTextBlock).SaveTo(&PreviewMessage)
            .Content(InText)
            .Brush(LTextBlockBrush::Body())
        FinishWidget(PreviewMessage);
        this->ConsolePreview->AddChild(PreviewMessage);
        if (this->ConsolePreview->GetChildren().GetSize() > GetMaxPreviewLines())
        {
            this->ConsolePreview->RemoveChildAt(0);
        }
        this->PreviewMessages.Emplace(Application::GetHighestNow(), static_cast<const WNode*>(PreviewMessage));
    }

    return;
}

void Jafg::WConsoleScreen::ClearMessages()
{
    check( this->ConsolePreview )
    check( this->ConsoleHistory )

    this->ConsolePreview->RemoveChildren();
    this->ConsoleHistory->RemoveChildren();

    this->SetConsoleFrontendState(EConsoleScreenState::Hide);

    return;
}

void Jafg::WConsoleScreen::AddIntellisense(const LString& InText)
{
    WTextBlock* PreviewMessage;
    NewNode(WTextBlock).SaveTo(&PreviewMessage)
        .Content(InText)
        .Brush(LTextBlockBrush::Body())
    FinishWidget(PreviewMessage);

    this->Intellisense->AddChildAt(0, PreviewMessage);

    return;
}

void Jafg::WConsoleScreen::AddIntellisense(LString&& InText)
{
    WTextBlock* PreviewMessage;
    NewNode(WTextBlock).SaveTo(&PreviewMessage)
        .Content(std::move(InText))
        .Brush(LTextBlockBrush::Body())
    FinishWidget(PreviewMessage);

    this->Intellisense->AddChildAt(0, PreviewMessage);

    return;
}

void Jafg::WConsoleScreen::ClearIntellisense()
{
    this->Intellisense->RemoveChildren();

    return;
}

void Jafg::WConsoleScreen::TryHideIntellisense()
{
    this->IntellisenseContainer->SetVisibility(EWidgetVisibility::Collapsed);

    return;
}

void Jafg::WConsoleScreen::AddIntellisensePrediction(const LString& InText)
{
    WTextBlock* PreviewMessage;
    NewNode(WTextBlock).SaveTo(&PreviewMessage)
        .Anchor(EAnchor::HFill)
        .Content(InText)
        .Brush(LTextBlockBrush::Body())
    FinishWidget(PreviewMessage);

    this->IntellisensePredictions->AddChildAt(0, PreviewMessage);

    return;
}

void Jafg::WConsoleScreen::AddIntellisensePrediction(LString&& InText)
{
    WTextBlock* PreviewMessage;
    NewNode(WTextBlock).SaveTo(&PreviewMessage)
        .Anchor(EAnchor::HFill)
        .Content(std::move(InText))
        .Brush(LTextBlockBrush::Body())
    FinishWidget(PreviewMessage);

    this->IntellisensePredictions->AddChildAt(0, PreviewMessage);

    return;
}

void Jafg::WConsoleScreen::ClearIntellisensePredictions()
{
    this->IntellisensePredictions->RemoveChildren();

    return;
}

bool Jafg::WConsoleScreen::OnAllowCommit()
{
    if (this->IntellisenseContainer->IsPainted() && this->IntellisensePredictions->IsPainted())
    {
        check( this->IsCurrentSelectedIntellisensePredictionValid() )
        this->ApplyCurrentIntellisensePrediction();

        return false;
    }

    return true;
}

void Jafg::WConsoleScreen::OnTextCommit(const LString& InText, const ETextCommit::Type InCommitType)
{
    if (InCommitType != ETextCommit::OnEnter)
    {
        return;
    }

    if (InText.IsEmpty())
    {
        this->SetConsoleFrontendState(EConsoleScreenState::TryPreview);
        return;
    }

    this->AddToHistory(InText);

    if (CliStatics::IsCommand(InText))
    {
        const LString Command = CliStatics::SafelyRemoveCommandPrefix(InText);
        if (Command.IsEmpty())
        {
            this->SetConsoleFrontendState(EConsoleScreenState::TryPreview);
            return;
        }

        LCommandExecutionResponse Response;
        this->GetEngine()->GetCommandLineInterface()->Invoke(Command, &Response);

        if (Response.Rc > ECommandReturnCode::Failure)
        {
            check( Response.StdOut.IsEmpty() )

            if (Response.StdErr.IsEmpty())
            {
                LOG_ERROR(LogCli, "Command [{}] failed with return code [{}] but no StdEerr was provided.", Command, LexToString(Response.Rc))
                return;
            }

            /*
             * As this is a local error code, we can just log the StdErr. We do not have to sanitize it.
             */
            this->AddNewMessage(Response.StdErr);
            LOG_ERROR(LogCli, "[{}] failed RC[{}]: {}.", Command, LexToString(Response.Rc), Response.StdErr)
        }

        else
        {
            check( Response.StdErr.IsEmpty() && Response.SanitizedStdErr.IsEmpty() )

            if (Response.Rc == ECommandReturnCode::Success)
            {
                check( Response.StdOut.IsEmpty() == false )
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
                check( Response.StdOut.IsEmpty() == false )
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

    this->IntellisenseContainer->SetVisibility(EWidgetVisibility::Collapsed);

    return;
}

void Jafg::WConsoleScreen::ShredOutdatedPreviewMessages()
{
    for (i32 Idx { 0 }; Idx < this->PreviewMessages.GetSize();)
    {
        if
        (
            LPreviewMessage* Msg { &this->PreviewMessages[Idx] };
            Application::GetTimeDiffFromNow(Msg->AddedTime) > this->GetPreviewMessageLifetime()
        )
        {
            if
            (
                LWidgetSlot* const* Ref { this->ConsolePreview->GetChildren().FindRefByPredicate([Msg](const LWidgetSlot* Slot) -> bool
                {
                    return Slot->Content == Msg->Node;
                })};
                Ref
            )
            {
                this->ConsolePreview->RemoveChild(*Ref);
            }

            this->PreviewMessages.RemoveAt(Idx);
            continue;
        }

        ++Idx;
        continue;
    }

    if (this->ConsolePreview->GetChildren().IsEmpty())
    {
        this->SetConsoleFrontendState(EConsoleScreenState::Hide);
    }

    return;
}

void Jafg::WConsoleScreen::ClearMessagesDefault(const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse)
{
    check( this->IsDefault() )

    if (GEngine == nullptr)
    {
        OutResponse->Rc = ECommandReturnCode::Failure;
        OutResponse->StdErr = "Cannot clear console messages, GEngine is invalid.";
        return;
    }

    if
    (
        WConsoleScreen* Instance
        {
            GEngine->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->GetViewport().GetTopLevelWidgetByClass<WConsoleScreen>()
        };
        Instance
    )
    {
        Instance->ClearMessages();
        OutResponse->Rc = ECommandReturnCode::SuccessNoResponse;
    }
    else
    {
        OutResponse->Rc = ECommandReturnCode::Failure;
        OutResponse->StdErr = "Cannot clear console messages, no console screen found.";
    }

    return;
}

bool Jafg::WConsoleScreen::IsCurrentSelectedIntellisensePredictionValid() const
{
    for (const LWidgetSlot* Child : this->IntellisensePredictions->GetChildren())
    {
        if
        (
            const WTextBlock* TextBlock { Child->Content->AsStatic<WTextBlock>() };
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
    const LCommandLineInterface* Cli { this->GetEngine()->GetCommandLineInterface() };

    if (CommandLine.IsEmpty())
    {
        i32 AddedCommands { 0 };
        for (const LCliCommand& Command : Cli->GetCommands())
        {
            if (AddedCommands >= this->GetMaxIntellisensePredictions())
            {
                break;
            }

            ++AddedCommands;
            this->AddIntellisensePrediction(Command.GetIdentifier());
        }

        if (AddedCommands == 0)
        {
            LOG_ERROR(LogWidgets, "No commands found for intellisense.")
            this->IntellisenseContainer->SetVisibility(EWidgetVisibility::Collapsed);
            this->ResetIntellisense();
            return;
        }

        this->IntellisensePredictions->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
        this->UpdateIntellisense();

        return;
    }

    const bool bFinishedTypingCommand { CommandLine.Contains(' ') };
    const LString CommandStr { CliStatics::GetCommandFromText(CommandLine) };

    if (bFinishedTypingCommand == false)
    {
        const LCliCommand* Command { nullptr };

        TArray<const LCliCommand*> Predictions;

        for (const LCliCommand& CliCommand : Cli->GetCommands())
        {
            if (Predictions.GetSize() >= this->GetMaxIntellisensePredictions())
            {
                break;
            }

            if (CliCommand.GetIdentifier().StartsWith(CommandStr))
            {
                Predictions.Emplace(&CliCommand);
            }

            continue;
        }

        if (Predictions.IsEmpty())
        {
            this->IntellisensePredictions->SetVisibility(EWidgetVisibility::Collapsed);
        }
        else
        {
            if (Predictions.GetSize() == 1 && Predictions[0]->GetIdentifier() == CommandStr)
            {
                this->IntellisensePredictions->SetVisibility(EWidgetVisibility::Collapsed);
                Command = Cli->GetCommandAsserted(CommandStr);
            }
            else
            {
                this->IntellisensePredictions->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
                for (const LCliCommand* Prediction : Predictions)
                {
                    this->AddIntellisensePrediction(Prediction->GetIdentifier());
                    continue;
                }
            }
        }

        this->UpdateIntellisense(Command);
    }
    else if (const LCliCommand* Command { Cli->GetCommand(CommandStr) }; Command)
    {
        check( this->IntellisensePredictions->GetChildren().IsEmpty() )

        if
        (
            TArray<LString> Suggestions { Cli->GetCommonSuggestions(NewContent, this->GetMaxIntellisensePredictions()) };
            Suggestions.IsEmpty())
        {
            this->IntellisensePredictions->SetVisibility(EWidgetVisibility::Collapsed);
        }
        else
        {
            this->IntellisensePredictions->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
            for (LString& Suggestion : Suggestions)
            {
                check( Suggestions.IsEmpty() == false )
                this->AddIntellisensePrediction(std::move(Suggestion));
                continue;
            }
        }

        this->UpdateIntellisense(Command);
    }
    else
    {
        check( this->IntellisensePredictions->GetChildren().IsEmpty() )
        this->IntellisensePredictions->SetVisibility(EWidgetVisibility::Collapsed);
        this->UpdateIntellisense();
    }

    return;
}

void Jafg::WConsoleScreen::UpdateIntellisense(const LCliCommand* InTargetCommand /* = nullptr */)
{
    if (this->IntellisensePredictions->IsPainted())
    {
        check( this->IntellisensePredictions->GetChildren().IsEmpty() == false)

        if (this->IsCurrentSelectedIntellisensePredictionValid() == false)
        {
            const WTextBlock* ChildText { (*this->IntellisensePredictions->GetChildren().GetLast())->Content->AsStatic<WTextBlock>() };
            this->CurrentIntellisensePrediction = ChildText->GetContent();
            check( this->IsCurrentSelectedIntellisensePredictionValid() )
        }

        this->UpdateIntellisensePredictionsColors();
    }
    else
    {
        this->ResetIntellisense();
    }

    const LCliCommand* Command { InTargetCommand };
    if (Command == nullptr)
    {
        Command = this->GetCurrentHighlightedIntellisenseCommand();
    }

    if (Command)
    {
        for (const LCommandParams& Overload : Command->GetOverloads())
        {
            if (LString Repr { Overload.GetCatRepresentation() }; Repr.IsEmpty() == false)
            {
                this->AddIntellisense(std::move(Repr));
            }
        }
        if (this->Intellisense->GetChildren().IsEmpty())
        {
            this->Intellisense->SetVisibility(EWidgetVisibility::Collapsed);
        }
        else
        {
            this->Intellisense->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
        }
        if (Command->GetHelp().IsEmpty())
        {
            this->IntellisenseHelpContainer->SetVisibility(EWidgetVisibility::Collapsed);
        }
        else
        {
            this->IntellisenseHelp->SetContent(Command->GetHelp());
            this->IntellisenseHelpContainer->SetVisibility(EWidgetVisibility::TransitiveHitTestInvisible);
        }
    }
    else
    {
        this->IntellisenseHelpContainer->SetVisibility(EWidgetVisibility::Collapsed);
        this->Intellisense->SetVisibility(EWidgetVisibility::Collapsed);
    }

    if (this->Intellisense->IsPainted() || this->IntellisensePredictions->IsPainted())
    {
        this->IntellisenseContainer->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
    }
    else
    {
        check( this->IntellisenseHelpContainer->IsPainted() == false )
        this->IntellisenseContainer->SetVisibility(EWidgetVisibility::Collapsed);
    }

    return;
}

void Jafg::WConsoleScreen::ResetIntellisense()
{
    this->CurrentIntellisensePrediction.Empty();
    return;
}

void Jafg::WConsoleScreen::UpdateIntellisensePredictionsColors()
{
    for (const LWidgetSlot* Child : this->IntellisensePredictions->GetChildren())
    {
        if
        (
            WTextBlock* TextBlock { Child->Content->AsStatic<WTextBlock>() };
            TextBlock->GetContent() == this->CurrentIntellisensePrediction
        )
        {
            TextBlock->SetTint(this->GetIntellisenseHighlightTint());
        }
        else
        {
            TextBlock->SetTint(this->GetIntellisenseTint());
        }

        continue;
    }

    return;
}

bool Jafg::WConsoleScreen::TryGoIntellisensePredictionUp()
{
    LString* CurPrediction { &this->CurrentIntellisensePrediction };
    const i32 CurIdx { this->IntellisensePredictions->GetChildren().FindByPredicate([CurPrediction](const LWidgetSlot* InWidgetSlot) -> bool
    {
        return InWidgetSlot->Content->AsStatic<WTextBlock>()->GetContent() == *CurPrediction;
    })};

    if (CurIdx != INDEX_NONE)
    {
        if (CurIdx > 0)
        {
            this->CurrentIntellisensePrediction =
                this->IntellisensePredictions->GetChildren()[CurIdx - 1]
                    ->Content->AsStatic<WTextBlock>()->GetContent();

            this->PrepareIntellisense(this->EditableTextBlock->GetContent());

            return true;
        }
    }

    return false;
}

bool Jafg::WConsoleScreen::TryGoIntellisensePredictionDown()
{
    LString* CurPrediction { &this->CurrentIntellisensePrediction };
    const i32 CurIdx { this->IntellisensePredictions->GetChildren().FindByPredicate([CurPrediction](const LWidgetSlot* InWidgetSlot) -> bool
    {
        return InWidgetSlot->Content->AsStatic<WTextBlock>()->GetContent() == *CurPrediction;
    })};

    if (CurIdx != INDEX_NONE)
    {
        if (CurIdx < this->IntellisensePredictions->GetChildren().GetSize() - 1)
        {
            this->CurrentIntellisensePrediction =
                this->IntellisensePredictions->GetChildren()[CurIdx + 1]
                    ->Content->AsStatic<WTextBlock>()->GetContent();

            this->PrepareIntellisense(this->EditableTextBlock->GetContent());

            return true;
        }
    }

    return false;
}

void Jafg::WConsoleScreen::ApplyCurrentIntellisensePrediction()
{
    check( this->IsCurrentSelectedIntellisensePredictionValid() )

    if (const i32 Space { this->EditableTextBlock->GetContent().FindLast(' ') }; Space == INDEX_NONE)
    {
        this->EditableTextBlock->SetContent(LString::SprintF("/{}", this->CurrentIntellisensePrediction));
    }
    else
    {
        LString Temp { this->EditableTextBlock->GetContent() };
        Temp.InlineLeftChop(Space + 1);
        Temp.Append(this->CurrentIntellisensePrediction);

        this->EditableTextBlock->SetContent(std::move(Temp));
    }

    this->PrepareIntellisense(this->EditableTextBlock->GetContent());

    return;
}

Jafg::LCliCommand* Jafg::WConsoleScreen::GetCurrentHighlightedIntellisenseCommand()
{
    LString* CurPrediction { &this->CurrentIntellisensePrediction };
    const i32 CurIdx { this->IntellisensePredictions->GetChildren().FindByPredicate([CurPrediction](const LWidgetSlot* InWidgetSlot) -> bool
    {
        return InWidgetSlot->Content->AsStatic<WTextBlock>()->GetContent() == *CurPrediction;
    })};

    if (CurIdx == INDEX_NONE)
    {
        return nullptr;
    }

    check( GEngine )
    return GEngine->GetCommandLineInterface()->GetCommandChecked(this->CurrentIntellisensePrediction);
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
