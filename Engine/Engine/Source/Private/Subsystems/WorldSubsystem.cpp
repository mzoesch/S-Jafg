// Copyright mzoesch. All rights reserved.

#include "Subsystems/WorldSubsystem.h"
#include "Core/CoreNames.h"
#include "Engine/World.h"

bool Jafg::JWorldSubsystem::IsOuterFrontend(const LObjectContext* InOuter) noexcept
{
    return InOuter && InOuter->IsWorld() && static_cast<const LWorld*>(InOuter)->GetUnderlyingLevelName() == Name_LevelFrontend.ToString().c_str();
}

bool Jafg::JWorldSubsystem::IsOuterWorld(const LObjectContext* InOuter) noexcept
{
    return InOuter && InOuter->IsWorld() && static_cast<const LWorld*>(InOuter)->GetUnderlyingLevelName() == Name_LevelMyWorld.ToString().c_str();
}
