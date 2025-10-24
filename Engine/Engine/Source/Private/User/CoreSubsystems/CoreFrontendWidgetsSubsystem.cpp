// Copyright mzoesch. All rights reserved.

#include "User/CoreSubsystems/CoreFrontendWidgetsSubsystem.h"
#include "Framework/Frontend.h"
#include "User/LocalEgo.h"
#include "User/Frontend/Osd/FrontendScreen.h"

bool Jafg::JCoreFrontendWidgetsSubsystem::ShouldCreateSubsystem(LClassOuter const* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return Super::IsOuterFrontend(Outer);
}

void Jafg::JCoreFrontendWidgetsSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    this->FrontendScreen = ConstructDeferredWidgetNode<WFrontendScreen>(this->GetOuterChecked());
    this->FrontendScreen->AddToViewport(&this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->GetViewport());
    MakeDeferredWidgetNodeFinal(this->FrontendScreen);

    // auto* Temp{ ConstructDeferredWidgetNode<WUserWidget>(this->GetOuterChecked()) };
    // Temp->AddChild(ConstructDeferredWidgetNode<WTextButton>(this->GetOuterChecked()));
    // Temp->GetChildren().back()->Content->AsChecked<WTextButton>()->SetContent(
    //     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    // Temp->GetChildren().back()->Content->AsChecked<WTextButton>()->SetOmniOutlineThickness(0);
    // Temp->GetChildren().back()->Content->AsChecked<WTextButton>()->SetHoverTint(Lal::LColor::Black);
    // Temp->GetChildren().back()->Content->AsChecked<WTextButton>()->SetAnchor(EAnchor::BottomRight);
    // Temp->GetChildren().back()->Content->AsChecked<WTextButton>()->SetPadding({});
    // Temp->AddToViewport(&this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->GetViewport());
    // MakeDeferredWidgetNodeFinal(Temp);
    // Temp->GetChildren().back()->Content->AsChecked<WTextButton>()->GetButtonTextChecked()->SetPadding({});
    // Temp->GetChildren().back()->Content->AsChecked<WTextButton>()->GetButtonTextChecked()->SetTextScale(LTextScale{0.6});
    // Temp->GetChildren().back()->Content->AsChecked<WTextButton>()->SetMarginChecked(LMargin{2});

    return;
}
