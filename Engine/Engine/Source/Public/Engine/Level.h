// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Input/InputMode.h"
#include "Engine/CxxClass.h"

namespace Jafg
{

class JSupremePolicies;

//#
//# Represents a level in the engine.
//# A level is a blueprint for a world.
//#
//# You can create your own levels by registering them with the engine.
//#
struct LLevel final
{
    LString Identifier;
    EInputMode::Type InputMode{ EInputMode::None | EInputMode::ShowMouseCursor };
    TSubclassOf<JSupremePolicies> SupremePoliciesClass;
};

} /* Namespace Jafg */
