// Copyright mzoesch. All rights reserved.

#include "Svw/SvwSupremePolicies.h"
#include "Svw/JgcPawn.h"

Jgc::JSvwSupremePolicies::JSvwSupremePolicies(Jafg::LCxxObjectInitializer const& ObjectInitializer)
    : JSupremePolicies(ObjectInitializer)
{
    this->DefaultPawnClass = AJgcPawn::StaticClass();
}
