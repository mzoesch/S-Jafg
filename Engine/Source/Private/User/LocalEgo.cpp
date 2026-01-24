// Copyright mzoesch. All rights reserved.

#include "User/LocalEgo.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"
#include "Platform/Surface.h"
#include "Framework/Frontend.h"
#include "Framework/Pawn.h"
#include "Framework/PersonaController.h"
#include "Framework/Lackey.h"
#include "Subsystems/LocalEgoSubsystem.h"
#include "Stats/Stats.h"

void Jafg::LLocalEgo::Initialize()
{
    STAT_CYCLE_FUNCTION()

    check( Tasks::IsOnMasterThread() )
    check( this->bDecommissioned == false )

    LCommandLineInterface& Cli{ GEngine->GetCommandLineInterface() };
    this->VariableHandle_UpdateFrustum = Cli.RegisterVariable({"UpdateFrustum", LCliType::Type("Bool"), "true"});
    this->VariableHandle_VisualizeFrustum = Cli.RegisterVariable({"VisualizeFrustum", LCliType::Type("Bool"), "false"});
    this->VariableHandle_FrustumNearPlane = Cli.RegisterVariable({"NearFrustumPlane", LCliType::Type("Float"), "0.1f",
    LOnVariableChangedDelegate::CreateStrong([](const LString& InValue) -> void
    {
        if (GEngine)
        {
            f32 NearFrustum; Serialization::FromString(&NearFrustum, InValue);
            LOG_VERBOSE(LogEgo, "Setting all possessed eyes near frustum to [{}].", NearFrustum)

            for (auto& Surface : GEngine->GetLocalEgo().GetFrontend().GetSurfaces())
            {
                if (auto* Controller{ Surface->GetController() })
                {
                    if (auto* Pawn{ Controller->GetPawn() })
                    {
                        Pawn->SetNearFrustum(NearFrustum);
                    }
                }

                continue;
            }
        }

        return;
    })});
    this->VariableHandle_FrustumFarPlane = Cli.RegisterVariable({"FarFrustumPlane", LCliType::Type("Float"), "2000.0f",
    LOnVariableChangedDelegate::CreateStrong([](const LString& InValue) -> void
    {
        if (GEngine)
        {
            f32 FarFrustum; Serialization::FromString(&FarFrustum, InValue);
            LOG_VERBOSE(LogEgo, "Setting all possessed eyes far frustum to [{}].", FarFrustum)

            for (auto& Surface : GEngine->GetLocalEgo().GetFrontend().GetSurfaces())
            {
                if (auto* Controller{ Surface->GetController() })
                {
                    if (auto* Pawn{ Controller->GetPawn() })
                    {
                        Pawn->SetFarFrustum(FarFrustum);
                    }
                }

                continue;
            }
        }

        return;
    })});

    this->Frontend.Initialize(&this->Outer);

    this->Collection.InitializeDeferred(&this->Outer);
    this->Collection.InitializeSubsystems<JLocalEgoSubsystem>();

    this->Frontend.GetCollection()->InitializeDeferred(&this->Outer);
    this->Frontend.GetCollection()->InitializeSubsystems<JFrontendSubsystem>();

    return;
}

void Jafg::LLocalEgo::Tick(const f32 DeltaTime)
{
    STAT_CYCLE_FUNCTION()

    if (const i32 PurgedFactories{ Private::PurgeWidgetFactories() }; PurgedFactories > 0)
    {
        LOG_TRACE(LogWidgetFramework, "Purged {} widget factories.", PurgedFactories)
    }

    this->Frontend.Tick();

    return;
}

void Jafg::LLocalEgo::TearDown()
{
    check( GEngine )

    check( this->bDecommissioned == false )
    checkCode( this->bDecommissioned = true )

    this->Collection.TearDownSubsystems();

    this->Frontend.TearDown();
    this->Outer.TearDown();

    GEngine->GetCommandLineInterface().UnregisterVariable(&this->VariableHandle_UpdateFrustum);
    GEngine->GetCommandLineInterface().UnregisterVariable(&this->VariableHandle_VisualizeFrustum);
    GEngine->GetCommandLineInterface().UnregisterVariable(&this->VariableHandle_FrustumNearPlane);
    GEngine->GetCommandLineInterface().UnregisterVariable(&this->VariableHandle_FrustumFarPlane);

    return;
}

Jafg::LEngine& Jafg::LLocalEgo::GetEngine()
{
    check( GEngine )
    return *GEngine;
}

Jafg::LCommandLineInterface& Jafg::LLocalEgo::GetCommandLineInterface()
{
    check( GEngine )
    return GEngine->GetCommandLineInterface();
}
