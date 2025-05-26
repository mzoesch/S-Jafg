// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Frontend/Osd/DebugScreen.h"
#include "Build/EngineBuildInfo.h"
#include "Core/Application.h"
#include "Engine/Engine.h"
#include "Framework/Pawn.h"
#include "Framework/PersonaController.h"
#include "MyWorld/ChunkKey.h"
#include "MyWorld/VoxelKey.h"
#include "MyWorld/Chunk/Chunk.h"
#include "System/MaterialSubsystem.h"
#include "System/TextureSubsystem.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "Widgets/Spacer.h"
#include "Widgets/TextBlock.h"
#include "Widgets/VRegion.h"
#include "Debug/DebugTraceSphere.h"
#include "Debug/DebugTraceCube.h"
#include "Debug/DebugTraceLine.h"
#include "Debug/DebugTracePlane.h"
#include "MyWorld/Validation/ChunkValidationUtility.h"

Jafg::WDebugScreen::WDebugScreen(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->SetShouldTick(true);
    return;
}

void Jafg::WDebugScreen::Construct()
{
    Super::Construct();

#define TEXT_BLOCK_FONT_SIZE Body

    const JMaterialSubsystem* MaterialSubsystem = this->GetEngine()->GetSubsystem<JMaterialSubsystem>();

    MakeRootNode(WRegion)
        .Anchor(EAnchor::Fill)
    [
        NewNode(WVRegion)
            .Anchor(EAnchor::TopLeft)
            .Tint(LColor(255,0,0,32))
        [
            NewNode(WVRegion)
                .Type(ERegionBrush::Box)
                .Tint(LColor(0,0,32,128))
            [
                NewNode(WTextBlock)
                    .Brush(LTextBlockBrush::TEXT_BLOCK_FONT_SIZE().TintRet({0, 0, 0, 128}))
                    .Content(LString::SprintF("JAFG v{}", BuildInfo::GetEngineVersion().ToString()))
                +
                NewNode(WTextBlock).SaveTo(&this->FpsSection)
                    .Brush(LTextBlockBrush::TEXT_BLOCK_FONT_SIZE().TintRet({0, 0, 0, 128}))
                +
                NewNode(WTextBlock).SaveTo(&this->TimeStatsSection)
                    .Brush(LTextBlockBrush::TEXT_BLOCK_FONT_SIZE().TintRet({0, 0, 0, 128}))
            ]
            +
            NewNode(WSpacer).Height(20.0f)
            +
            NewNode(WVRegion)
                .Type(ERegionBrush::Box)
                .Tint(LColor(0,0,32,128))
            [
                NewNode(WTextBlock).SaveTo(&this->LocalPawnLocationSection)
                    .Brush(LTextBlockBrush::TEXT_BLOCK_FONT_SIZE().TintRet({0, 0, 0, 128}))
                +
                NewNode(WTextBlock).SaveTo(&this->LocalPawnFacingSection)
                    .Brush(LTextBlockBrush::TEXT_BLOCK_FONT_SIZE().TintRet({0, 0, 0, 128}))
                +
                NewNode(WTextBlock).SaveTo(&this->LocalPawnChunkSection)
                    .Brush(LTextBlockBrush::TEXT_BLOCK_FONT_SIZE().TintRet({0, 0, 0, 128}))
                +
                NewNode(WTextBlock).SaveTo(&this->LocalPawnVoxelSection)
                    .Brush(LTextBlockBrush::TEXT_BLOCK_FONT_SIZE().TintRet({0, 0, 0, 128}))
            ]
            +
            NewNode(WSpacer).Height(20.0f)
            +
            NewNode(WVRegion)
            [
                NewNode(WRegion)
                    .Padding(LPadding(70.0f))
                    .Type(ERegionBrush::Box)
                    .Texture(&MaterialSubsystem->GetBlendOpaqueAtlasTexture())
            ]
            + NewNode(WVRegion)
            [
                NewNode(WRegion)
                    .Padding(LPadding(70.0f))
                    .Type(ERegionBrush::Box)
                    .Texture(&MaterialSubsystem->GetBlendersAtlasTexture())
            ]
        ]
        +
        NewNode(WVRegion)
            .Anchor(EAnchor::TopRight)
            .Type(ERegionBrush::Box)
            .Tint(LColor(255,0,0,32))
        [
            NewNode(WVRegion)
                .Anchor(EAnchor::TopRight)
                .Type(ERegionBrush::Box)
                .Tint(LColor(0,0,32,128))
            [
                NewNode(WTextBlock)
                    .Anchor(EAnchor::TopRight)
                    .Content("Memory statistics")
                    .Brush(LTextBlockBrush::TEXT_BLOCK_FONT_SIZE().TintRet({0, 0, 0, 128}))
                +
                NewNode(WTextBlock)
                    .Anchor(EAnchor::TopRight)
                    .Content("Central processing unit information")
                    .Brush(LTextBlockBrush::TEXT_BLOCK_FONT_SIZE().TintRet({0, 0, 0, 128}))
                +
                NewNode(WTextBlock)
                    .Anchor(EAnchor::TopRight)
                    .Content("Graphics processing unit information")
                    .Brush(LTextBlockBrush::TEXT_BLOCK_FONT_SIZE().TintRet({0, 0, 0, 128}))
                +
                NewNode(WTextBlock)
                    .Anchor(EAnchor::TopRight)
                    .Content("Display information")
                    .Brush(LTextBlockBrush::TEXT_BLOCK_FONT_SIZE().TintRet({0, 0, 0, 128}))
            ]
            + NewNode(WSpacer)
                .Height(20.0f)
            + NewNode(WVRegion)
                .Anchor(EAnchor::TopRight)
                .Type(ERegionBrush::Box)
                .Tint(LColor::Cyan)
            [
                NewNode(WTextBlock).SaveTo(&this->LocalPawnTargetVoxelSectionDestroy)
                    .Brush(LTextBlockBrush::TEXT_BLOCK_FONT_SIZE().TintRet({0, 0, 0, 128}))
                +
                NewNode(WTextBlock).SaveTo(&this->LocalPawnTargetVoxelSectionCreate)
                    .Brush(LTextBlockBrush::TEXT_BLOCK_FONT_SIZE().TintRet({0, 0, 0, 128}))
            ]
        ]
    ]
    FinishWidgetStyling()

#undef TEXT_BLOCK_FONT_SIZE

    return;
}

void Jafg::WDebugScreen::Tick()
{
    Super::Tick();

    const LLocalEgo* LocalEgo = GEngine->GetLocalEgo();
    const APersonaController* Controller = LocalEgo->GetCheckedPossessed();
    if (Controller->DoesPossess())
    {
        if (this->LocalPawnLocationSection)
        {
            const LVector Location = Controller->GetPossessed()->GetTranslation();
            this->LocalPawnLocationSection->SetContent(LString::SprintF(
                "XYZ: {:.3f} / {:.3f} / {:.3f}",
                Location.X,
                Location.Y,
                Location.Z
            ));
        }

        if (this->LocalPawnFacingSection)
        {
            const LRotator Rotator = Controller->GetPossessed()->GetRotator();
            LString YawAsText = "N/A";
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
            this->LocalPawnFacingSection->SetContent(LString::SprintF(
                "Facing: {} ({:.2f}Y / {:.2f}P)",
                YawAsText, Rotator.Yaw, Rotator.Pitch
            ));
        }

        if (this->LocalPawnChunkSection)
        {
            const LVector Location = Controller->GetPossessed()->GetTranslation();
            const LChunkKey Key = LChunkKey(Location);
            this->LocalPawnChunkSection->SetContent(LString::SprintF(
                "Chunk: {} {} {}",
                Key.X, Key.Y, Key.Z
            ));
        }

        if (this->LocalPawnVoxelSection)
        {
            const LVector Location = Controller->GetPossessed()->GetTranslation();
            const LVoxelKey Key = LVoxelKey::FromWorldSpace(Location);
            this->LocalPawnVoxelSection->SetContent(LString::SprintF(
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
                    const LVoxelKey Key = LVoxelKey::FromWorldSpace(Hit.GlobalWorldLocation);
                    this->LocalPawnTargetVoxelSectionDestroy->SetContent(LString::SprintF("TvD: {} {} {}", Key.X, Key.Y, Key.Z));
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
                    this->LocalPawnTargetVoxelSectionCreate->SetContent(LString::SprintF("TvC: {} {} {}", Key.X, Key.Y, Key.Z));
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
            const AChunk* HitChunk = Hit.Actor->As<AChunk>();
            if (HitChunk == nullptr)
            {
                continue;
            }

            LWorld* World = Controller->GetPossessed()->GetWorld();
            const LChunkKey CKey = HitChunk->GetChunkKey();

            World->AddTemporalObject(LDebugTraceSphere(
                LTemporalWorldObject::DrawOnce, Hit.GlobalWorldLocation, 0.1f,
                LDebugTraceSphereVisualParams(16, 16, LColor::Green)
            ));

            if (Hit.SurfaceNormal)
            {
                const LVector   WorldHit_Create = Hit.GlobalWorldLocation + Hit.SurfaceNormal.GetValue() * 0.5f;
                const LVoxelKey VKey_Create = LVoxelKey::FromWorldSpace(WorldHit_Create);
                const LVector WorldSpaceCenter_Create  = CKey.ToWorldSpace() + LVector(VKey_Create.X, VKey_Create.Y, VKey_Create.Z);
                World->AddTemporalObject(LDebugTraceLine(
                    LTemporalWorldObject::DrawOnce, Hit.GlobalWorldLocation, Hit.GlobalWorldLocation + Hit.SurfaceNormal.GetValue(),
                    LDebugTraceLineVisualParams(LColor::Magenta)
                ));
                World->AddTemporalObject(LDebugTraceSphere(
                    LTemporalWorldObject::DrawOnce, WorldHit_Create, 0.1f,
                    LDebugTraceSphereVisualParams(16, 16, LColor::Magenta)
                ));
                World->AddTemporalObject(LDebugTraceSphere(
                    LTemporalWorldObject::DrawOnce,
                    CKey.ToWorldSpace() + HitChunk->CreateRelativeVoxelKey(WorldHit_Create).ToWorldSpace() + LVector(0.5f), 0.6f,
                    LDebugTraceSphereVisualParams(16, 16, LColor::Emerald)
                ));
                World->AddTemporalObject(LDebugTraceCube(
                    LTemporalWorldObject::DrawOnce, WorldSpaceCenter_Create, LVector::One(),
                    LDebugTraceCubeVisualParams(LColor::Blue)
                ));
            }
            break;
        }

        // Chunk debug lines
        {
            LWorld* World = Controller->GetPossessed()->GetWorld();

            const LVector PawnTranslation = Controller->GetPossessed()->GetTranslation();
            LChunkKey CKey = LChunkKey(PawnTranslation);
            const LVector ChunkCenter = CKey.ToWorldSpace();

            World->AddTemporalObject(LDebugTraceCube(
                LTemporalWorldObject::DrawOnce, LVector::Zero() + ChunkCenter, LVector::One() * MwStatics::ChunkSize,
                LDebugTraceCubeVisualParams(LColor::Yellow)
            ));

            for (i32 izDelta = 2; izDelta < MwStatics::ChunkSize; izDelta += 2)
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
                const LVector WorldSpaceCenter = Key.ToWorldSpace();
                World->AddTemporalObject(LDebugTraceCube(
                    LTemporalWorldObject::DrawOnce, LVector::Zero() + WorldSpaceCenter, LVector::One() * MwStatics::ChunkSize,
                    LDebugTraceCubeVisualParams(LColor::Red)
                ));
            }

            const TArray<LChunkKey> OtherChunks = Validation::GetAllChunksFromCenterAsBox(CKey, 5, 3, CKey.Z - 1);
            for (const LChunkKey& Key : OtherChunks)
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

                World->AddTemporalObject(LDebugTraceLine
                (
                    LTemporalWorldObject::DrawOnce,
                    Key.ToWorldSpace() + LVector::Down() * MwStatics::ChunkSize * 10,
                    Key.ToWorldSpace() + LVector::Up() * MwStatics::ChunkSize * 10,
                    LDebugTraceLineVisualParams(LColor::Blue)
                ));

                continue;
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
            LString::SprintF(
                "{} Fps @ {:.2f} ms T: {}; VSync: {} - Fcsssi: {}; L@{:.2f} Lh@{:.2f} I@{:.2f}",
                static_cast<i32>(Application::GetCurrentFps()),
                Application::GetDeltaTimeAsFloat() * 1'000.0f,
                UserPreferences->MaxFps,
                UserPreferences->bVSyncEnabled,
                Application::GetFrameCount(),
                Application::GetLostDeltaTime() * 1'000.0f,
                Application::GetHighestLostDeltaTime() * 1'000.0f,
                Application::GetIdleDeltaTime() * 1'000.0f
            )
        );
    }

    if (this->TimeStatsSection)
    {
        this->TimeStatsSection->SetContent(
            LString::SprintF(
                "Steady: Avg {}fps/{:.2f}ms; Low: {}fps/{:.2f}ms; High: {}fps/{:.2f}ms",
                static_cast<i32>(static_cast<double>(Application::GetPreviousFrameCount()) / Application::GetRealTimeOfPreviousStatisticsDuration()),
                static_cast<float>(1.0 / (static_cast<double>(Application::GetPreviousFrameCount()) / Application::GetRealTimeOfPreviousStatisticsDuration())) * 1'000.0f,
                static_cast<i32>(1.0 / Application::GetPreviousHighestDeltaTime()),
                Application::GetPreviousHighestDeltaTime() * 1'000.0f,
                static_cast<i32>(1.0 / Application::GetPreviousLowestDeltaTime()),
                Application::GetPreviousLowestDeltaTime() * 1'000.0f
            )
        );
    }

    return;
}
