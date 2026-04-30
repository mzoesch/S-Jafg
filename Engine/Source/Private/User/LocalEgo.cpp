// Copyright mzoesch. All rights reserved.

#include "User/LocalEgo.h"
#include "Engine/Engine.h"
#include "Framework/Frontend.h"
#include "Framework/Pawn.h"
#include "Framework/PersonaController.h"
#include "Subsystems/LocalEgoSubsystem.h"
#include "Stats/Stats.h"

void Jafg::LLocalEgo::Initialize()
{
    STAT_CYCLE_FUNCTION()

    check(Tasks::IsOnMasterThread())
    check(this->bDecommissioned == false)

    LCommandLineInterface& Cli{GMutableEngine->GetCommandLineInterface() };
    this->VariableHandle_UpdateFrustum = Cli.RegisterVariable({"UpdateFrustum", LCliType::Type("Bool"), "true"});
    this->VariableHandle_VisualizeFrustum = Cli.RegisterVariable({"VisualizeFrustum", LCliType::Type("Bool"), "false"});
    this->VariableHandle_FrustumNearPlane = Cli.RegisterVariable({"NearFrustumPlane", LCliType::Type("Float"), "0.1f",
    LOnVariableChangedDelegate::CreateStrong([](const LString& InValue) -> void
    {
        if (GEngine)
        {
            f32 NearFrustum; Serde::FromString(&NearFrustum, InValue);
            LOG_VERBOSE(LogEgo, "Setting all possessed eyes near frustum to [{}].", NearFrustum)

            for (auto& Track : GEngine->GetTracks())
            {
                if (Track.ChildWorld)
                {
                    auto& World{*Track.ChildWorld};
                    for (auto& Employee : World.GetEmployees())
                    {
                        if (APawn* Pawn{DynamicCast<APawn>(Employee.get())})
                        {
                            if (Pawn->IsPossessedLocally())
                            {
                                Pawn->SetNearFrustum(NearFrustum);
                            }
                        }
                    }
                }
            }
        }

        return;
    })});
    this->VariableHandle_FrustumFarPlane = Cli.RegisterVariable({"FarFrustumPlane", LCliType::Type("Float"), "2000.0f",
    LOnVariableChangedDelegate::CreateStrong([](const LString& InValue) -> void
    {
        if (GEngine)
        {
            f32 FarFrustum; Serde::FromString(&FarFrustum, InValue);
            LOG_VERBOSE(LogEgo, "Setting all possessed eyes far frustum to [{}].", FarFrustum)

            for (auto& Track : GEngine->GetTracks())
            {
                if (Track.ChildWorld)
                {
                    auto& World{*Track.ChildWorld};
                    for (auto& Employee : World.GetEmployees())
                    {
                        if (APawn* Pawn{DynamicCast<APawn>(Employee.get())})
                        {
                            if (Pawn->IsPossessedLocally())
                            {
                                Pawn->SetFarFrustum(FarFrustum);
                            }
                        }
                    }
                }
            }
        }

        return;
    })});

    this->Collection.InitializeDeferred(&this->Outer);
    this->Collection.InitializeSubsystems<JLocalEgoSubsystem>();

    this->Frontend.Initialize(&this->Outer);

    return;
}

void Jafg::LLocalEgo::TearDown()
{
    check(GMutableEngine)
    check(this->bDecommissioned == false)
    checkCode(this->bDecommissioned = true)

    this->Collection.TearDownSubsystems();
    Detail::GetGlobalCarnifex().KillAllGarbageChildren();

    this->Frontend.TearDown();
    this->Outer.TearDown();

    GMutableEngine->GetCommandLineInterface().UnregisterVariable(&this->VariableHandle_UpdateFrustum);
    GMutableEngine->GetCommandLineInterface().UnregisterVariable(&this->VariableHandle_VisualizeFrustum);
    GMutableEngine->GetCommandLineInterface().UnregisterVariable(&this->VariableHandle_FrustumNearPlane);
    GMutableEngine->GetCommandLineInterface().UnregisterVariable(&this->VariableHandle_FrustumFarPlane);

    return;
}

Jafg::LEngine const& Jafg::LLocalEgo::GetEngine() const
{
    check(GEngine)
    return *GEngine;
}

Jafg::LEngine& Jafg::LLocalEgo::GetMutableEngine()
{
    check(GMutableEngine)
    return *GMutableEngine;
}

Jafg::LCommandLineInterface const& Jafg::LLocalEgo::GetCommandLineInterface() const
{
    check(GEngine)
    return GEngine->GetCommandLineInterface();
}

Jafg::LCommandLineInterface& Jafg::LLocalEgo::GetMutableCommandLineInterface()
{
    check(GMutableEngine)
    return GMutableEngine->GetCommandLineInterface();
}
