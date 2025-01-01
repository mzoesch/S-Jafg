// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Frontend/Osd/DebugScreen.h"
#include "Build/EngineBuildInfo.h"
#include "Core/Application.h"
#include "Engine/Engine.h"
#include "Engine/Framework/ApplicationInstance.h"
#include "Engine/Framework/Pawn.h"
#include "Engine/Framework/PersonaController.h"
#include "MyWorld/ChunkKey.h"
#include "MyWorld/VoxelKey.h"
#include "MyWorld/Chunk/Chunk.h"
#include "System/MaterialSubsystem.h"
#include "System/TextureSubsystem.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "Widgets/Spacer.h"
#include "Widgets/TextBlock.h"
#include "Widgets/VBox.h"

Jafg::WDebugScreen::WDebugScreen(const LObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
    this->SetShouldTick(true);
    return;
}

void Jafg::WDebugScreen::Construct()
{
    Super::Construct();

    JMaterialSubsystem* MaterialSubsystem = this->GetApplicationInstance()->GetSubsystem<JMaterialSubsystem>();

    MakeRootNode(WWidgetRegion)
    .SetAnchor(EAnchor::Fill)
    [
        NewNode(WVBox)
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
            [
                NewNode(WTextBlock) >> this->LocalPawnChunkSection
                & LTextBlockBrush::MakeDefaultSmall()
            ]
            [
                NewNode(WTextBlock) >> this->LocalPawnVoxelSection
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
                .SetPadding(LPadding(100.0f))
                & &MaterialSubsystem->GetBlendOpaqueAtlasTexture()
            ]
        ]
        [
            NewNode(WVBox)
            [
                NewNode(WWidgetRegion)
                .SetPadding(LPadding(100.0f))
                & &MaterialSubsystem->GetBlendersAtlasTexture()
            ]
        ]
    ]
    [
        NewNode(WVBox)
        .SetAnchor(EAnchor::TopRight)
        [
            NewNode(WVBox)
            .SetAnchor(EAnchor::TopRight)
            [
                NewNode(WTextBlock)
                & EAnchor::TopRight
                & "Memory statistics"
                & LTextBlockBrush::MakeDefaultSmall()
            ]
            [
                NewNode(WTextBlock)
                & EAnchor::TopRight
                & "Central processing unit information"
                & LTextBlockBrush::MakeDefaultSmall()
            ]
            [
                NewNode(WTextBlock)
                & EAnchor::TopRight
                & "Graphics processing unit information"
                & LTextBlockBrush::MakeDefaultSmall()
            ]
            [
                NewNode(WTextBlock)
                & EAnchor::TopRight
                & "Display information"
                & LTextBlockBrush::MakeDefaultSmall()
            ]
        ]
        [
            NewNode(WSpacer)
            .SetHeight(20.0f)
        ]
        [
            NewNode(WVBox)
            .SetAnchor(EAnchor::TopRight)
            [
                NewNode(WTextBlock) >> this->LocalPawnTarrgetVoxelSection
                & LTextBlockBrush::MakeDefaultSmall()
            ]
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

    if (this->LocalPawnChunkSection)
    {
        if (LocalEgo->DoesPossess())
        {
            const APersonaController* Controller = LocalEgo->GetPossessed();
            if (Controller->DoesPossess())
            {
                const LVector Location = Controller->GetPossessed()->GetTranslation();
                const LChunkKey Key = LChunkKey(Location);
                this->LocalPawnChunkSection->SetContent(LSimpleString::SprintF(
                    "Chunk: {} {} {}",
                    Key.X, Key.Y, Key.Z
                ));
            }
            else
            {
                this->LocalPawnChunkSection->SetContent("[ERR: No pawn]");
            }
        }
        else
        {
            this->LocalPawnChunkSection->SetContent("[ERR: No possessor]");
        }
    }

    if (this->LocalPawnVoxelSection)
    {
        if (LocalEgo->DoesPossess())
        {
            const APersonaController* Controller = LocalEgo->GetPossessed();
            if (Controller->DoesPossess())
            {
                const LVector Location = Controller->GetPossessed()->GetTranslation();
                const LVoxelKey Key = LVoxelKey::FromWorldLocation(Location);
                this->LocalPawnVoxelSection->SetContent(LSimpleString::SprintF(
                    "Local voxel: {} {} {}",
                    Key.X, Key.Y, Key.Z
                ));
            }
            else
            {
                this->LocalPawnVoxelSection->SetContent("[ERR: No pawn]");
            }
        }
        else
        {
            this->LocalPawnVoxelSection->SetContent("[ERR: No possessor]");
        }
    }

    if (this->LocalPawnTarrgetVoxelSection)
    {
        this->LocalPawnTarrgetVoxelSection->EmptyContent();
        if (LocalEgo->DoesPossess())
        {
            const APersonaController* Controller = LocalEgo->GetPossessed();
            if (Controller->DoesPossess())
            {
                for (const LHitResult& Hit : Controller->GetPossessed()->GetCurrentGenericTraceResults())
                {
                    if (AChunk* HitChunk = Hit.Actor->As<AChunk>(); HitChunk)
                    {
                        const LVoxelKey Key = LVoxelKey::FromWorldLocation(Hit.GlobalWorldLocation);
                        this->LocalPawnTarrgetVoxelSection->SetContent(LSimpleString::SprintF("Tv: {} {} {}", Key.X, Key.Y, Key.Z));
                        break;
                    }
                }
            }
        }
        if (this->LocalPawnTarrgetVoxelSection->GetContent().IsEmpty())
        {
            this->LocalPawnTarrgetVoxelSection->SetContent("Tv: N/A");
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
