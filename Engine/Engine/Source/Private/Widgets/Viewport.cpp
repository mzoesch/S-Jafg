// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/Viewport.h"
#include "Platform/Surface.h"
#include "User/Input/Replies.h"
#include "Widgets/UserWidget.h"

void Jafg::LViewport::Initialize()
{
}

void Jafg::LViewport::DispatchInputs(LSurface& Context, const LVector2& InLocation)
{
    this->LastFrameHoveredWidgets.CopyFrom(this->HoveredWidgets);
    this->HoveredWidgets.Reset(this->HoveredWidgets.GetSize());

    // Sweep cursor input over widgets.
    for (WUserWidget* Widget : this->TopLevelWidgets)
    {
        if (Widget->ShouldCheckForInputs() == false)
        {
            continue;
        }

        LCursorReply Reply = Widget->SweepMouse(*this, InLocation);

        if (Reply.IsHandled() == false)
        {
            continue;
        }

        if (Reply.GetCursorType() != EMouseCursor::None)
        {
            Context.SetMouseCursor(Reply.GetCursorType());
        }

        if (Reply.ShouldLooseFocus())
        {
            if (this->FocusedWidget)
            {
                this->FocusedWidget->OnFocusLost();
                this->FocusedWidget = nullptr;
            }
        }

        if (Reply.IsFocusedWidgetValid())
        {
            if (this->FocusedWidget != Reply.GetFocusedWidget())
            {
                if (this->FocusedWidget)
                {
                    this->FocusedWidget->OnFocusLost();
                }
                this->FocusedWidget = Reply.GetFocusedWidget();
                this->FocusedWidget->OnFocusReceived();
            }
        }

        break;
    }

    // Check for cursor leave events.
    LCursorReply MostRecentReply = LCursorReply::Unhandled();
    for (WWidgetNode* Node : this->LastFrameHoveredWidgets)
    {
        if (this->HoveredWidgets.Contains(Node) == false)
        {
            LCursorReply Reply = Node->OnCursorLeave();
            if (Reply.IsHandled())
            {
                MostRecentReply = Reply;
            }
        }

        continue;
    }
    if (MostRecentReply.IsHandled() && MostRecentReply.GetCursorType() != EMouseCursor::None)
    {
        Context.SetMouseCursor(MostRecentReply.GetCursorType());
    }

    // Check for left-mouse-button down events to focus on another widget.
    if (Context.IsNewKeyDown(EKeys::LeftMouseButton))
    {
        for (WUserWidget* Widget : this->TopLevelWidgets)
        {
            if (Widget->ShouldCheckForInputs() == false)
            {
                continue;
            }

            LReply Reply = Widget->SweepFocusTest(*this, InLocation);

            if (Reply.IsHandled() == false)
            {
                continue;
            }

            if (Reply.ShouldLooseFocus())
            {
                if (this->FocusedWidget)
                {
                    this->FocusedWidget->OnFocusLost();
                    this->FocusedWidget = nullptr;
                }
            }

            if (Reply.IsFocusedWidgetValid())
            {
                if (this->FocusedWidget != Reply.GetFocusedWidget())
                {
                    if (this->FocusedWidget)
                    {
                        this->FocusedWidget->OnFocusLost();
                    }
                    this->FocusedWidget = Reply.GetFocusedWidget();
                    this->FocusedWidget->OnFocusReceived();
                }
            }

            break;
        }
    }

    // Check if the focused widget is valid to be focused.
    if (this->FocusedWidget)
    {
        bool bIsDrawn = false;
        for (const WUserWidget* Widget : this->TopLevelWidgets)
        {
            if (Widget->FindNodeInVisiblePath(this->FocusedWidget))
            {
                bIsDrawn = true;
                break;
            }

            continue;
        }

        if (bIsDrawn == false)
        {
            this->FocusedWidget->OnFocusLost();
            this->FocusedWidget = nullptr;
        }
    }

    return;
}

void Jafg::LViewport::OnMouseLeftViewport(LSurface& Context)
{
    if (this->FocusedWidget)
    {
        this->FocusedWidget->OnFocusLost();
        this->FocusedWidget = nullptr;
    }

    if (this->HoveredWidgets.IsEmpty() == false || this->LastFrameHoveredWidgets.IsEmpty() == false)
    {
        LCursorReply MostRecentReply = LCursorReply::Unhandled();
        for (WWidgetNode* Node : this->HoveredWidgets)
        {
            LCursorReply Reply = Node->OnCursorLeave();
            if (Reply.IsHandled())
            {
                MostRecentReply = Reply;
            }
        }
        if (MostRecentReply.IsHandled())
        {
            Context.SetMouseCursor(MostRecentReply.GetCursorType());
        }

        this->HoveredWidgets.Empty();
        this->LastFrameHoveredWidgets.Empty();
    }

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
