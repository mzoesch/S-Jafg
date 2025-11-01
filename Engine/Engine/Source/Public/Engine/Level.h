// Copyright mzoesch. All rights reserved.

#pragma once

#include "System/EnginePath.h"
#include "User/Input/InputMode.h"
#include "Engine/CxxClass.h"

namespace Jafg
{

class JSupremePolicies;

struct LLevelSkyboxMap final
{
    LString Identifier;
    TArray<LEnginePath> Textures;
    f32 DefaultLoad { 1.0f };
};

//#
//# Represents a level in the engine.
//# A level is a blueprint for a world.
//#
//# You can create your own levels by registering them with the engine.
//#
//# The core levels that are part of the engine's core are:
//#   - LFrontEnd (Client only. This is the front-end of the client window.)
//#   - LWorld (The world.)
//#
struct LLevel final
{
    LLevel() = default;

    explicit LLevel
    (
        LString InIdentifier,
        const EInputMode::Type InInputMode = EInputMode::None,
        const bool bInShowMouseCursor = true,
        Lal::LLinearColor const& BackgroundColor = Lal::LLinearColor::Black,
        const bool bInCreateSkybox = false,
        const bool bInDrawSkyboxFirst = true,
        TArray<LLevelSkyboxMap> const& InSkybox = {},
        const TSubclassOf<JSupremePolicies> InSupremePoliciesClass = {}
    )
        : Identifier(std::move(InIdentifier))
        , InputMode(InInputMode)
        , bShowMouseCursor(bInShowMouseCursor)
        , BackgroundColor(BackgroundColor)
        , bCreateSkybox(bInCreateSkybox)
        , bDrawSkyboxFirst(bInDrawSkyboxFirst)
        , Skybox(InSkybox)
        , SupremePoliciesClass(InSupremePoliciesClass)
    {
        return;
    }

    DEFAULT_REALLOC_OF_ANY_FORM(LLevel)

    FORCEINLINE bool operator==(const LLevel&  Other) const noexcept { return this->Identifier == Other.Identifier; }

    LString Identifier;

    EInputMode::Type InputMode;
    bool bShowMouseCursor;

    Lal::LLinearColor BackgroundColor;
    bool bCreateSkybox;
    bool bDrawSkyboxFirst;
    TArray<LLevelSkyboxMap> Skybox;

    TSubclassOf<JSupremePolicies> SupremePoliciesClass;
};

} /* Namespace Jafg */
