// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Player/Frontend/DebugScreen.h"
#include "Build/EngineBuildInfo.h"
#include "Core/Application.h"
#include "Engine/Engine.h"
#include "Engine/Framework/Pawn.h"
#include "Engine/Framework/PlayerController.h"
#include "Player/LocalPlayer.h"
#include "User/UserPreferences.h"
#include "Widgets/Spacer.h"
#include "Widgets/TextBlock.h"
#include "Widgets/VBox.h"

void Jafg::WDebugScreen::Construct()
{
    Super::Construct();

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
    ]
    [
        NewNode(WSpacer)
        .SetHeight(20.0f)
    ]
    [
        NewNode(WVBox)
        [
            NewNode(WTextBlock) >> this->LocalPlayerLocationSection
            & LTextBlockBrush::MakeDefaultSmall()
        ]
        [
            NewNode(WTextBlock) >> this->LocalPlayerFacingSection
            & LTextBlockBrush::MakeDefaultSmall()
        ]
    ]
    FinishWidgetStyling()

    return;
}

void Jafg::WDebugScreen::Tick()
{
    Super::Tick();

    const LLocalPlayer* LocalPlayer = GEngine->GetCheckedLocalPlayer();

    if (this->LocalPlayerLocationSection)
    {
        if (LocalPlayer->DoesPossess())
        {
            const APlayerController* Controller = LocalPlayer->GetPossessed();
            if (Controller->DoesPossess())
            {
                const LVector Location = Controller->GetPossessed()->GetLocation();
                this->LocalPlayerLocationSection->SetContent(LSimpleString::SprintF(
                    "XYZ: {:.3f} / {:.3f} / {:.3f}",
                    Location.X,
                    Location.Y,
                    Location.Z
                ));
            }
            else
            {
                this->LocalPlayerLocationSection->SetContent("[ERR: No pawn]");
            }
        }
        else
        {
            this->LocalPlayerLocationSection->SetContent("[ERR: No possessor]");
        }
    }

    if (this->LocalPlayerFacingSection)
    {
        if (LocalPlayer->DoesPossess())
        {
            const APlayerController* Controller = LocalPlayer->GetPossessed();
            if (Controller->DoesPossess())
            {
                const LSimpleString Facing = Controller->GetPossessed()->GetFacingStringDebug();
                this->LocalPlayerFacingSection->SetContent(Facing);
            }
            else
            {
                this->LocalPlayerFacingSection->SetContent("[ERR: No pawn]");
            }
        }
        else
        {
            this->LocalPlayerFacingSection->SetContent("[ERR: No possessor]");
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
                "{} fps @ {:.2f} ms T: {}; vsync: {} - fcss: {}",
                static_cast<int32>(Application::GetCurrentFps()),
                Application::GetDeltaTimeAsFloat() * 100.0f,
                "?",
                UserPreferences->GetVSyncEnabled(),
                Application::GetFrameCount()
            )
        );
    }

    return;
}
