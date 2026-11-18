// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Actor.h"
#include "AvatarStart.generated.h"

namespace Jafg
{

//# Where the avatars that are spawned for persona controllers start. @see #ASupremePolicies::GetAvatarStartFor
DECLARE_JAFG_CLASS()
class ENGINE_API AAvatarStart: public AActor
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_EXTERNAL_BODY(AAvatarStart)

public:

    //# @param Avatar The actor of the avatar that wants to start here.
    virtual bool CanAccept(AActor const& Avatar) const
    {
        //# TODO: To some collision checking as default maybe?
        (void)Avatar;
        return true;
    }

    TArray<LString> Tags;
};

} /* ~Namespace Jafg */
