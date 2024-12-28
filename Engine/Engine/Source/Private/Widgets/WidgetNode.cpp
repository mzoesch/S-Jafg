// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/WidgetNode.h"
#include "Engine/Engine.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetParent.h"

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
        && this->Anchors.X <= 1.0f && this->Anchors.Y <= 1.0f && this->Anchors.Z <= 1.0f && this->Anchors.W <= 1.0f;
}

void LAnchor::Normalize()
{
    this->Anchors.X = Maths::Clamp(this->Anchors.X, 0.0f, 1.0f);
    this->Anchors.Y = Maths::Clamp(this->Anchors.Y, 0.0f, 1.0f);
    this->Anchors.Z = Maths::Clamp(this->Anchors.Z, 0.0f, 1.0f);
    this->Anchors.W = Maths::Clamp(this->Anchors.W, 0.0f, 1.0f);

    return;
}

LSimpleString LexToString(const EWidgetVisibility::Type InVisibility)
{
    switch (InVisibility)
    {
    case EWidgetVisibility::Visible: { return "Visible"; }
    case EWidgetVisibility::Hidden: { return "Hidden"; }
    case EWidgetVisibility::Collapsed: { return "Collapsed"; }
    case EWidgetVisibility::TransitiveHitTestInvisible: { return "TransitiveHitTestInvisible"; }
    case EWidgetVisibility::IntransitiveHitTestInvisible: { return "IntransitiveHitTestInvisible"; }
    default: checkNoEntry() return { };
    }
}

ENGINE_API Private::LObjectContext* GCurrentWidgetContextState = nullptr;

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
    const LVector2 TopLeftMostOuter = this->GetAnchoredTopLeftFromMostOuter(Context, this);
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
    if (this->IsHitTestable() == false)
    {
        return LReply::HandledWithFocusLost();
    }

    if (this->IsInBounds(Context, InLocation) == false)
    {
        return LReply::Unhandled();
    }

    return { this };
}

bool Jafg::WWidgetNode::IsFocusWidget() const
{
    return this->GetViewport()->GetFocusedWidget() == this;
}

bool Jafg::WWidgetNode::IsFocusWidgetTransitive() const
{
    return this->GetViewport()->GetFocusedWidget() == this;
}

bool Jafg::WWidgetNode::IsFocusWidgetTransitive(const LViewport* InViewport) const
{
    return InViewport->GetFocusedWidget() == this;
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
    if (this->Slot)
    {
        check( this->Slot->Parent )
        return this->Slot->Parent->GetViewportSize();
    }

    panic( "Failed to find window dimensions." )

    return LIntVector2::ZeroVector;
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

Jafg::LViewport* Jafg::WWidgetNode::GetCheckedViewport() const
{
    LViewport* Viewport = this->GetViewport();
    check( Viewport )
    return Viewport;
}

Jafg::LViewport* Jafg::WWidgetNode::GetPanickedViewport() const
{
    if (LViewport* Viewport = this->GetViewport(); Viewport)
    {
        return Viewport;
    }

    panic ( "Failed to find viewport." )

    return nullptr;
}

Jafg::LVector2 Jafg::WWidgetNode::GetRelativeTopLeftFromOuter() const
{
    if (this->Slot)
    {
        return this->Slot->Margin->GetTopLeftOffset();
    }

    return LVector2::Zero();
}

Jafg::LVector2 Jafg::WWidgetNode::GetRelativeTopLeftFromMostOuter(const WWidgetNode* WhoAsked) const
{
    if (this->Slot)
    {
        return this->Slot->Parent->GetRelativeTopLeftFromMostOuter(this) + this->GetRelativeTopLeftFromOuter();
    }

    return this->GetRelativeTopLeftFromOuter();
}

Jafg::LVector2 Jafg::WWidgetNode::GetAnchoredTopLeftFromMostOuter(const LViewport& Context, const WWidgetNode* WhoAsked) const
{
    LVector2 Out;

    if (this->Slot)
    {
        const LVector2 ThisAnchoredSize = this->GetAnchoredSize();
        const LVector2 ParentAnchorSize = this->Slot->Parent->GetAnchoredSize();
        Out = Slot->Parent->GetAnchoredTopLeftFromMostOuter(Context, this);
        Out += (-LVector2(this->Anchor.MinX, this->Anchor.MinY) + 1.0f) * this->Slot->Margin->GetTopLeftOffset();
        Out.X += this->Anchor.MinX * (ParentAnchorSize.X - ThisAnchoredSize.X - this->Slot->Margin->Left);
        Out.Y += this->Anchor.MinY * (ParentAnchorSize.Y - ThisAnchoredSize.Y - this->Slot->Margin->Top);
    }
    else
    {
        const LVector2 TopLeftMostOuter = this->GetRelativeTopLeftFromMostOuter(WhoAsked);
        Out = TopLeftMostOuter;
        Out.X +=
            this->Anchor.MinX
            * (static_cast<float>(Context.GetDimensions().X) - (this->DesiredSize.X + TopLeftMostOuter.X));
        Out.Y +=
            this->Anchor.MinY
            * (static_cast<float>(Context.GetDimensions().Y) - (this->DesiredSize.Y + TopLeftMostOuter.Y));
    }

    return Out;
}

void Jafg::WWidgetNode::UpdateAnchoredSize(const LViewport& Context) const
{
    LVector2 Out = this->GetDesiredSize();

    if (this->Slot)
    {
        const LVector2 ParentAnchorSize = this->Slot->Parent->GetAnchoredSize();

        Out.X +=
            this->Anchor.MaxX
            * (ParentAnchorSize.X - (this->GetRelativeTopLeftFromOuter().X + this->GetDesiredSize().X));
        if (Out.X + this->Slot->Margin->Left + this->Slot->Margin->Right > ParentAnchorSize.X)
        {
            Out.X = ParentAnchorSize.X - this->Slot->Margin->Left - this->Slot->Margin->Right;
        }

        Out.Y +=
            this->Anchor.MaxY
            * (ParentAnchorSize.Y - (this->GetRelativeTopLeftFromOuter().Y + this->GetDesiredSize().Y));
        if (Out.Y + this->Slot->Margin->Top + this->Slot->Margin->Bottom > ParentAnchorSize.Y)
        {
            Out.Y = ParentAnchorSize.Y - this->Slot->Margin->Top - this->Slot->Margin->Bottom;
        }
    }
    else
    {
        Out.X +=
            this->Anchor.MaxX
            * static_cast<float>(Context.GetDimensions().X) - (this->GetRelativeTopLeftFromOuter().X + this->GetDesiredSize().X);

        Out.Y +=
            this->Anchor.MaxY
            * static_cast<float>(Context.GetDimensions().Y) - (this->GetRelativeTopLeftFromOuter().Y + this->GetDesiredSize().Y);
    }

    this->AnchoredSize = Out;

    return;
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
