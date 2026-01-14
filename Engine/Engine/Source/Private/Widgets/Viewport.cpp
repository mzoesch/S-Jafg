// Copyright mzoesch. All rights reserved.

#include "Widgets/Viewport.h"
#include "Engine/Engine.h"
#include "Framework/Eye.h"
#include "Platform/Surface.h"
#include "User/Input/Replies.h"
#include "Widgets/UserWidget.h"
#include "Stats/Stats.h"

#include "TestWidget.h"

void Jafg::LViewport::ClearInvalidWidgets()
{
    if (this->FocusedWidget.IsValidDeep() == false)
    {
        if constexpr (IS_COMPILED_LOG(LogWidgetFramework, Verbose))
        {
            if (this->FocusedWidget.IsNotNull())
            {
                LOG_VERBOSE(LogWidgetFramework, "Current focused widget is invalid.")
            }
        }

        this->FocusedWidget.Reset();
    }

    auto ClearOnContainer{[](TArray<TClassStorage<WNode>>* InContainer) -> void
    {
        auto const Removed{ algo::erase_if(InContainer, [](auto const& E)
        {
            return E.IsValidDeep() == false;
        }) };
        if constexpr (IS_COMPILED_LOG(LogWidgetFramework, Verbose))
        {
            if (Removed > 0)
            {
                LOG_VERBOSE(LogWidgetFramework, "Found [{}] hovered widgets from last frame that are now invalid.", Removed)
            }
        }

        return;
    }};

    ClearOnContainer(&this->LastFrameHoveredWidgets);
    ClearOnContainer(&this->HoveredWidgets);

    return;
}

void Jafg::LViewport::DispatchInputs(LSurface& Context, const LVector2D& InCursorLocation)
{
    check( &this->Surface == &Context )
    this->SweepTranslation = LVector2D::ZeroVector;

    const bool bCursorLocationIsMeaningful { InCursorLocation.X >= 0.0f && InCursorLocation.Y >= 0.0f };

    if (bCursorLocationIsMeaningful)
    {
        this->CachedCursorLocation = InCursorLocation;
    }
    else
    {
        this->CachedCursorLocation.reset();
    }

    this->LastFrameHoveredWidgets = this->HoveredWidgets;
    if (bCursorLocationIsMeaningful)
    {
        this->HoveredWidgets.clear();
    }

    LCursorReply SweepReply;

    /* Sweep cursor input over widgets. */
    if (bCursorLocationIsMeaningful)
    {
        for (auto It{this->TopLevelWidgets.rbegin()}; It != this->TopLevelWidgets.rend(); ++It)
        {
            if ((*It)->ShouldCheckForInputs() == false)
            {
                continue;
            }

            if (LCursorReply Reply{ (*It)->SweepMouse(*this, InCursorLocation) }; Reply.IsHandled())
            {
                SweepReply = std::move(Reply);
                break;
            }

            continue;
        }
    }

    /* Check for cursor leave events. */
    if (bCursorLocationIsMeaningful)
    {
        LCursorReply MostRecentReply = LCursorReply::Unhandled();
        for (auto& Node : this->LastFrameHoveredWidgets)
        {
            if (algo::contains(this->HoveredWidgets, Node) == false)
            {
                if (const LCursorReply Reply { Node->OnCursorLeave() }; MostRecentReply.IsHandled() == false && Reply.IsHandled())
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

    /*
     * Handle the sweep reply after the cursor leave events, so that in the case of mutual changes to (e.g., the
     * cursor) are still reflected in the importance. Obviously, the sweep reply is more important than some random
     * fuck widget that was hovered last frame.
     */
    if (SweepReply.IsHandled())
    {
        this->HandleReply(Context, SweepReply);
    }

    /* Check for left-mouse-button down events to focus on another widget. */
    if (bCursorLocationIsMeaningful && Context.IsNewKeyDown(EKeys::LeftMouseButton))
    {
        bool bIsHandled { false };
        for (auto& Widget : this->HoveredWidgets)
        {
            if (Widget->ShouldCheckForInputs() == false)
            {
                continue;
            }

            if (const LReply Reply{ Widget->SweepFocusTest(*this, InCursorLocation) }; Reply.IsHandled())
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

    /* Check if the focused widget is valid to be focused. */
    if (this->FocusedWidget.IsValid())
    {
        bool bIsDrawn { false };
        for (auto It{ this->TopLevelWidgets.rbegin() }; It != this->TopLevelWidgets.rend(); ++It)
        {
            if ((*It)->FindNodeInVisiblePath(this->FocusedWidget.Get()))
            {
                bIsDrawn = true;
                break;
            }

            continue;
        }

        if (bIsDrawn == false)
        {
            LOG_VERBOSE(LogWidgetFramework, "Lost focus on [{}].", this->FocusedWidget->GetNameAsString())
            this->FocusedWidget->OnFocusLost();
            this->FocusedWidget = nullptr;
        }
    }

    /* Check for key down events. */
    for (const LRawInput& Input : Context.GetCurrentlyPressedKeys())
    {
        if (Input.bRepeated == false && Context.IsNewKeyDown(Input) == false)
        {
            continue;
        }

        if (this->FocusedWidget.IsNotNull())
        {
            if (const LReply Reply { this->FocusedWidget->OnKeyDown(*this, Input) }; Reply.IsHandled())
            {
                this->HandleReply(Context, Reply);
                continue;
            }
        }

        if (bCursorLocationIsMeaningful)
        {
            for (auto It{ this->TopLevelWidgets.rbegin() }; It != this->TopLevelWidgets.rend(); ++It)
            {
                if ((*It) == this->FocusedWidget || (*It)->ShouldCheckForInputs() == false)
                {
                    continue;
                }

                if ((*It)->IsInBounds(*this, InCursorLocation) == false)
                {
                    continue;
                }

                if (const LReply Reply { (*It)->OnKeyDownNoFocus(*this, Input) }; Reply.IsHandled())
                {
                    this->HandleReply(Context, Reply);
                    break;
                }

                continue;
            }
        }

        continue;
    }

    /* Check for key up events. */
    for (const LRawInput& Input : Context.GetLastFramePressedKeys())
    {
        if (Context.IsKeyUp(Input) == false)
        {
            continue;
        }

        if (this->FocusedWidget.IsNotNull())
        {
            if (const LReply Reply { this->FocusedWidget->OnKeyUp(*this, Input) }; Reply.IsHandled())
            {
                this->HandleReply(Context, Reply);
                continue;
            }
        }

        if (bCursorLocationIsMeaningful)
        {
            for (auto It{ this->TopLevelWidgets.rbegin() }; It != this->TopLevelWidgets.rend(); ++It)
            {
                if ((*It) == this->FocusedWidget || (*It)->ShouldCheckForInputs() == false)
                {
                    continue;
                }

                if ((*It)->IsInBounds(*this, InCursorLocation) == false)
                {
                    continue;
                }

                if (const LReply Reply { (*It)->OnKeyUpNoFocus(*this, Input) }; Reply.IsHandled())
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
    check( &this->Surface == &Context )

    if (this->HoveredWidgets.empty() == false || this->LastFrameHoveredWidgets.empty() == false)
    {
        LCursorReply MostRecentReply;
        for (auto& Node : this->HoveredWidgets)
        {
            if (LCursorReply Reply { Node->OnCursorLeave() }; Reply.IsHandled())
            {
                MostRecentReply = std::move(Reply);
            }
        }
        if (MostRecentReply.IsHandled())
        {
            if (MostRecentReply.GetCursorType() != EMouseCursor::None)
            {
                Context._SetMouseCursor(MostRecentReply.GetCursorType());
            }
        }

        algo::orphan(&this->HoveredWidgets);
        algo::orphan(&this->LastFrameHoveredWidgets);
    }

    if (bInvalidateAllInputs && this->FocusedWidget.IsNotNull())
    {
        LOG_VERBOSE(LogWidgetFramework, "Lost focus on [{}].", this->FocusedWidget->GetNameAsString())
        this->FocusedWidget->OnFocusLost();
        this->FocusedWidget = nullptr;
    }

    return;
}

void Jafg::LViewport::OnClear()
{
    // if
    // (
    //        this->BackgroundContexts.empty() == false
    //     && this->BackgroundContexts[0].World->IsSkyboxValid()
    // )
    // {
    //     this->IntermediateBuffer.MakeDrawTargetAndReset
    //     (
    //         this->BackgroundContexts[0].World->GetSkybox().GetBackgroundColor().ToLinearColor()
    //     );
    // }
    // else
    // {
    //     this->IntermediateBuffer.MakeDrawTargetAndReset(this->BackgroundColor);
    // }

    return;
}

void Jafg::LViewport::Tick()
{
    STAT_CYCLE_FUNCTION()

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

void Jafg::LViewport::Draw(LRenderInfo const& Info)
{
    STAT_CYCLE_FUNCTION()

    this->ClearInvalidWidgets();

    this->FrameZLayerDepth = 0.0f;
    this->FrameTranslation = LVector2D::ZeroVector;
    this->RecalculateScaleFactor();

    // this->IntermediateBuffer.MakeDrawTarget();

    const auto Dimensions{ this->GetDimensions() };
    this->CachedOrthographicProjectionMatrix = Maths::MakeOrthographicProjectionMatrix
    (
        LVector2(static_cast<f32>(Dimensions.X), static_cast<f32>(Dimensions.Y))
    );

    // RendererStateMachine::PrepareForPerspectivePainting();
    for (auto const& [Eye, World] : this->PerspectiveViews)
    {
        Eye->UpdateViewMatrix();

        // for (LEngineShader* Shader : GEngine->GetShaders() | std::views::values)
        // {
        //     checkSlow( Shader && Shader->IsValid() )
        //     Shader->UpdateWorldUniforms(*this, *World, *Eye);
        //     continue;
        // }

        // Info.Surface.GetDimensions()

        World->Draw(Info, *Eye);

        continue;
    }

    // RendererStateMachine::PrepareForOrthographicPainting();
    // for (LEngineShader* Shader : GEngine->GetShaders() | std::views::values)
    // {
    //     checkSlow( Shader && Shader->IsValid() )
    //     Shader->UpdateViewportUniforms(*this);
    //     continue;
    // }

    for (const WUserWidget* Widget : this->TopLevelWidgets)
    {
        if (Widget->TransformsWidgetLayout())
        {
            Widget->UpdateDesiredSize();
            Widget->UpdateAnchoredSize(*this);
            if (Widget->ShouldNowDraw())
            {
                STAT_QUICK_CYCLE_START(Widget->GetNameAsString())
                if (Widget->IsA<WTestWidget>())
                {
                    Widget->Draw(*this);
                }
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

    // LFrameBuffer::MakeDefaultDrawTarget();
    // this->IntermediateBuffer.PaintToViewport(*this);

    return;
}

void Jafg::LViewport::TearDown()
{
    for (WUserWidget* Widget : this->TopLevelWidgets)
    {
        Widget->MarkAsGarbage_v2();
    }

    algo::orphan(&this->TopLevelWidgets);

    return;
}

void Jafg::LViewport::AddWidget(WUserWidget* Widget)
{
    check( Widget )
    this->TopLevelWidgets.push_back(Widget);

    check( &Widget->GetViewport() == this )

    return;
}

void Jafg::LViewport::AddWidgetAt(const i32 Index, WUserWidget* Widget)
{
    check( Widget )
    this->TopLevelWidgets.insert(this->TopLevelWidgets.begin() + Index, Widget);

    check( &Widget->GetViewport() == this )

    return;
}

void Jafg::LViewport::RemoveWidget(WUserWidget* Widget)
{
    algo::erase_once_checked(&this->TopLevelWidgets, Widget);
}

bool Jafg::LViewport::TryRemoveWidget(WUserWidget* Widget)
{
    return algo::erase_once(&this->TopLevelWidgets, Widget);
}

LUIntVector2 Jafg::LViewport::GetDimensions() const noexcept
{
    return this->Surface.GetDimensions();
}

// void Jafg::LViewport::ChangeDimensions(const LIntVector2& InDimensions)
// {
//     check( GEngine )
//     check( InDimensions.X > 0 && InDimensions.Y > 0 )
//
//     this->Dimensions = InDimensions;
//
//     if (this->IntermediateBuffer.IsValid())
//     {
//         this->IntermediateBuffer = { };
//     }
//
//     this->IntermediateBuffer.Build(this->GetDimensions());
//
//     return;
// }

Jafg::WNode* Jafg::LViewport::GetTopLevelWidgetByClass(TSubclassOf<WNode> Class) const
{
    for (WUserWidget* Widget : this->TopLevelWidgets)
    {
        if (Widget->GetVirtualTable().DerivesFrom(Class))
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
    check( algo::contains(this->HoveredWidgets, Node) == false )
    this->HoveredWidgets.emplace_back(Node);
    return algo::contains(this->LastFrameHoveredWidgets, Node) == false;
}

void Jafg::LViewport::ChangeFocusUnsafe(WNode* InNode)
{
    if (this->FocusedWidget.IsNotNull())
    {
        LOG_VERBOSE(LogWidgetFramework, "Lost focus on [{}].", this->FocusedWidget->GetNameAsString())
        this->FocusedWidget->OnFocusLost();
    }

    this->FocusedWidget = InNode;

    if (this->FocusedWidget.IsNotNull())
    {
        LOG_VERBOSE(LogWidgetFramework, "Gained focus on [{}].", this->FocusedWidget->GetNameAsString())
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
        Context._SetMouseCursor(Reply.GetCursorType());
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
