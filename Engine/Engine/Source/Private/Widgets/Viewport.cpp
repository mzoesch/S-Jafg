// Copyright mzoesch. All rights reserved.

#include "Widgets/Viewport.h"
#include "Engine/Engine.h"
#include "Framework/Eye.h"
#include "Platform/Surface.h"
#include "Rhi/EngineShader.h"
#include "Rhi/RendererStateMachine.h"
#include "User/Input/Replies.h"
#include "Widgets/UserWidget.h"

void Jafg::LViewport::ClearInvalidWidgets()
{
    if constexpr (IS_COMPILED_LOG(LogWidgetFramework, Verbose))
    {
        if (this->FocusedWidget.IsNotNull() && this->FocusedWidget.IsValidDeep() == false)
        {
            LOG_VERBOSE(LogWidgetFramework, "Current focused widget is invalid.")
            this->FocusedWidget.Reset();
        }
    }
    else
    {
        if (this->FocusedWidget.IsValidDeep() == false)
        {
            this->FocusedWidget.Reset();
        }
    }

    auto ClearOnContainer {[](TArray<TObjectStorage<WNode>>& InContainer) -> void
    {
        if constexpr (IS_COMPILED_LOG(LogWidgetFramework, Verbose))
        {
            if (const i32 Removed = InContainer.RemoveByPredicate([](const TObjectStorage<WNode>& InNode)
            {
                return InNode.IsValidDeep() == false;
            }); Removed > 0)
            {
                LOG_VERBOSE(LogWidgetFramework, "Found [{}] hovered widgets from last frame that are now invalid.", Removed)
            }

        }
        else
        {
            InContainer.RemoveByPredicate([](const TObjectStorage<WNode>& InNode)
            {
                return InNode.IsValidDeep() == false;
            });

            return;
        }

        return;
    }};

    ClearOnContainer(this->LastFrameHoveredWidgets);
    ClearOnContainer(this->HoveredWidgets);

    return;
}

void Jafg::LViewport::DispatchInputs(LSurface& Context, const LVector2& InCursorLocation)
{
    this->CachedContext = &Context;
    this->SweepTranslation = LVector2::ZeroVector;

    const bool bCursorLocationIsMeaningful = InCursorLocation.X >= 0.0f && InCursorLocation.Y >= 0.0f;

    if (bCursorLocationIsMeaningful)
    {
        this->CachedCursorLocation = InCursorLocation;
    }
    else
    {
        this->CachedCursorLocation.Reset();
    }

    this->LastFrameHoveredWidgets.CopyFrom(this->HoveredWidgets);
    if (bCursorLocationIsMeaningful)
    {
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

            if (const LCursorReply Reply = Widget->SweepMouse(*this, InCursorLocation); Reply.IsHandled())
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
        for (WNode* Node : this->LastFrameHoveredWidgets)
        {
            if (this->HoveredWidgets.Contains(Node) == false)
            {
                if (const LCursorReply Reply = Node->OnCursorLeave(); MostRecentReply.IsHandled() == false && Reply.IsHandled())
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

            if (const LReply Reply = Widget->SweepFocusTest(*this, InCursorLocation); Reply.IsHandled())
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
            LOG_VERBOSE(LogWidgetFramework, "Lost focus on [{}].", this->FocusedWidget->GetFullName())
            this->FocusedWidget->OnFocusLost();
            this->FocusedWidget = nullptr;
        }
    }

    // Check for key down events.
    for (LRawInput& Input : Context.GetCurrentlyPressedKeys())
    {
        if (Context.IsNewKeyDown(Input) == false)
        {
            continue;
        }

        if (this->FocusedWidget)
        {
            if (const LReply Reply = this->FocusedWidget->OnKeyDown(*this, Input); Reply.IsHandled())
            {
                this->HandleReply(Context, Reply);
                continue;
            }
        }

        if (bCursorLocationIsMeaningful)
        {
            for (WUserWidget* Widget : this->TopLevelWidgets)
            {
                if (Widget == this->FocusedWidget || Widget->ShouldCheckForInputs() == false)
                {
                    continue;
                }

                if (Widget->IsInBounds(*this, InCursorLocation) == false)
                {
                    continue;
                }

                if (const LReply Reply = Widget->OnKeyDownNoFocus(*this, Input); Reply.IsHandled())
                {
                    this->HandleReply(Context, Reply);
                    break;
                }

                continue;
            }
        }

        continue;
    }

    // Check for platform repeat key down events.
    if (this->FocusedWidget)
    {
        if (Context.HasRepeatedKey())
        {
            if (const LReply Reply = this->FocusedWidget->OnRepeatedKeyDown({Context.GetRepeatedKey(), true}); Reply.IsHandled())
            {
                this->HandleReply(Context, Reply);
            }
        }
    }

    // Check for key up events.
    for (LRawInput& Input : Context.GetLastFramePressedKeys())
    {
        if (Context.IsKeyUp(Input) == false)
        {
            continue;
        }

        if (this->FocusedWidget)
        {
            if (const LReply Reply = this->FocusedWidget->OnKeyUp(*this, Input); Reply.IsHandled())
            {
                this->HandleReply(Context, Reply);
                continue;
            }
        }

        if (bCursorLocationIsMeaningful)
        {
            for (WUserWidget* Widget : this->TopLevelWidgets)
            {
                if (Widget == this->FocusedWidget || Widget->ShouldCheckForInputs() == false)
                {
                    continue;
                }

                if (Widget->IsInBounds(*this, InCursorLocation) == false)
                {
                    continue;
                }

                if (const LReply Reply = Widget->OnKeyUpNoFocus(*this, Input); Reply.IsHandled())
                {
                    this->HandleReply(Context, Reply);
                    break;
                }

                continue;
            }
        }

        continue;
    }

    return;
}

void Jafg::LViewport::OnMouseLeftViewport(LSurface& Context, const bool bInvalidateAllInputs)
{
    this->CachedContext = &Context;

    if (this->HoveredWidgets.IsEmpty() == false || this->LastFrameHoveredWidgets.IsEmpty() == false)
    {
        LCursorReply MostRecentReply = LCursorReply::Unhandled();
        for (WNode* Node : this->HoveredWidgets)
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
        LOG_VERBOSE(LogWidgetFramework, "Lost focus on [{}].", this->FocusedWidget->GetFullName())
        this->FocusedWidget->OnFocusLost();
        this->FocusedWidget = nullptr;
    }

    return;
}

void Jafg::LViewport::OnClear()
{
    if (this->bChangedBackgroundColor)
    {
        this->bChangedBackgroundColor = false;
        this->BackgroundBuffer.ResetAndMakeDrawTarget(this->BackgroundColor);
    }
    else
    {
        this->BackgroundBuffer.ResetAndMakeDrawTarget();
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

    this->OnLateTick.Broadcast(*this);

    return;
}

void Jafg::LViewport::Draw()
{
    this->FrameZLayerDepth = 0.0f;
    this->FrameTranslation = LVector2::ZeroVector;
    this->RecalculateScaleFactor();

    this->BackgroundBuffer.MakeDrawTarget();

    RendererStateMachine::PrepareForPerspectivePainting();
    for (const auto& [Eye, World] : this->BackgroundContexts)
    {
        check( Eye && World )
        Eye->UpdateViewMatrix();

        for (LEngineShader* Shader : GEngine->GetShaders() | std::views::values)
        {
            checkSlow( Shader && Shader->IsValid() )
            Shader->UpdateWorldUniforms(*this, *World, *Eye);
            continue;
        }

        World->Draw(*this, *Eye);

        continue;
    }

    LFrameBuffer::ResetAndMakeDefaultDrawTarget();
    RendererStateMachine::PrepareForOrthographicPainting();
    for (LEngineShader* Shader : GEngine->GetShaders() | std::views::values)
    {
        checkSlow( Shader && Shader->IsValid() )
        Shader->UpdateViewportUniforms(*this);
        continue;
    }

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

    checkCode
    (
        if (this->FrameTranslation.IsNearlyZero() == false)
        {
            LOG_WARNING(LogWidgetFramework, "Viewport translation state is not zero: [{}].", this->FrameTranslation.ToString())
        }
    )

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

Jafg::WNode* Jafg::LViewport::GetTopLevelWidgetByClass(const LObjectClass* WidgetClass) const
{
    for (WUserWidget* Widget : this->TopLevelWidgets)
    {
        if (Widget->GetVTableChecked()->DerivesFrom(WidgetClass))
        {
            return Widget;
        }

        continue;
    }

    return nullptr;}

bool Jafg::LViewport::FocusWidgetNode(WNode* InNode)
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

bool Jafg::LViewport::AddHoveredWidgetForFrame(WNode* Node)
{
    check( this->HoveredWidgets.Contains(Node) == false )
    this->HoveredWidgets.Emplace(Node);
    return this->LastFrameHoveredWidgets.Contains(Node) == false;
}

void Jafg::LViewport::ChangeFocusUnsafe(WNode* InNode)
{
    if (this->FocusedWidget)
    {
        LOG_VERBOSE(LogWidgetFramework, "Lost focus on [{}].", this->FocusedWidget->GetFullName())
        this->FocusedWidget->OnFocusLost();
    }

    this->FocusedWidget = InNode;

    if (this->FocusedWidget)
    {
        LOG_VERBOSE(LogWidgetFramework, "Gained focus on [{}].", this->FocusedWidget->GetFullName())
        this->FocusedWidget->OnFocusReceived();
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
