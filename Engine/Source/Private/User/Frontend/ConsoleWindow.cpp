// Copyright mzoesch. All rights reserved.

#include "User/Frontend/ConsoleWindow.h"
#include "User/Frontend/ConsoleStdIn.h"
#include "Widgets/ScrollRegion.h"
#include "Widgets/VRegion.h"
#include "Widgets/HRegion.h"
#include "Cli/CliStatics.h"
#include "Cli/CliCommand.h"
#include "Cli/CommandLineInterface.h"
#include "Widgets/EditableTextBoxForward.h"
#include "Widgets/EditableTextBox.h"

namespace
{

FORCEINLINE LColor LogToColor(auto const& Verbosity) noexcept
{
    if (Verbosity == "Trace") { return Colors::NotSoDarkGray; }
    if (Verbosity == "Verbose") { return Colors::DarkGray; }
    if (Verbosity == "Info") { return Colors::White; }
    if (Verbosity == "Warning") { return Colors::Yellow; }
    if (Verbosity == "Error") { return Colors::Red; }

    return Colors::White;
}

} /* ~Namespace <Anonymous> */

ENGINE_API TOptional<LVec2F> Jafg::WConsoleWindow::LastWindowLocation;
ENGINE_API TOptional<LVec2F> Jafg::WConsoleWindow::LastWindowSize;

Jafg::WConsoleWindow::WConsoleWindow(LCxxObjectInitializer const& ObjectInitializer)
    : Super{ObjectInitializer}
{
    this->SetShouldTick(true);
    return;
}

void Jafg::WConsoleWindow::Construct()
{
    Super::Construct();

    WRegion* Content;
    NewNode(WRegion).SaveTo(&Content)
        .Type(ERegionBrush::OutlineBox)
        .Tint(Colors::DarkSlateGray)
        .OutlineThickness(1)
        .OutlineTint(Colors::Black)
    [
        NewNode(WOverlay)
            .Anchor(EAnchor::Fill)
            .Padding({0_spt, 0, 0, 18})
        [
            NewNode(WScrollRegion).SaveTo(&this->HistoryScrollRegion)
                .Anchor(EAnchor::Fill)
            [
                NewNode(WVRegion).SaveTo(&this->HistoryRegion)
                    .Anchor(EAnchor::Fill)
                    .Padding({4_spt, 8})
                    .VSpace(5_spt)
            ]
        ]
        +
        NewNode(WConsoleStdIn).SaveTo(&this->StdIn)
            .Anchor(EAnchor::Fill)
    ];

    this->SetContentNode(*Content);
    MakeDeferredWidgetNodeFinal(Content);

    check( Content->GetVisibility() == EWidgetVisibility::Visible )

    if (WConsoleWindow::LastWindowLocation.has_value())
    {
        this->SetWindowPosition(WConsoleWindow::LastWindowLocation.value());
    }

    this->SetWindowTitle("Console");

    this->LoadLogs(0);
    this->HistoryScrollRegion->ApplyVScroll(WScrollRegion::MaxScrollDown);

    this->StdIn->OnTextCommit.Bind(this, &WConsoleWindow::StdInCommit);

    if (this->bHighlightOnConstruct)
    {
        this->GetViewport().FocusWidgetNode(this->StdIn->GetEditableTextBox());
    }

    return;
}

void Jafg::WConsoleWindow::OnGarbage(ECxxRecordTearDownReason::Type Reason, LClassOuter& PreviousOuter)
{
    if (auto* Window{ this->GetWindow() })
    {
        if (auto Margin{ Window->GetMargin() }; Margin.has_value())
        {
            WConsoleWindow::LastWindowLocation = {Margin->Left, Margin->Top};
        }
    }

    Super::OnGarbage(Reason, PreviousOuter);

    return;
}

void Jafg::WConsoleWindow::Tick()
{
    Super::Tick();

    if (this->LoadLogs(this->LoadedLogCount))
    {
        this->HistoryScrollRegion->ApplyVScroll(WScrollRegion::MaxScrollDown);
    }

    return;
}

void Jafg::WConsoleWindow::AddToConsole(LConsoleMessage Message)
{
    check( Tasks::IsOnMasterThread() )
    check( this->HistoryRegion )

    auto* Node{ ConstructWidgetNode<WTextBox>(this->HistoryRegion) };
    Node->SetContent(std::move(Message.Content));
    Node->SetTextColor(Message.Color);

    return;
}

u64 Jafg::WConsoleWindow::LoadLogs(u64 Start)
{
    check( Tasks::IsOnMasterThread() )

    std::unique_lock Lock{ Detail::GLongLiquidLogsMutex };
    while (algo::is_valid_index(Detail::GLongLiquidLogs, Start))
    {
        std::string const& Message{ std::get<1>(Detail::GLongLiquidLogs[Start]) };
        this->AddToConsole
        ({
            "Log",
            LString{Message.begin(), Message.end()},
            LogToColor(std::get<0>(Detail::GLongLiquidLogs[Start])),
        });

        ++Start;
    }

    const u64 Diff{ Start - this->LoadedLogCount };

    this->LoadedLogCount = Start;

    return Diff;
}

bool Jafg::WConsoleWindow::StdInCommit(LString const& Text, ETextCommit::Type CommitType)
{
    if (CommitType == ETextCommit::OnCleared)
    {
        this->RemoveFromParent();
        return true;
    }

    if (CommitType == ETextCommit::FocusLost)
    {
        return false;
    }

    this->StdIn->ResetHistoryCursor();

    if (Text.empty())
    {
        return false;
    }

    LString CmdLine{ Text };
    if (CliStatics::IsCommand(CmdLine))
    {
        algo::inline_right_chop(&CmdLine, 1);
    }

    if (CmdLine.empty())
    {
        return false;
    }

    LCommandExecutionResponse Response;
    this->GetCommandLineInterface().Invoke(CmdLine, &Response);

    if (Response.StdOut.empty() == false)
    {
        LOG_INFO(LogCli, "[{}] executed RC[{}]: {}.", CmdLine, LexToString(Response.Rc), Response.StdOut)
    }

    if (Response.StdErr.empty() == false)
    {
        LOG_ERROR(LogCli, "[{}] failed RC[{}]: {}.", CmdLine, LexToString(Response.Rc), Response.StdErr)
    }

    if (Response.StdOut.empty() && Response.StdErr.empty() && Response.Rc != ECommandReturnCode::SuccessNoResponse)
    {
        LOG_INFO(LogCli, "Command [{}] executed RC[{}] with no output.", CmdLine, LexToString(Response.Rc))
    }

    return false;
}
