// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/ConsoleWindow.h"
#include "User/Frontend/Osd/ConsoleStdIn.h"
#include "Widgets/ScrollRegion.h"
#include "Widgets/VRegion.h"
#include "Widgets/HRegion.h"

namespace
{

FORCEINLINE Lal::LColor LogToColor(auto const& Verbosity) noexcept
{
    if (Verbosity == "Trace") { return Lal::LColor::NotSoDarkGray; }
    if (Verbosity == "Verbose") { return Lal::LColor::DarkGray; }
    if (Verbosity == "Info") { return Lal::LColor::White; }
    if (Verbosity == "Warning") { return Lal::LColor::Yellow; }
    if (Verbosity == "Error") { return Lal::LColor::Red; }

    return Lal::LColor::White;
}

} /* ~Namespace <Anonymous> */

ENGINE_API TOptional<LVector2> Jafg::WConsoleWindow::LastWindowLocation;
ENGINE_API TOptional<LVector2> Jafg::WConsoleWindow::LastWindowSize;

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
        .Tint(Lal::LColor::DarkSlateGray)
        .OutlineThickness(1)
        .OutlineTint(Lal::LColor::Black)
    [
        NewNode(WConsoleStdIn)
            .Anchor(EAnchor::Fill)
        +
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

    return;
}

void Jafg::WConsoleWindow::OnGarbage(ECxxRecordTearDownReason::Type Reason)
{
    if (auto* Window{ this->GetWindow() })
    {
        if (auto Margin{ Window->GetMargin() }; Margin.has_value())
        {
            WConsoleWindow::LastWindowLocation = LVector2{ Margin->Left, Margin->Top };
        }
    }

    Super::OnGarbage(Reason);

    return;
}

void Jafg::WConsoleWindow::Tick()
{
    Super::Tick();

    this->LoadLogs(this->LoadedLogCount);
    this->HistoryScrollRegion->ApplyVScroll(WScrollRegion::MaxScrollDown);

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

void Jafg::WConsoleWindow::LoadLogs(u64 Start)
{
    check( Tasks::IsOnMasterThread() )

    std::unique_lock Lock{ JafgCore::GLongLiquidLogsMutex };
    while (algo::is_valid_index(JafgCore::GLongLiquidLogs, Start))
    {
        std::string const& Message{ std::get<1>(JafgCore::GLongLiquidLogs[Start]) };
        this->AddToConsole
        ({
            "Log",
            LString{Message.begin(), Message.end()},
            LogToColor(std::get<0>(JafgCore::GLongLiquidLogs[Start])),
        });

        ++Start;
    }

    this->LoadedLogCount = Start;

    return;
}
