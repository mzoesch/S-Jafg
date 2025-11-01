// Copyright mzoesch. All rights reserved.

#include "Svw/SvwSupremePolicies.h"
#include "Svw/SvwPawn.h"

Jgc::JSvwSupremePolicies::JSvwSupremePolicies(Jafg::LCxxObjectInitializer const& ObjectInitializer)
    : Jafg::JSupremePolicies(ObjectInitializer)
{
    this->DefaultPawnClass = ASvwPawn::StaticClass();
}
