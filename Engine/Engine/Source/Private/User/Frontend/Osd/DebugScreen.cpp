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
#include "Debug/DebugTraceSphere.h"
#include "Debug/DebugTraceCube.h"
#include "Debug/DebugTraceLine.h"
#include "Debug/DebugTracePlane.h"
#include "MyWorld/Validation/ChunkValidationUtility.h"

Jafg::WDebugScreen::WDebugScreen(const LObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
    this->SetShouldTick(true);
    return;
}

void Jafg::WDebugScreen::Construct()
{
    Super::Construct();

    const JMaterialSubsystem* MaterialSubsystem = this->GetApplicationInstance()->GetSubsystem<JMaterialSubsystem>();

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
                .SetPadding(LPadding(70.0f))
                & &MaterialSubsystem->GetBlendOpaqueAtlasTexture()
            ]
        ]
        [
            NewNode(WVBox)
            [
                NewNode(WWidgetRegion)
                .SetPadding(LPadding(70.0f))
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
                NewNode(WTextBlock) >> this->LocalPawnTargetVoxelSectionDestroy
                & LTextBlockBrush::MakeDefaultSmall()
            ]
            [
                NewNode(WTextBlock) >> this->LocalPawnTargetVoxelSectionCreate
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
    const APersonaController* Controller = LocalEgo->GetCheckedPossessed();
    if (Controller->DoesPossess())
    {
        if (this->LocalPawnLocationSection)
        {
            const LVector Location = Controller->GetPossessed()->GetTranslation();
            this->LocalPawnLocationSection->SetContent(LSimpleString::SprintF(
                "XYZ: {:.3f} / {:.3f} / {:.3f}",
                Location.X,
                Location.Y,
                Location.Z
            ));
        }

        if (this->LocalPawnFacingSection)
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

        if (this->LocalPawnChunkSection)
        {
            const LVector Location = Controller->GetPossessed()->GetTranslation();
            const LChunkKey Key = LChunkKey(Location);
            this->LocalPawnChunkSection->SetContent(LSimpleString::SprintF(
                "Chunk: {} {} {}",
                Key.X, Key.Y, Key.Z
            ));
        }

        if (this->LocalPawnVoxelSection)
        {
            const LVector Location = Controller->GetPossessed()->GetTranslation();
            const LVoxelKey Key = LVoxelKey::FromWorldLocation(Location);
            this->LocalPawnVoxelSection->SetContent(LSimpleString::SprintF(
                "Local voxel: {} {} {}",
                Key.X, Key.Y, Key.Z
            ));
        }

        if (this->LocalPawnTargetVoxelSectionDestroy)
        {
            this->LocalPawnTargetVoxelSectionDestroy->EmptyContent();
            for (const LHitResult& Hit : Controller->GetPossessed()->GetCurrentGenericTraceResults())
            {
                if (const AChunk* HitChunk = Hit.Actor->As<AChunk>(); HitChunk)
                {
                    const LVoxelKey Key = LVoxelKey::FromWorldLocation(Hit.GlobalWorldLocation);
                    this->LocalPawnTargetVoxelSectionDestroy->SetContent(LSimpleString::SprintF("TvD: {} {} {}", Key.X, Key.Y, Key.Z));
                    break;
                }
            }
            if (this->LocalPawnTargetVoxelSectionDestroy->GetContent().IsEmpty())
            {
                this->LocalPawnTargetVoxelSectionDestroy->SetContent("TvD: N/A");
            }
        }

        if (this->LocalPawnTargetVoxelSectionCreate)
        {
            this->LocalPawnTargetVoxelSectionCreate->EmptyContent();
            for (const LHitResult& Hit : Controller->GetPossessed()->GetCurrentGenericTraceResults())
            {
                if (const AChunk* HitChunk = Hit.Actor->As<AChunk>(); HitChunk)
                {
                    const LVoxelKey Key = HitChunk->CreateRelativeVoxelKey(Hit.GlobalWorldLocation + Hit.SurfaceNormal * 0.5f);
                    this->LocalPawnTargetVoxelSectionCreate->SetContent(LSimpleString::SprintF("TvC: {} {} {}", Key.X, Key.Y, Key.Z));
                    break;
                }
            }
            if (this->LocalPawnTargetVoxelSectionCreate->GetContent().IsEmpty())
            {
                this->LocalPawnTargetVoxelSectionCreate->SetContent("TvC: N/A");
            }
        }

        for (const LHitResult& Hit : Controller->GetPossessed()->GetCurrentGenericTraceResults())
        {
            if (const AChunk* HitChunk = Hit.Actor->As<AChunk>(); HitChunk)
            {
                const LVector   WorldHit = Hit.GlobalWorldLocation + Hit.SurfaceNormal * 0.5f;
                const LVoxelKey VKey_Destroy = LVoxelKey::FromWorldLocation(Hit.GlobalWorldLocation);
                const LVoxelKey VKey_Create = LVoxelKey::FromWorldLocation(WorldHit);
                const LChunkKey CKey = HitChunk->GetChunkKey();

                const LVector WorldSpaceCenter_Destroy = CKey.ToWorldSpaceVector() + LVector(VKey_Destroy.X, VKey_Destroy.Y, VKey_Destroy.Z);
                const LVector WorldSpaceCenter_Create  = CKey.ToWorldSpaceVector() + LVector(VKey_Create.X, VKey_Create.Y, VKey_Create.Z);

                LWorld* World = Controller->GetPossessed()->GetWorld();
                World->AddTemporalObject(LDebugTraceSphere(
                    LTemporalWorldObject::DrawOnce, Hit.GlobalWorldLocation, 0.1f,
                    LDebugTraceSphereVisualParams(16, 16, LColor::Green)
                ));
                World->AddTemporalObject(LDebugTraceCube(
                    LTemporalWorldObject::DrawOnce, WorldSpaceCenter_Destroy, LVector::One(),
                    LDebugTraceCubeVisualParams(LColor::Red)
                ));
                World->AddTemporalObject(LDebugTraceCube(
                    LTemporalWorldObject::DrawOnce, WorldSpaceCenter_Create, LVector::One(),
                    LDebugTraceCubeVisualParams(LColor::Blue)
                ));
                break;
            }
        }

        // Chunk debug lines
        {
            LWorld* World = Controller->GetPossessed()->GetWorld();

            const LVector PawnTranslation = Controller->GetPossessed()->GetTranslation();
            LChunkKey CKey = LChunkKey(PawnTranslation);
            const LVector ChunkCenter = CKey.ToWorldSpaceVector();

            World->AddTemporalObject(LDebugTraceCube(
                LTemporalWorldObject::DrawOnce, LVector::Zero() + ChunkCenter, LVector::One() * MwStatics::ChunkSize,
                LDebugTraceCubeVisualParams(LColor::Yellow)
            ));

            for (int32 izDelta = 2; izDelta < MwStatics::ChunkSize; izDelta += 2)
            {
                const float zDelta = static_cast<float>(izDelta);
                const LVector P1 = ChunkCenter + LVector::Up() * zDelta;
                const LVector P2 = P1 + LVector::Right() * MwStatics::ChunkSize;
                const LVector P3 = P1 + (LVector::Right() + LVector::Forward()) * MwStatics::ChunkSize;
                const LVector P4 = P1 + LVector::Forward() * MwStatics::ChunkSize;

                World->AddTemporalObject(LDebugTracePlane(
                    LTemporalWorldObject::DrawOnce,
                    P1, P2, P3, P4,
                    LDebugTracePlaneVisualParams(LColor::Yellow)
                ));
            }

            for (const LChunkKey& Key : CKey.GetNeighboringChunkKeys())
            {
                const LVector WorldSpaceCenter = Key.ToWorldSpaceVector();
                World->AddTemporalObject(LDebugTraceCube(
                    LTemporalWorldObject::DrawOnce, LVector::Zero() + WorldSpaceCenter, LVector::One() * MwStatics::ChunkSize,
                    LDebugTraceCubeVisualParams(LColor::Red)
                ));
            }

            TdhArray<LChunkKey2> OtherChunks;
            Validation::GetAllChunksFromCenterAsBox(CKey.ToVerticalKey(), 5, OtherChunks);

            for (const LChunkKey2& Key : OtherChunks)
            {
                if (
                       Key.X == CKey.X + 0 && Key.Y == CKey.Y + 0
                    || Key.X == CKey.X + 1 && Key.Y == CKey.Y + 0
                    || Key.X == CKey.X + 0 && Key.Y == CKey.Y + 1
                    || Key.X == CKey.X + 1 && Key.Y == CKey.Y + 1
                )
                {
                    continue;
                }
                World->AddTemporalObject(LDebugTraceLine(
                    LTemporalWorldObject::DrawOnce,
                    Key.ToWorldSpaceVector() + LVector::Down() * MwStatics::ChunkSize * 10,
                    Key.ToWorldSpaceVector() + LVector::Up() * MwStatics::ChunkSize * 10,
                    LDebugTraceLineVisualParams(LColor::Blue)
                ));
            }

        }
    }
    else
    {
        if (this->LocalPawnLocationSection)
        {
            this->LocalPawnLocationSection->SetContent("XYZ: [ERR: No pawn]");
        }
        if (this->LocalPawnFacingSection)
        {
            this->LocalPawnFacingSection->SetContent("Facing: [ERR: No pawn]");
        }
        if (this->LocalPawnChunkSection)
        {
            this->LocalPawnChunkSection->SetContent("Chunk: [ERR: No pawn]");
        }
        if (this->LocalPawnVoxelSection)
        {
            this->LocalPawnVoxelSection->SetContent("Local voxel: [ERR: No pawn]");
        }
        if (this->LocalPawnTargetVoxelSectionDestroy)
        {
            this->LocalPawnTargetVoxelSectionDestroy->SetContent("TvD: [ERR: No pawn]");
        }
        if (this->LocalPawnTargetVoxelSectionCreate)
        {
            this->LocalPawnTargetVoxelSectionCreate->SetContent("TvC: [ERR: No pawn]");
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
