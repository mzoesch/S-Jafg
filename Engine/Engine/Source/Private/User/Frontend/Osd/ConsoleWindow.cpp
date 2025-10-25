// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/ConsoleWindow.h"
#include "User/Frontend/Osd/ConsoleStdIn.h"

ENGINE_API TOptional<LVector2> Jafg::WConsoleWindow::LastWindowLocation;
ENGINE_API TOptional<LVector2> Jafg::WConsoleWindow::LastWindowSize;

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
    ];

    this->SetContentNode(*Content);
    MakeDeferredWidgetNodeFinal(Content);

    check( Content->GetVisibility() == EWidgetVisibility::Visible )

    if (WConsoleWindow::LastWindowLocation.has_value())
    {
        this->SetWindowPosition(WConsoleWindow::LastWindowLocation.value());
    }

    this->SetWindowTitle("Console");

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
