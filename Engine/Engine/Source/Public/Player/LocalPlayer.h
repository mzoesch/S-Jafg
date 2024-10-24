// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

class LHud;
class LSurface;
class LPlayerInput;
class APlayerController;

/**
 * Represents a player that is considered local (physically present) on the
 * current machine where this application instance is running.
 * It is responsible for handling input / output for the human player.
 * This includes inputs from physical devices and output to some kind
 * of display device.
 * This class is therefore never created on a dedicated server.
 */
class LLocalPlayer final
{
public:

    LLocalPlayer() = default;

    PROHIBIT_REALLOC_OF_ANY_FROM(LLocalPlayer)

    void Initialize();
    void Tick(const float DeltaTime);
    void OnLateTick(const float DeltaTime);
    void TearDown();

    FORCEINLINE auto IsPlayerInputValid() const -> bool { return this->PlayerInput != nullptr; }
    FORCEINLINE auto GetPlayerInput() const -> LPlayerInput* { return this->PlayerInput; }

    FORCEINLINE auto HasPrimarySurface() const -> bool { return this->SurfaceToDrawOn != nullptr; }
    FORCEINLINE auto GetPrimarySurface() const -> LSurface* { return this->SurfaceToDrawOn; }

    FORCEINLINE auto DoesPossess() const -> bool { return this->PlayerController != nullptr; }
    FORCEINLINE auto GetPossessed() const -> APlayerController* { return this->PlayerController; }
                auto Possess(APlayerController* InNewController) -> void;

private:

    LPlayerInput* PlayerInput     = nullptr;

    LHud*         Hud             = nullptr;
    LSurface*     SurfaceToDrawOn = nullptr;

    APlayerController* PlayerController = nullptr;
};

} /* ~Namespace Jafg. */
