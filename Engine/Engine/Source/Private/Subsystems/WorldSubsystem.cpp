// Copyright mzoesch. All rights reserved.

#include "Subsystems/WorldSubsystem.h"
#include "Core/CoreNames.h"
#include "Engine/World.h"

bool Jafg::JWorldSubsystem::IsOuterFrontend(LClassOuter const* Outer) noexcept
{
    return Outer && Outer->IsWorld() && Outer->AsWorld()->GetUnderlyingLevelName() == "LevelFrontend";
}

bool Jafg::JWorldSubsystem::IsOuterWorld(LClassOuter const* Outer) noexcept
{
    return Outer && Outer->IsWorld() && Outer->AsWorld()->GetUnderlyingLevelName() == Name_LevelMyWorld.ToString();
}
