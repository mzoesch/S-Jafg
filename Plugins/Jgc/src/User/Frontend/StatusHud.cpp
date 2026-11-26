// Copyright mzoesch. All rights reserved.

#include "Jgc/User/Frontend/StatusHud.h"
#include "Framework/Lackey.h"
#include "Nodes/Text.h"
#include "Nodes/VParent.h"

Jgc::WStatusHud::~WStatusHud()
{
    if (this->OwnerSlot && *this->OwnerSlot == this)
    {
        *this->OwnerSlot = nullptr;
    }
}

void Jgc::WStatusHud::Construct()
{
    Super::Construct();

    /* Only shows something. Clicks go through to the world. */
    this->SetVisibility(Jafg::ENodeVisibility::TransitiveHitTestInvisible);
    this->Padding = 16_pt;

    Jafg::BeginStyling(*this).StaticRoot<Jafg::WVParent>().SaveTo(&this->List)
        .Anchor(Jafg::EAnchor::BottomRight)
        ;
}

std::size_t Jgc::WStatusHud::AddRow(LString Label)
{
    check(this->List)
    auto& Text{this->List->AddChild(Jafg::NewNode(this->GetViewport()).Class<Jafg::WText>()
        .Anchor(Jafg::EAnchor::HFill)
        .TextAlign(Jafg::ETextHAlign::Right)
        .TextScale(Jafg::ETextScale::SubHeader)
        .Unique()
        ).AsStatic<Jafg::WText>()};
    this->Labels.emplace_back(std::move(Label));
    this->Values.emplace_back(&Text);
    this->SetRow(this->Values.size() - 1uz, "", Colors::White);
    return this->Values.size() - 1uz;
}

void Jgc::WStatusHud::SetRow(std::size_t Row, LStringView Value, LColor const& Tint) noexcept
{
    check(Row < this->Values.size())
    auto& Text{*this->Values[Row]};
    Text.TextBrush.Tint = Tint;
    /* Rows like a speed are set every frame. Only lay them out again if they changed. */
    if (LString Content{algo::sprintf("{}  {}", this->Labels[Row], Value)}; Content != Text.GetContent())
    {
        Text.SetContent(std::move(Content));
    }
}

void Jgc::WStatusHud::SetToggle(std::size_t Row, bool bEngaged) noexcept
{
    this->SetRow(Row, bEngaged ? "ON" : "OFF", bEngaged ? Colors::LimeGreen : Colors::Gray);
}

Jgc::WStatusHud& Jgc::WStatusHud::Show(Jafg::LLocalLackey& Lackey, WStatusHud** Slot)
{
    check(Slot)
    Hide(Slot);

    /* Over the rendered world, so that it sits in the corner of the view and not of the window. */
    Jafg::LViewport* const WorldViewport{Lackey.GetWorldViewport()};
    WStatusHud& Result{*Jafg::ConstructWidget(Jafg::TNodeStaticInit<WStatusHud>{WorldViewport ? *WorldViewport : Lackey.Viewport})};
    Result.OwnerSlot = Slot;
    *Slot = &Result;
    return Result;
}

void Jgc::WStatusHud::Hide(WStatusHud** Slot) noexcept
{
    check(Slot)
    if (WStatusHud* Old{std::exchange(*Slot, nullptr)})
    {
        Old->OwnerSlot = nullptr;
        if (!Old->_IsGarbage())
        {
            Old->MarkAsGarbage_v2();
        }
    }
}
