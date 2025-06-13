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
            .TextTint(LColor::Red)
            .TextScale(0.5f)
            .Padding({5.0f, 4.5f})
            .Tint({0, 0, 0, 164})
            .OnCommit(LEditableTextBlockCommitDelegate::CreateFunction(this, &WConsoleScreen::OnTextCommit))
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
        NewNode(WOverlay)
        .Anchor(EAnchor::Fill)
        .Padding({0.0f, 0.0f, 0.0f, 50.0f})
        [
            NewNode(WVRegion).SaveTo(&this->Intellisense)
                .Anchor(EAnchor::VBottom)
                .Type(ERegionBrush::Box)
                .Tint({0, 0, 0, 192})
        ]
    ]
    FinishWidgetStyling()

    return;
}

void Jafg::WConsoleScreen::Tick()
{
    Super::Tick();

    check( this->EditableTextBlock )
    if (this->EditableTextBlock->IsWidgetVisible())
    {
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
    }

    check( this->ConsolePreview )
    if (this->ConsolePreview->IsWidgetVisible())
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

void Jafg::WConsoleScreen::SetConsoleFrontendState(const EConsoleScreenState::Type InState)
{
    if (InState == EConsoleScreenState::Show)
    {
        this->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);

        checkSlow( this->ConsolePreview )
        checkSlow( this->EditableTextBlock )
        checkSlow( this->ConsoleHistoryContainer )

        this->ConsolePreview->SetVisibility(EWidgetVisibility::Collapsed);
        this->EditableTextBlock->SetVisibility(EWidgetVisibility::Visible);
        this->ConsoleHistoryContainer->SetVisibility(EWidgetVisibility::Visible);
        this->Intellisense->SetVisibility(EWidgetVisibility::Collapsed);

        this->EditableTextBlock->ClearContent();
        this->GetViewport()->FocusWidgetNode(this->EditableTextBlock);

        this->ConsoleHistoryContainer->ApplyVScroll(WScrollRegion::MaxScrollDown);

        this->HistoryCursor = INDEX_NONE;

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
        this->Intellisense->SetVisibility(EWidgetVisibility::Collapsed);

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

    this->AddNewMessage(InText);

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
