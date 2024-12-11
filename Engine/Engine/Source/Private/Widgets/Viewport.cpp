// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/Viewport.h"
#include "User/Input/Replies.h"
#include "Widgets/UserWidget.h"

void Jafg::LViewport::Initialize()
{
}

void Jafg::LViewport::DispatchInputs(const LVector2& InLocation)
{
    this->LastFrameHoveredWidgets.CopyFrom(this->HoveredWidgets);
    this->HoveredWidgets.Reset(this->HoveredWidgets.GetSize());

    for (WUserWidget* Widget : this->TopLevelWidgets)
    {
        if (Widget->ShouldCheckForInputs())
        {
            LCursorReply Reply = Widget->SweepMouse(*this, InLocation);

            if (Reply.IsHandled())
            {
                break;
            }
        }

        continue;
    }

    for (WWidgetNode* Node : this->LastFrameHoveredWidgets)
    {
        if (this->HoveredWidgets.Contains(Node) == false)
        {
            Node->OnCursorLeave();
        }

        continue;
    }

    return;
}

void Jafg::LViewport::OnMouseLeftViewport()
{
    for (WWidgetNode* Node : this->HoveredWidgets)
    {
        Node->OnCursorLeave();
    }

    this->HoveredWidgets.Empty();
    this->LastFrameHoveredWidgets.Empty();

    return;
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
    this->FrameZLayerDepth = 0.0f;

    this->RecalculateScaleFactor();

    for (const WUserWidget* Widget : this->TopLevelWidgets)
    {
        if (Widget->ShouldNowDraw())
        {
            Widget->UpdateDesiredSize();
            Widget->UpdateAnchoredSize(*this);
            Widget->Draw(*this);
        }

        continue;
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

Jafg::WWidgetNode* Jafg::LViewport::GetTopLevelWidgetByClass(const LObjectClass* WidgetClass)
{
    for (WUserWidget* Widget : this->TopLevelWidgets)
    {
        if (Widget->GetVTable()->DerivesFrom(WidgetClass))
        {
            return Widget;
        }

        continue;
    }

    return nullptr;
}

const Jafg::WWidgetNode* Jafg::LViewport::GetTopLevelWidgetByClass(const LObjectClass* WidgetClass) const
{
    return const_cast<LViewport*>(this)->GetTopLevelWidgetByClass(WidgetClass);
}

bool Jafg::LViewport::AddHoveredWidgetForFrame(WWidgetNode* Node)
{
    check( this->HoveredWidgets.Contains(Node) == false )
    this->HoveredWidgets.Add(Node);
    return this->LastFrameHoveredWidgets.Contains(Node) == false;
}

void Jafg::LViewport::RecalculateScaleFactor()
{
    this->ScaleFactor = this->PlatformDpi / this->BaseDpi;
}
