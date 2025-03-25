// Copyright mzoesch. All rights reserved.

#include "Widgets/Viewport.h"
#include "Engine/Engine.h"
#include "Framework/Eye.h"
#include "Platform/Surface.h"
#include "Rhi/EngineShader.h"
#include "Rhi/RendererStateMachine.h"
#include "User/Input/Replies.h"
#include "Widgets/UserWidget.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_access.hpp"

void Jafg::LViewport::DispatchInputs(LSurface& Context, const LVector2& InCursorLocation)
{
    const bool bCursorLocationIsMeaningful = InCursorLocation.X >= 0.0f && InCursorLocation.Y >= 0.0f;

    if (bCursorLocationIsMeaningful)
    {
        this->LastFrameHoveredWidgets.CopyFrom(this->HoveredWidgets);
        this->HoveredWidgets.Reset(this->HoveredWidgets.GetSize());
    }

    // Sweep cursor input over widgets.
    if (bCursorLocationIsMeaningful)
    {
        for (WUserWidget* Widget : this->TopLevelWidgets)
        {
            if (Widget->ShouldCheckForInputs() == false)
            {
                continue;
            }

            LCursorReply Reply = Widget->SweepMouse(*this, InCursorLocation);
            if (Reply.IsHandled())
            {
                this->HandleReply(Context, Reply);
                break;
            }

            continue;
        }
    }

    // Check for cursor leave events.
    if (bCursorLocationIsMeaningful)
    {
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
        if (MostRecentReply.IsHandled())
        {
            this->HandleReply(Context, MostRecentReply);
        }
    }

    // Check for left-mouse-button down events to focus on another widget.
    if (bCursorLocationIsMeaningful && Context.IsNewKeyDown(EKeys::LeftMouseButton))
    {
        bool bIsHandled = false;
        for (WUserWidget* Widget : this->TopLevelWidgets)
        {
            if (Widget->ShouldCheckForInputs() == false)
            {
                continue;
            }

            LReply Reply = Widget->SweepFocusTest(*this, InCursorLocation);
            if (Reply.IsHandled())
            {
                this->HandleReply(Context, Reply);
                bIsHandled = true;
                break;
            }

            continue;
        }
        if (bIsHandled == false)
        {
            this->HandleReply(Context, LReply::HandledWithFocusLost());
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

    // Check for key down events.
    if (this->FocusedWidget)
    {
        for (LRawInput& Input : Context.GetCurrentlyPressedKeys())
        {
            if (Context.IsNewKeyDown(Input) == false)
            {
                continue;
            }

            LKeyEvent KeyEvent = LKeyEvent(Input);
            LReply Reply = this->FocusedWidget->OnKeyDown(KeyEvent);
            if (Reply.IsHandled())
            {
                this->HandleReply(Context, Reply);
                break;
            }

            continue;
        }
    }

    // Check for platform repeat key down events.
    if (this->FocusedWidget)
    {
        if (Context.HasRepeatedKey())
        {
            LKeyEvent KeyEvent = LKeyEvent(Context.GetRepeatedKey(), true);
            if (const LReply Reply = this->FocusedWidget->OnKeyDown(KeyEvent); Reply.IsHandled())
            {
                this->HandleReply(Context, Reply);
            }
        }
    }

    // Check for key up events.
    if (this->FocusedWidget)
    {
        for (LRawInput& Input : Context.GetLastFramePressedKeys())
        {
            if (Context.IsKeyUp(Input) == false)
            {
                continue;
            }

            LKeyEvent KeyEvent = LKeyEvent(Input);
            LReply Reply = this->FocusedWidget->OnKeyUp(KeyEvent);
            if (Reply.IsHandled())
            {
                this->HandleReply(Context, Reply);
                break;
            }

            continue;
        }
    }

    return;
}

void Jafg::LViewport::OnMouseLeftViewport(LSurface& Context, const bool bInvalidateAllInputs)
{
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

    if (bInvalidateAllInputs && this->FocusedWidget)
    {
        this->FocusedWidget->OnFocusLost();
        this->FocusedWidget = nullptr;
    }

    return;
}

void Jafg::LViewport::OnClear()
{
    this->BackgroundBuffer.ResetAndMakeDrawTarget();
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

// Jafg::LVector3 PlaneIntersection(const Jafg::LPlane& Plane1, const Jafg::LPlane& Plane2, const Jafg::LPlane& Plane3)
// {
//     using namespace Jafg;
//     LMatrix3 A = LMatrix3(Plane1.GetPlaneNormal(), Plane2.GetPlaneNormal(), Plane3.GetPlaneNormal());
//     LVector3 B = LVector3(-Plane1.W, -Plane2.W, -Plane3.W);
//
//     glm::mat3 Ac;
//     inverse(Ac)
//
//     return A.GetInverse() * B;
// }

void Jafg::LViewport::Draw()
{
    this->FrameZLayerDepth = 0.0f;
    this->RecalculateScaleFactor();

    this->BackgroundBuffer.MakeDrawTarget();

    RendererStateMachine::PrepareForPerspectivePainting();
    for (const auto& [Eye, World] : this->BackgroundContexts)
    {
        check( Eye && World )
        Eye->UpdateViewMatrix();

        // const LMatrix P = Maths::MakePerspectiveProjectionMatrix
        // (
        //     Maths::ToRadians(Eye->GetDegYFov()),
        //     static_cast<float>(this->GetDimensions().X) / static_cast<float>(this->GetDimensions().Y),
        //     0.1f, 2000.0f
        // );
        // const LMatrix V = Eye->GetViewMatrix();
        // const LMatrix PV = P * V;
        // const LVector4 Rx = PV.GetRow(LMatrix::X);
        // const LVector4 Ry = PV.GetRow(LMatrix::Y);
        // const LVector4 Rz = PV.GetRow(LMatrix::Z);
        // const LVector4 Rw = PV.GetRow(LMatrix::W);
        //
        // const LVector4 FrustumL = Rw + Rx;
        // const LVector4 FrustumR = Rw - Rx;
        // const LVector4 FrustumT = Rw - Ry;
        // const LVector4 FrustumB = Rw + Ry;
        // const LVector4 FrustumN = Rw + Rz;
        // const LVector4 FrustumF = Rw - Rz;

        for (LEngineShader* Shader : GEngine->GetShaders())
        {
            checkSlow( Shader )
            Shader->UpdateUniforms(*this, *World, *Eye);
            continue;
        }

        World->Draw(*this, *Eye);

        continue;
    }

    LFrameBuffer::ResetAndMakeDefaultDrawTarget();
    RendererStateMachine::PrepareForOrthographicPainting();
    this->BackgroundBuffer.PaintToViewport(*this);

    for (const WUserWidget* Widget : this->TopLevelWidgets)
    {
        if (Widget->TransformsWidgetLayout())
        {
            Widget->UpdateDesiredSize();
            Widget->UpdateAnchoredSize(*this);
            if (Widget->ShouldNowDraw())
            {
                Widget->Draw(*this);
            }
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

bool Jafg::LViewport::TryRemoveWidget(WUserWidget* Widget)
{
    return this->TopLevelWidgets.RemoveOnce(Widget);
}

void Jafg::LViewport::ChangeDimensions(const LIntVector2& InDimensions)
{
    check( InDimensions.X > 0 && InDimensions.Y > 0 )

    this->Dimensions = InDimensions;

    if (this->BackgroundBuffer.IsMeaningful())
    {
        this->BackgroundBuffer = { };
    }

    if (GEngine)
    {
        this->BackgroundBuffer.Build(this->GetDimensions());
    }
    else
    {
        Tasks::Make(ENamedThreads::Master, ETaskTime::AfterCorePackageLoad, [this] { this->BackgroundBuffer.Build(this->GetDimensions()); });
    }

    return;
}

Jafg::WWidgetNode* Jafg::LViewport::GetTopLevelWidgetByClass(const LObjectClass* WidgetClass) const
{
    for (WUserWidget* Widget : this->TopLevelWidgets)
    {
        if (Widget->GetVTable()->DerivesFrom(WidgetClass))
        {
            return Widget;
        }

        continue;
    }

    return nullptr;}

bool Jafg::LViewport::FocusWidgetNode(const WWidgetNode* InNode)
{
    if (InNode == nullptr)
    {
        return false;
    }

    for (const WUserWidget* Widget : this->TopLevelWidgets)
    {
        if (Widget->FindNodeInVisiblePath(InNode))
        {
            this->ChangeFocusUnsafe(InNode);
            return true;
        }

        continue;
    }

    /*
     * The node cannot be focused because it is not visible.
     */
    return false;
}

bool Jafg::LViewport::AddHoveredWidgetForFrame(WWidgetNode* Node)
{
    check( this->HoveredWidgets.Contains(Node) == false )
    this->HoveredWidgets.Add(Node);
    return this->LastFrameHoveredWidgets.Contains(Node) == false;
}

void Jafg::LViewport::ChangeFocusUnsafe(const WWidgetNode* InNode)
{
    if (this->FocusedWidget)
    {
        this->FocusedWidget->OnFocusLost();
    }

    if (InNode)
    {
        this->FocusedWidget = const_cast<WWidgetNode*>(InNode);
        this->FocusedWidget->OnFocusReceived();
    }
    else
    {
        this->FocusedWidget = nullptr;
    }

    return;
}

void Jafg::LViewport::RecalculateScaleFactor()
{
    this->ScaleFactor = this->PlatformDpi / this->BaseDpi;
}

void Jafg::LViewport::HandleReply(LSurface& Context, const LCursorReply& Reply)
{
    check( Reply.IsHandled() )

    if (Reply.GetCursorType() != EMouseCursor::None)
    {
        Context.SetMouseCursor(Reply.GetCursorType());
    }

    if (Reply.ShouldLooseFocus())
    {
        this->ChangeFocusUnsafe(nullptr);
    }

    if (Reply.IsFocusedWidgetValid())
    {
        if (this->FocusedWidget != Reply.GetFocusedWidget())
        {
            this->ChangeFocusUnsafe(Reply.GetFocusedWidget());
        }
    }

    return;
}

void Jafg::LViewport::HandleReply(LSurface& Context, const LReply& Reply)
{
    check( Reply.IsHandled() )

    if (Reply.ShouldLooseFocus())
    {
        this->ChangeFocusUnsafe(nullptr);
    }

    if (Reply.IsFocusedWidgetValid())
    {
        if (this->FocusedWidget != Reply.GetFocusedWidget())
        {
            this->ChangeFocusUnsafe(Reply.GetFocusedWidget());
        }
    }

    return;
}
