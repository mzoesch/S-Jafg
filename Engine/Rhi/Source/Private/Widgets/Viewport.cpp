// Copyright mzoesch. All rights reserved.

#include "Widgets/Viewport.h"
#include "Widgets/UserWidget.h"

void Jafg::LViewport::Initialize()
{
}

void Jafg::LViewport::Tick()
{
    for (WUserWidget* Widget : this->TopLevelWidgets)
    {
        if (Widget->ShouldNowTick())
        {
            Widget->Tick();
        }

        continue;
    }

    return;
}

void Jafg::LViewport::Draw()
{
    for (const WUserWidget* Widget : this->TopLevelWidgets)
    {
        Widget->ViewportDrawEntry(this);
        // Widget->Draw(this);
    }

    return;
}

void Jafg::LViewport::TearDown()
{
    for (WUserWidget* Widget : this->TopLevelWidgets)
    {
        Widget->MarkAsGarbage();
    }

    this->TopLevelWidgets.Empty();

    return;
}

void Jafg::LViewport::AddWidget(WUserWidget* Widget)
{
    check( Widget )
    this->TopLevelWidgets.Add(Widget);
}

void Jafg::LViewport::RemoveWidget(WUserWidget* Widget)
{
    this->TopLevelWidgets.RemoveOnceChecked(Widget);
}

void Jafg::LViewport::ChangeDimensions(const LIntVector2& InDimensions)
{
    check( InDimensions.X > 0 && InDimensions.Y > 0 )

    this->Dimensions = InDimensions;

    return;
}
