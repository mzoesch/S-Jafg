// Copyright mzoesch. All rights reserved.

#include "Svw/SvwFrontendSubsystem.h"
#include "Core/JgcNames.h"

bool Jgc::JSvwFrontendSubsystem::ShouldCreateSubsystem(Jafg::LClassOuter const* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return Outer && Outer->IsWorld() && Outer->AsWorld()->GetUnderlyingLevelName() == Name_LevelSvw.ToString();
}

void Jgc::JSvwFrontendSubsystem::Initialize(Jafg::LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    Jafg::LViewport* TargetViewport{ nullptr };

    if (auto* Pc{ this->GetWorldChecked()->GetThisWorldsLocalPersonaControllerSlow() })
    {
        if (auto* Surface{ Pc->GetSurface() })
        {
            TargetViewport = &Surface->GetViewport();
        }
    }

    if (TargetViewport == nullptr)
    {
        return;
    }

    LOG_VERBOSE(LogFrontEnd, "Creating SVW core widgets.");

    this->Crosshair = Jafg::ConstructWidgetNode<Jafg::WCrosshair>(TargetViewport, this->GetOuter(), this->CrosshairClass);
    this->PauseScreen = Jafg::ConstructWidgetNode<Jafg::WPauseScreen>(TargetViewport, this->GetOuter(), this->PauseScreenClass);

    return;
}
