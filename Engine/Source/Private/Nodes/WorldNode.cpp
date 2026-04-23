// Copyright mzoesch. All rights reserved.

#include "Nodes/WorldNode.h"
#include "User/LocalEgo.h"
#include "Framework/PersonaController.h"
#include "Framework/Pawn.h"
#include "Framework/Frontend.h"

void Jafg::WWorldNode::Tick()
{
    Super::Tick();

    auto& Frontend{this->GetFrontend()};
    if (this->Controller.get()
        && Frontend.IsFocusedSurfaceValid() && Frontend.GetFocusedSurfaceChecked() == &this->GetViewport().GetSurface())
    {
        this->UserInput.DispatchInputDelegates(*this->Controller);
    }

    return;
}

void Jafg::WWorldNode::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);

    if (this->GetOwnedPersonaController())
    {
        if (auto& Ctrl{*this->GetOwnedPersonaControllerChecked()}; Ctrl.GetOwnedPawn())
        {
            auto& Pawn{*Ctrl.GetOwnedPawnChecked()};
            Pawn.GetWorld().Draw(Info, Pawn.GetEye());
        }
    }

    return;
}

void Jafg::WWorldNode::PossessPersonaController(TJxxUnique<APersonaController> New, bool bReleaseOld)
{
    if (this->IsOwnedPersonaControllerValid())
    {
        this->Controller->Node = nullptr;
        if (bReleaseOld)
        {
            this->Controller.release();
        }
    }

    this->Controller = std::move(New);
    if (this->IsOwnedPersonaControllerValid())
    {
        this->Controller->Node = this;
        // TODO: Move this when focusing the WWolrdNode
        // if (auto const& World{this->Controller->GetWorld()}; World.IsUnderlyingLevelValid())
        // {
        //     this->AsSurface()->SetInputMode(World.GetUnderlyingLevelChecked().InputMode);
        // }
    }

    this->GetMutableLocalEgo().ForEachMutableSubsystem([New = New.get()](JLocalEgoSubsystem* Subsystem)
    {
        Subsystem->OnNewPersonaController(New);
    });

    return;
}
