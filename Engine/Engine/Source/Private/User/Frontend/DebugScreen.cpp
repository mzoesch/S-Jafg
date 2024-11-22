// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Frontend/DebugScreen.h"
#include "Build/EngineBuildInfo.h"
#include "Core/Application.h"
#include "Engine/Engine.h"
#include "Engine/Framework/ApplicationInstance.h"
#include "Engine/Framework/Pawn.h"
#include "Engine/Framework/PersonaController.h"
#include "System/MaterialSubsystem.h"
#include "System/TextureSubsystem.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "Widgets/Spacer.h"
#include "Widgets/TextBlock.h"
#include "Widgets/VBox.h"

void Jafg::WDebugScreen::Construct()
{
    Super::Construct();

    JMaterialSubsystem* MaterialSubsystem = this->GetApplicationInstance()->GetSubsystem<JMaterialSubsystem>();

    MakeRootNode(WVBox)
    [
        NewNode(WVBox)
        [
            NewNode(WTextBlock)
            & LSimpleString::SprintF("JAFG v{}", BuildInfo::GetEngineVersion().ToString())
            & LTextBlockBrush::MakeDefaultSmall()
        ]
        [
            NewNode(WTextBlock) >> this->FpsSection
            & LTextBlockBrush::MakeDefaultSmall()
        ]
        [
            NewNode(WTextBlock) >> this->TimeStatsSection
            & LTextBlockBrush::MakeDefaultSmall()
        ]
    ]
    [
        NewNode(WSpacer)
        .SetHeight(20.0f)
    ]
    [
        NewNode(WVBox)
        [
            NewNode(WTextBlock) >> this->LocalPawnLocationSection
            & LTextBlockBrush::MakeDefaultSmall()
        ]
        [
            NewNode(WTextBlock) >> this->LocalPawnFacingSection
            & LTextBlockBrush::MakeDefaultSmall()
        ]
    ]
    [
        NewNode(WSpacer)
        .SetHeight(20.0f)
    ]
    [
        NewNode(WVBox)
        [
            NewNode(WWidgetRegion)
            .SetRegionPadding(LPadding(100.0f))
            & &MaterialSubsystem->GetBlendOpaqueAtlasTexture()
        ]
    ]
    [
        NewNode(WVBox)
        [
            NewNode(WWidgetRegion)
            .SetRegionPadding(LPadding(100.0f))
            & &MaterialSubsystem->GetBlendersAtlasTexture()
        ]
    ]
    FinishWidgetStyling()

    return;
}

void Jafg::WDebugScreen::Tick()
{
    Super::Tick();

    const LLocalEgo* LocalEgo = GEngine->GetCheckedLocalEgo();

    if (this->LocalPawnLocationSection)
    {
        if (LocalEgo->DoesPossess())
        {
            const APersonaController* Controller = LocalEgo->GetPossessed();
            if (Controller->DoesPossess())
            {
                const LVector Location = Controller->GetPossessed()->GetTranslation();
                this->LocalPawnLocationSection->SetContent(LSimpleString::SprintF(
                    "XYZ: {:.3f} / {:.3f} / {:.3f}",
                    Location.X,
                    Location.Y,
                    Location.Z
                ));
            }
            else
            {
                this->LocalPawnLocationSection->SetContent("[ERR: No pawn]");
            }
        }
        else
        {
            this->LocalPawnLocationSection->SetContent("[ERR: No possessor]");
        }
    }

    if (this->LocalPawnFacingSection)
    {
        if (LocalEgo->DoesPossess())
        {
            const APersonaController* Controller = LocalEgo->GetPossessed();
            if (Controller->DoesPossess())
            {
                const LRotator Rotator = Controller->GetPossessed()->GetRotator();

                LSimpleString YawAsText = "N/A";
                if (Rotator.Yaw >= -45.f && Rotator.Yaw <= 45.f)
                {
                    YawAsText = "North (Towards positive X)";
                }
                else if (Rotator.Yaw > 45.f && Rotator.Yaw < 135.f)
                {
                    YawAsText = "East (Towards positive Y)";
                }
                else if (Rotator.Yaw >= 135.f || Rotator.Yaw <= -135.f)
                {
                    YawAsText = "South (Towards negative X)";
                }
                else if (Rotator.Yaw > -135.f && Rotator.Yaw < -45.f)
                {
                    YawAsText = "West (Towards negative Y)";
                }

                this->LocalPawnFacingSection->SetContent(LSimpleString::SprintF(
                    "Facing: {} ({:.2f}Y / {:.2f}P)",
                    YawAsText, Rotator.Yaw, Rotator.Pitch
                ));
            }
            else
            {
                this->LocalPawnFacingSection->SetContent("[ERR: No pawn]");
            }
        }
        else
        {
            this->LocalPawnFacingSection->SetContent("[ERR: No possessor]");
        }
    }

    this->InvalidateCacheTime -= Application::GetDeltaTime();
    if (this->InvalidateCacheTime > 0.0)
    {
        return;
    }
    this->InvalidateCacheTime = this->ResetTime;

    this->SlowTick();

    return;
}

void Jafg::WDebugScreen::SlowTick()
{
    const JUserPreferences* UserPreferences = GetDefault<JUserPreferences>();

    if (this->FpsSection)
    {
        this->FpsSection->SetContent(
            LSimpleString::SprintF(
                "{} Fps @ {:.2f} ms T: {}; VSync: {} - Fcsssi: {}",
                static_cast<int32>(Application::GetCurrentFps()),
                Application::GetDeltaTimeAsFloat() * 1'000.0f,
                "?",
                UserPreferences->GetVSyncEnabled(),
                Application::GetFrameCount()
            )
        );
    }

    if (this->TimeStatsSection)
    {
        this->TimeStatsSection->SetContent(
            LSimpleString::SprintF(
                "Steady: Avg {}fps/{:.2f}ms; Low: {}fps/{:.2f}ms; High: {}fps/{:.2f}ms",
                static_cast<int32>(static_cast<double>(Application::GetPreviousFrameCount()) / Application::GetRealTimeOfPreviousStatisticsDuration()),
                static_cast<float>(1.0 / (static_cast<double>(Application::GetPreviousFrameCount()) / Application::GetRealTimeOfPreviousStatisticsDuration())) * 1'000.0f,
                static_cast<int32>(1.0 / Application::GetPreviousHighestDeltaTime()),
                Application::GetPreviousHighestDeltaTime() * 1'000.0f,
                static_cast<int32>(1.0 / Application::GetPreviousLowestDeltaTime()),
                Application::GetPreviousLowestDeltaTime() * 1'000.0f
            )
        );
    }

    return;
}
