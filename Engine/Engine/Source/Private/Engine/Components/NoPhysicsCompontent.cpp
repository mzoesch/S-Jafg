// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/Components/NoPhysicsCompontent.h"

Jafg::LNoPhysicsComponent* Jafg::LNoPhysicsComponent::GetUsableClass()
{
    static LNoPhysicsComponent Component;
    return &Component;
}
