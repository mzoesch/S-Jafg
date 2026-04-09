// Copyright mzoesch. All rights reserved.

#include "Nodes/TabOverlay.h"
#include "User/UserPreferences.h"
#include "Framework/Frontend.h"
#include "Framework/TextureSubsystem.h"

void Jafg::WTabOverlay::Construct()
{
    Super::Construct();

    if (this->Wrapper == nullptr)
    {
        this->InitializeBoilerplate();
    }
    check(this->Wrapper && this->Selectors && this->Switcher)

    return;
}

void Jafg::WTabOverlay::RegisterTab(LTabOverlayElement&& Descriptor)
{
    if (this->Wrapper == nullptr)
    {
        this->InitializeBoilerplate();
    }
    check(this->Wrapper && this->Selectors && this->Switcher)

    this->Switcher->AddChild(TJxxUnique<WNode>{&Descriptor.Panel.GetRawNode()});
    checkCode(Descriptor.Panel._Release())

    bool bActivated{};
    if (this->Switcher->GetChildren().size() == 1)
    {
        bActivated = true;
        this->Switcher->SetActiveNodeByIndex(0);
    }

    if (Descriptor.Selector.index() == 0)
    {
        BeginStyling(*this->Selectors)
            .StaticRoot<WTabOverlaySelector>(std::move(std::get<LTabOverlayElement::CreateInfo>(Descriptor.Selector)))
            .Delegate(this->DefaultSelectorDelegate);
        auto& Selector{*StaticCast<WTabOverlaySelector>(&*this->Selectors->GetChildren().back())};
        check(Selector.OnKeyDownEvent.IsValid() == false)
        Selector.OnKeyDownEvent.Bind([this, Panel = &Descriptor.Panel.GetRawNode()]
        (WNode& Self, LNodeKeyEventData const& Data, LKeyEvent const& Event) -> LReply
        {
            if (Event.PhysicalKey != LPhysicalKey::FromLogical(ENamedPhysicalKey::LeftMouseButton))
            {
                return LReply::Unhandled();
            }
            check(this->Switcher)
            check(IsValidFast(&this->GetOuter(), Panel))
            for (auto& Child : this->Selectors->GetChildren())
            {
                if (auto* Selector{Child->As<WTabOverlaySelector>()})
                {
                    Selector->SetSelected(false);
                }
            }
            this->Switcher->SetActiveNode(*Panel);
            auto& Selector{*StaticCast<WTabOverlaySelector>(&Self)};
            Selector.SetSelected(true);
            return LReply::Unhandled();
        });
        if (bActivated)
        {
            Selector.SetSelected(true);
        }
    }
    else
    {
        this->Selectors->AddChild(TJxxUnique<WNode>{&std::get<Detail::LNodeFactoryBase>(Descriptor.Selector).GetRawNode()});
        check(this->Selectors->GetChildren().back()->OnKeyDownEvent.IsValid() == false)
        this->Selectors->GetChildren().back()->OnKeyDownEvent.Bind([this, Panel = &Descriptor.Panel.GetRawNode()]
        (WNode& Self, LNodeKeyEventData const& Data, LKeyEvent const& Event) -> LReply
        {
            if (Event.PhysicalKey != LPhysicalKey::FromLogical(ENamedPhysicalKey::LeftMouseButton))
            {
                return LReply::Unhandled();
            }
            check(this->Switcher)
            check(IsValidFast(&this->GetOuter(), Panel))
            this->Switcher->SetActiveNode(*Panel);
            return LReply::Unhandled();
        });
    }

    checkCode(Descriptor.Panel._Decommission())

    for (LTabOverlayElement& Sibling : Descriptor._Siblings)
    {
        this->RegisterTab(std::move(Sibling));
    }

    return;
}

void Jafg::WTabOverlay::InitializeBoilerplate()
{
    check(this->Wrapper == nullptr)
    check(this->Selectors == nullptr)
    check(this->Switcher == nullptr)

    BeginStyling(*this).Root<WParent>(this->WrapperClass.GetClassOrDefault()).SaveTo(&this->Wrapper)
        .Anchor(EAnchor::Fill)
        .Delegate(this->WrapperDelegate)
    [
        NewSubNode(this->SelectorsClass).SaveTo(&this->Selectors)
            .Delegate(this->SelectorsDelegate)
        +
        NewSubNode(this->SwitcherClass).SaveTo(&this->Switcher)
            .Anchor(EAnchor::Fill)
            .Delegate(this->SwitcherDelegate)
    ];

    return;
}


void Jafg::WTabOverlaySelector::LoadIconFromTextureViewIdentifier(LString const& Identifier) noexcept
{
    this->SetIcon(this->GetFrontend().GetSubsystemChecked<JTextureSubsystem>()->FromTextureViewIdentifier(Identifier));
}
