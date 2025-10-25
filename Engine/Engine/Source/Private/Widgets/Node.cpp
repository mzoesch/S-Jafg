// Copyright mzoesch. All rights reserved.

#include "Widgets/Node.h"
#include "Engine/Engine.h"
#include "Widgets/Viewport.h"
#include "Widgets/Parent.h"
#include "Widgets/Region.h"
#include "User/UserPreferences.h"

namespace
{

std::map<void*, Jafg::LWidgetFactory*> GWidgetFactories;

} /* ~Namespace <Anonymous> */

namespace Jafg::Private
{

ENGINE_API void AddWidgetFactory(LWidgetFactory* InFactory)
{
#if LAL_DO_CHECKS
    check( InFactory->GetNodeRaw() )
    const std::map<void*, Jafg::LWidgetFactory*>::iterator It = GWidgetFactories.find(InFactory->GetNodeRaw());
    if (It != GWidgetFactories.end())
    {
        panic( "The widget factory is already registered." )
    }
#endif /* LAL_DO_CHECKS */

    GWidgetFactories[InFactory->GetNodeRaw()] = InFactory;

    return;
}

ENGINE_API LWidgetFactory* FindOrNullWidgetFactory(const void* InNode)
{
    const std::map<void*, Jafg::LWidgetFactory*>::iterator It = GWidgetFactories.find(const_cast<void*>(InNode));
    if (It != GWidgetFactories.end())
    {
        return It->second;
    }

    return nullptr;
}

ENGINE_API LWidgetFactory& GetWidgetFactory(const void* InNode)
{
    const std::map<void*, Jafg::LWidgetFactory*>::iterator It = GWidgetFactories.find(const_cast<void*>(InNode));
    if (It != GWidgetFactories.end())
    {
        check( It->second )
        return *It->second;
    }

    panic( "The widget factory is not registered." )
    abort();
}

ENGINE_API i32 PurgeWidgetFactories()
{
    i32 Count = 0;
    for (const auto& [fst, snd] : GWidgetFactories)
    {
        delete snd;
        ++Count;
    }

    GWidgetFactories.clear();

    return Count;
}

} /* ~Namespace Jafg::Private */

namespace Jafg
{

ENGINE_API const LAnchor LAnchor::VTop    { 0.0f, 0.0f, 0.0f, 0.0f };
ENGINE_API const LAnchor LAnchor::VCenter { 0.0f, 0.5f, 0.0f, 0.0f };
ENGINE_API const LAnchor LAnchor::VBottom { 0.0f, 1.0f, 0.0f, 0.0f };
ENGINE_API const LAnchor LAnchor::HLeft   { 0.0f, 0.0f, 0.0f, 0.0f };
ENGINE_API const LAnchor LAnchor::HCenter { 0.5f, 0.0f, 0.0f, 0.0f };
ENGINE_API const LAnchor LAnchor::HRight  { 1.0f, 0.0f, 0.0f, 0.0f };
ENGINE_API const LAnchor LAnchor::VFill   { 0.0f, 0.0f, 0.0f, 1.0f };
ENGINE_API const LAnchor LAnchor::HFill   { 0.0f, 0.0f, 1.0f, 0.0f };

bool LAnchor::IsNormalized() const noexcept
{
    return
           this->Anchors.X >= 0.0f && this->Anchors.Y >= 0.0f && this->Anchors.Z >= 0.0f && this->Anchors.W >= 0.0f
        && this->Anchors.X <= 1.0f && this->Anchors.Y <= 1.0f && this->Anchors.Z <= 1.0f && this->Anchors.W <= 1.0f
        && this->MinX + this->MaxX <= 1.0f
        && this->MinY + this->MaxY <= 1.0f;
}

void LAnchor::Normalize() noexcept
{
#if LAL_DO_CHECKS
    LAnchor Old = *this;
#endif /* LAL_DO_CHECKS */

    this->Anchors.X = Maths::Clamp(this->Anchors.X, 0.0f, 1.0f);
    this->Anchors.Y = Maths::Clamp(this->Anchors.Y, 0.0f, 1.0f);
    this->Anchors.Z = Maths::Clamp(this->Anchors.Z, 0.0f, 1.0f);
    this->Anchors.W = Maths::Clamp(this->Anchors.W, 0.0f, 1.0f);

    this->MaxX = Maths::Min(this->MaxX, 1.0f - this->MinX);
    this->MaxY = Maths::Min(this->MaxY, 1.0f - this->MinY);

#if LAL_DO_CHECKS
    if (Old != *this)
    {
        LOG_WARNING(LogWidgetFramework, "Anchor was not normalized correctly: {} -> {}.", Old.ToString(), this->ToString())
    }
#endif /* LAL_DO_CHECKS */

    return;
}

LString LexToString(const EWidgetVisibility::Type InVisibility)
{
    switch (InVisibility)
    {
    case EWidgetVisibility::Visible: { return "Visible"; }
    case EWidgetVisibility::Hidden: { return "Hidden"; }
    case EWidgetVisibility::Collapsed: { return "Collapsed"; }
    case EWidgetVisibility::DerivedHitTestInvisible: { return "DerivedHitTestInvisible"; }
    case EWidgetVisibility::TransitiveHitTestInvisible: { return "TransitiveHitTestInvisible"; }
    case EWidgetVisibility::IntransitiveHitTestInvisible: { return "IntransitiveHitTestInvisible"; }
    default: checkNoEntry() return { };
    }
}

struct LWidgetConstructor
{
};

} /* ~Namespace Jafg */

f32 Jafg::InSpt(LViewport const& Viewport, LWidgetSize1 Size) noexcept
{
    if (Size.Type == EWidgetSize::StaticPoints)
    {
        return Size.Size;
    }

    return InSptFromRelative(Viewport, Size.Size);
}

LVector2 Jafg::InSpt(LViewport const& Viewport, LWidgetSize2 Size) noexcept
{
    if (Size.Type == EWidgetSize::StaticPoints)
    {
        return Size.Size;
    }

    return InSptFromRelative(Viewport, Size.Size);
}

f32 Jafg::InSptFromRelative(LViewport const& Viewport, f32 Relative) noexcept
{
    const EApplicationScale::Type Scale{ Viewport.GetMaxAllowApplicationScale() };
    check( Scale != EApplicationScale::Auto )
    return Relative * LexToFloat(Scale);
}

LVector2 Jafg::InSptFromRelative(LViewport const& Viewport, LVector2 Relative) noexcept
{
    const EApplicationScale::Type Scale{ Viewport.GetMaxAllowApplicationScale() };
    check( Scale != EApplicationScale::Auto )
    return Relative * LexToFloat(Scale);
}

Jafg::WNode::~WNode()
{
    check( this->Slot.Parent == nullptr && this->Slot.Content == nullptr && this->Slot.Margin == nullptr )
}

void Jafg::WNode::OnGarbage(ECxxRecordTearDownReason::Type Reason)
{
    Super::OnGarbage(Reason);

    this->Destruct();

    return;
}

bool Jafg::WNode::IsInBounds(const LViewport& Context, const LVector2& InLocation) const
{
    if (this->TransformsWidgetLayout() == false)
    {
        return false;
    }

    const LVector2 TopLeftMostOuter = this->GetAnchoredTopLeftFromMostOuter(Context) + static_cast<LVector2>(Context.GetSweepTranslation());
    return
            TopLeftMostOuter.X <= InLocation.X
         && InLocation.X       <= TopLeftMostOuter.X + this->GetAnchoredSize_v2().X
         && TopLeftMostOuter.Y <= InLocation.Y
         && InLocation.Y       <= TopLeftMostOuter.Y + this->GetAnchoredSize_v2().Y
         ;
}

Jafg::LCursorReply Jafg::WNode::SweepMouse(LViewport& Context, const LVector2& InLocation)
{
    if (this->IsHitTestable() == false)
    {
        return LCursorReply::Unhandled();
    }

    if (this->IsInBounds(Context, InLocation) == false)
    {
        return LCursorReply::Unhandled();
    }

    if (Context.AddHoveredWidgetForFrame(this))
    {
        if (LCursorReply Reply{ this->OnCursorEnter() }; Reply.IsHandled())
        {
            return Reply;
        }

        return LCursorReply::Handled();
    }

    return this->OnCursorMoved(InLocation);
}

Jafg::LReply Jafg::WNode::SweepFocusTest(const LViewport& Context, const LVector2& InLocation)
{
    if (this->IsInBounds(Context, InLocation) == false || this->IsHitTestable() == false)
    {
        return LReply::Unhandled();
    }

    return { this };
}

Jafg::LReply Jafg::WNode::OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    if (this->OnKeyDownEvent.IsBound())
    {
        return this->OnKeyDownEvent.Invoke(*this, InViewport, InKeyEvent);
    }

    if (this->Slot.Parent)
    {
        return this->Slot.Parent->OnKeyDown(InViewport, InKeyEvent);
    }

    return LReply::Unhandled();
}

Jafg::LReply Jafg::WNode::OnKeyUp(LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    if (this->OnKeyUpEvent.IsBound())
    {
        return this->OnKeyUpEvent.Invoke(*this, InViewport, InKeyEvent);
    }

    if (this->Slot.Parent)
    {
        return this->Slot.Parent->OnKeyUp(InViewport, InKeyEvent);
    }

    return LReply::Unhandled();
}

Jafg::LReply Jafg::WNode::OnKeyDownNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    check( this->IsInBounds(InViewport, *InViewport.GetCachedCursorLocationChecked() ) )
    return LReply::Unhandled();
}

Jafg::LReply Jafg::WNode::OnKeyUpNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    check( this->IsInBounds(InViewport, *InViewport.GetCachedCursorLocationChecked() ) )
    return LReply::Unhandled();
}

bool Jafg::WNode::IsFocusWidget() const
{
    return this->IsFocusWidget(&this->GetViewport());
}

bool Jafg::WNode::IsFocusWidget(const LViewport* InViewport) const
{
    if (InViewport)
    {
        return InViewport->GetFocusedWidget() == this;
    }

    return false;
}

bool Jafg::WNode::IsFocusWidgetTransitive() const
{
    return this->IsFocusWidgetTransitive(&this->GetViewport());
}

bool Jafg::WNode::IsFocusWidgetTransitive(const LViewport* InViewport) const
{
    if (InViewport)
    {
        return InViewport->GetFocusedWidget() == this;
    }

    return false;
}

void Jafg::WNode::SetVisibility(const EWidgetVisibility::Type InVisibility)
{
    if (this->Visibility == InVisibility)
    {
        return;
    }

    const EWidgetVisibility::Type OldVisibility = this->Visibility;
    this->Visibility = InVisibility;
    this->OnVisibilityChanged(OldVisibility, InVisibility);

    return;
}

void Jafg::WNode::RemoveFromParent(const bool bDestroy /* = true */)
{
    if (this->Slot.Parent)
    {
        check( this->Slot.Content == this )
        this->Slot.Parent->RemoveChild(this);
        check( this->Slot.Parent == nullptr && this->Slot.Content == nullptr && this->Slot.Margin == nullptr )
    }

    if (bDestroy)
    {
        this->MarkAsGarbage_v2();
    }

    return;
}

Jafg::WNode* Jafg::WNode::GetMostOuterParent()
{
    if (this->Slot.Parent)
    {
        return this->Slot.Parent->GetMostOuterParent();
    }

    return this;
}

const Jafg::WNode* Jafg::WNode::GetMostOuterParent() const
{
    if (this->Slot.Parent)
    {
        return this->Slot.Parent->GetMostOuterParent();
    }

    return this;
}

bool Jafg::WNode::FindNodeInVisiblePath(const WNode* InNode) const
{
    return this == InNode && this->ShouldNowDraw();
}

LIntVector2 Jafg::WNode::GetViewportSize() const
{
    return this->GetViewport().GetDimensions();
}

void Jafg::WNode::RecacheViewport() noexcept
{
    this->CachedViewport = this->GetMostOuterViewport();
}

Jafg::LViewport* Jafg::WNode::GetMostOuterViewport() noexcept
{
    if (this->Slot.Parent)
    {
        return this->Slot.Parent->GetMostOuterViewport();
    }

    return nullptr;
}

void Jafg::WNode::SetDesiredSizeInSpt(LVector2 Size) const noexcept
{
    this->DesiredSize_v2 = Size;

    LVector2 SptMinSize{InSpt(this->GetViewport(), this->MinDesiredSize)};
    LVector2 SptMaxSize{InSpt(this->GetViewport(), this->MaxDesiredSize)};

    this->DesiredSize_v2.X = Maths::Max(this->DesiredSize_v2.X, SptMinSize.X);
    this->DesiredSize_v2.Y = Maths::Max(this->DesiredSize_v2.Y, SptMinSize.Y);

    if (SptMaxSize.X > 0.0f)
    {
        this->DesiredSize_v2.X = Maths::Min(this->DesiredSize_v2.X, SptMaxSize.X);
    }
    if (SptMaxSize.Y > 0.0f)
    {
        this->DesiredSize_v2.Y = Maths::Min(this->DesiredSize_v2.Y, SptMaxSize.Y);
    }

    check( this->DesiredSize_v2.X >= 0.0f && this->DesiredSize_v2.Y >= 0.0f )

    return;
}

void Jafg::WNode::UpdateAnchoredSize(const LViewport& Context) const
{
    check( this->TransformsWidgetLayout() )
    check( this->Anchor.IsNormalized() )

    if (this->Slot.Parent)
    {
        check( this->Slot.Content == this )
        this->Slot.Parent->UpdateAnchoredSizeForChild(Context, this);
        return;
    }

    LVector2 Out;
    Out.X = Maths::Max(this->Anchor.MaxX * static_cast<f32>(Context.GetDimensions().X), this->DesiredSize_v2.X);
    Out.Y = Maths::Max(this->Anchor.MaxY * static_cast<f32>(Context.GetDimensions().Y), this->DesiredSize_v2.Y);
    this->SetAnchoredSize(Out);

    return;
}

void Jafg::WNode::SetAnchoredSize(LVector2&& InSize) const noexcept
{
    this->LostAnchoredSize_v2 = LVector2::ZeroVector;
    this->AnchoredSize_v2 = std::move(InSize);

    if (this->MaxDesiredSize.X > 0.0f)
    {
        this->LostAnchoredSize_v2.X = Maths::Max(this->AnchoredSize_v2.X - this->MaxDesiredSize.X, 0.0f);
        this->AnchoredSize_v2.X = Maths::Min(this->AnchoredSize_v2.X, this->MaxDesiredSize.X);
    }
    if (this->MaxDesiredSize.Y > 0.0f)
    {
        this->LostAnchoredSize_v2.Y = Maths::Max(this->AnchoredSize_v2.Y - this->MaxDesiredSize.Y, 0.0f);
        this->AnchoredSize_v2.Y = Maths::Min(this->AnchoredSize_v2.Y, this->MaxDesiredSize.Y);
    }

    return;
}

LVector2 Jafg::WNode::GetAnchoredTopLeftFromMostOuter(const LViewport& Context) const
{
    check( this->TransformsWidgetLayout() )
    check( this->Anchor.IsNormalized() )

    if (this->Slot.Parent)
    {
        check( this->Slot.Content == this )
        return this->Slot.Parent->GetAnchoredTopLeftFromMostOuterForChild(Context, this);
    }

    LVector2 Out;
    Out.X = this->Anchor.MinX * static_cast<f32>(Context.GetDimensions().X);
    Out.Y = this->Anchor.MinY * static_cast<f32>(Context.GetDimensions().Y);

    return Out;
}

LVector2 Jafg::WNode::GetAnchoredAndTranslatedTopLeftFromMostOuter(const LViewport& Context) const
{
    return this->GetAnchoredTopLeftFromMostOuter(Context) + static_cast<LVector2>(Context.GetFrameTranslation());
}

bool Jafg::WNode::SetMargin(const LMargin& InMargin) noexcept
{
    if (this->Slot.Margin)
    {
        check( this->Slot.Parent )
        *this->Slot.Margin = InMargin;
        return true;
    }

    return false;
}

void Jafg::WNode::ConstructInternal() noexcept
{

    this->Construct();
    return;
}

