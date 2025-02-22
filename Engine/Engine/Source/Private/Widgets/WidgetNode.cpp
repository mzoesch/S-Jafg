// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/WidgetNode.h"
#include "Engine/Engine.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetParent.h"

#include "Widgets/WidgetRegion.h"

namespace
{

std::map<void*, Jafg::LWidgetFactory*> GWidgetFactories;

} /* ~Namespace <Anonymous> */

namespace Jafg::Private
{

ENGINE_API void AddWidgetFactory(LWidgetFactory* InFactory)
{
#if DO_CHECKS
    check( InFactory->GetNodeRaw() )
    const std::map<void*, Jafg::LWidgetFactory*>::iterator It = GWidgetFactories.find(InFactory->GetNodeRaw());
    if (It != GWidgetFactories.end())
    {
        panic( "The widget factory is already registered." )
    }
#endif /* DO_CHECKS */

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

ENGINE_API int32 PurgeWidgetFactories()
{
    int32 Count = 0;
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

ENGINE_API const LAnchor LAnchor::VTop           = { 0.0f, 0.0f, 0.0f, 0.0f };
ENGINE_API const LAnchor LAnchor::VCenter        = { 0.0f, 0.5f, 0.0f, 0.0f };
ENGINE_API const LAnchor LAnchor::VBottom        = { 0.0f, 1.0f, 0.0f, 0.0f };
ENGINE_API const LAnchor LAnchor::HLeft          = { 0.0f, 0.0f, 0.0f, 0.0f };
ENGINE_API const LAnchor LAnchor::HCenter        = { 0.5f, 0.0f, 0.0f, 0.0f };
ENGINE_API const LAnchor LAnchor::HRight         = { 1.0f, 0.0f, 0.0f, 0.0f };
ENGINE_API const LAnchor LAnchor::VFill          = { 0.0f, 0.0f, 0.0f, 1.0f };
ENGINE_API const LAnchor LAnchor::HFill          = { 0.0f, 0.0f, 1.0f, 0.0f };

bool LAnchor::IsNormalized() const
{
    return
           this->Anchors.X >= 0.0f && this->Anchors.Y >= 0.0f && this->Anchors.Z >= 0.0f && this->Anchors.W >= 0.0f
        && this->Anchors.X <= 1.0f && this->Anchors.Y <= 1.0f && this->Anchors.Z <= 1.0f && this->Anchors.W <= 1.0f
        && this->MinX + this->MaxX <= 1.0f
        && this->MinY + this->MaxY <= 1.0f;
}

void LAnchor::Normalize()
{
#if DO_CHECKS
    LAnchor Old = *this;
#endif /* DO_CHECKS */

    this->Anchors.X = Maths::Clamp(this->Anchors.X, 0.0f, 1.0f);
    this->Anchors.Y = Maths::Clamp(this->Anchors.Y, 0.0f, 1.0f);
    this->Anchors.Z = Maths::Clamp(this->Anchors.Z, 0.0f, 1.0f);
    this->Anchors.W = Maths::Clamp(this->Anchors.W, 0.0f, 1.0f);

    this->MaxX = Maths::Min(this->MaxX, 1.0f - this->MinX);
    this->MaxY = Maths::Min(this->MaxY, 1.0f - this->MinY);

#if DO_CHECKS
    if (Old != *this)
    {
        LOG_WARNING(LogWidgetFramework, "Anchor was not normalized correctly: {} -> {}.", Old.ToString(), this->ToString())
    }
#endif /* DO_CHECKS */

    return;
}

LSimpleString LexToString(const EWidgetVisibility::Type InVisibility)
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

ENGINE_API LObjectContext* GCurrentWidgetContextState = nullptr;

struct LWidgetConstructor
{
};

} /* ~Namespace Jafg */

void Jafg::WWidgetNode::OnGarbage()
{
    Super::OnGarbage();

    this->Destruct();

    return;
}

bool Jafg::WWidgetNode::IsInBounds(const LViewport& Context, const LVector2& InLocation) const
{
    const LVector2 TopLeftMostOuter = this->GetAnchoredTopLeftFromMostOuter(Context);
    return
            TopLeftMostOuter.X <= InLocation.X
         && InLocation.X       <= TopLeftMostOuter.X + this->GetAnchoredSize().X
         && TopLeftMostOuter.Y <= InLocation.Y
         && InLocation.Y       <= TopLeftMostOuter.Y + this->GetAnchoredSize().Y
         ;
}

Jafg::LCursorReply Jafg::WWidgetNode::SweepMouse(LViewport& Context, const LVector2& InLocation)
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
        LCursorReply Reply = this->OnCursorEnter();
        if (Reply.IsHandled())
        {
            return Reply;
        }
    }

    return this->OnCursorMoved(InLocation);
}

Jafg::LReply Jafg::WWidgetNode::SweepFocusTest(LViewport& Context, const LVector2& InLocation)
{
    if (this->IsInBounds(Context, InLocation) == false || this->IsHitTestable() == false)
    {
        return LReply::Unhandled();
    }

    return { this };
}

bool Jafg::WWidgetNode::IsFocusWidget() const
{
    if (const LViewport* Viewport = this->GetViewport(); ensure(Viewport))
    {
        return Viewport->GetFocusedWidget() == this;
    }

    return false;
}

bool Jafg::WWidgetNode::IsFocusWidgetTransitive() const
{
    return this->IsFocusWidgetTransitive(this->GetViewport());
}

bool Jafg::WWidgetNode::IsFocusWidgetTransitive(const LViewport* InViewport) const
{
    if (InViewport)
    {
        return InViewport->GetFocusedWidget() == this;
    }

    return false;
}

void Jafg::WWidgetNode::SetVisibility(const EWidgetVisibility::Type InVisibility)
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

void Jafg::WWidgetNode::RemoveFromParent(const bool bDestroy /* = true */)
{
    if (this->Slot)
    {
        check( this->Slot->Parent )
        this->Slot->Parent->RemoveChild(this);
        this->Slot = nullptr;
    }
    if (bDestroy)
    {
        this->MarkAsGarbage();
    }

    return;
}

auto Jafg::WWidgetNode::GetParent() const -> WWidgetParentBase*
{
    if (this->Slot)
    {
        return this->Slot->Parent;
    }

    return nullptr;
}

bool Jafg::WWidgetNode::FindNodeInVisiblePath(const WWidgetNode* InNode) const
{
    return this == InNode && this->ShouldNowDraw();
}

Jafg::LIntVector2 Jafg::WWidgetNode::GetViewportSize() const
{
    return this->GetViewportChecked()->GetDimensions();
}

Jafg::LViewport* Jafg::WWidgetNode::GetViewport() const
{
    if (this->Slot)
    {
        check( this->Slot->Parent )
        return this->Slot->Parent->GetViewport();
    }

    return nullptr;
}

void Jafg::WWidgetNode::SetDesiredSize(const LVector2& InSize) const
{
    this->DesiredSize = InSize;
    this->DesiredSize.X = Maths::Max(this->DesiredSize.X, this->MinDesiredSize.X);
    this->DesiredSize.Y = Maths::Max(this->DesiredSize.Y, this->MinDesiredSize.Y);

    check( this->DesiredSize.X >= 0.0f && this->DesiredSize.Y >= 0.0f )

    return;
}

Jafg::LVector2 Jafg::WWidgetNode::GetRelativeTopLeftFromOuter() const
{
    if (this->Slot)
    {
        return this->Slot->Parent->GetRelativeTopLeftForChild(this);
    }

    return LVector2::Zero();
}

void Jafg::WWidgetNode::UpdateAnchoredSize(const LViewport& Context) const
{
    check( this->TransformsWidgetLayout() )
    check( this->Anchor.IsNormalized() )

    if (this->Slot)
    {
        this->Slot->Parent->UpdateAnchoredSizeForChild(Context, this);
        return;
    }

    LVector2 Out;
    Out.X = Maths::Max(this->Anchor.MaxX * static_cast<float>(Context.GetDimensions().X), this->DesiredSize.X);
    Out.Y = Maths::Max(this->Anchor.MaxY * static_cast<float>(Context.GetDimensions().Y), this->DesiredSize.Y);
    this->SetAnchoredSize(Out);

    return;
}

Jafg::LVector2 Jafg::WWidgetNode::GetAnchoredTopLeftFromMostOuter(const LViewport& Context) const
{
    if (this->Slot)
    {
        return this->Slot->Parent->GetAnchoredTopLeftFromMostOuterForChild(Context, this);
    }

    LVector2 Out;
    Out.X = this->Anchor.MinX * (static_cast<float>(Context.GetDimensions().X) * this->DesiredSize.X);
    Out.Y = this->Anchor.MinY * (static_cast<float>(Context.GetDimensions().Y) * this->DesiredSize.Y);

    return Out;
}

Jafg::LApplicationInstance* Jafg::WWidgetNode::GetApplicationInstance() const
{
    checkSlow( GEngine )
    return GEngine->GetApplicationInstance();
}

Jafg::LEngine* Jafg::WWidgetNode::GetEngine() const
{
    return GEngine;
}

Jafg::LLocalEgo* Jafg::WWidgetNode::GetLocalEgo() const
{
    return GEngine->GetLocalEgo();
}
