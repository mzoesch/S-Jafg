// Copyright mzoesch. All rights reserved.

#include "Components/NoPhysicsCompontent.h"

Jafg::LNoPhysicsComponent* Jafg::LNoPhysicsComponent::GetUsableClass()
{
    static LNoPhysicsComponent Component;
    return &Component;
}
