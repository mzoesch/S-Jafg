// Copyright mzoesch. All rights reserved.

#include "Jgc/Owss/OwssSubsystem.h"
#include "Engine/World.h"
#include "Framework/SupremePolicies.h"
#include "Jgc/Owss/OwssSupremePolicies.h"

bool Jgc::JOwssSubsystem::ShouldCreateSubsystem() const noexcept
{
    if (!Super::ShouldCreateSubsystem())
    {
        return false;
    }

    return this->GetWorld().GetSupremePoliciesChecked()->IsA<AOwssSupremePolicies>();
}
