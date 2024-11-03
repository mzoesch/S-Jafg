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
    /*
     * Maybe we want to make a callback to this. So that we do not have to recalculate
     * this every frame. But who cares? Its just one single floating point operation.
     */
    this->RecalculateScaleFactor();

    for (const WUserWidget* Widget : this->TopLevelWidgets)
    {
        Widget->UpdateDesiredSize();
        Widget->Draw(this);
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

void Jafg::LViewport::RecalculateScaleFactor()
{
    this->ScaleFactor = this->PlatformDpi / this->BaseDpi;
}
