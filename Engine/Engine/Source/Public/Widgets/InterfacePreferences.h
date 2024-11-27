// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
#include "InterfacePreferences.generated.h"

namespace Jafg
{

class LViewport;

DECLARE_JAFG_CLASS(EClassFlags::Singleton,)
class ENGINE_API JInterfacePreferences final : public Private::JObjectBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JInterfacePreferences)

public:

    /** A scale scalar that applies to the entire application. */
    float ApplicationScale = 1.0f;
};

} /* ~Namespace Jafg */
