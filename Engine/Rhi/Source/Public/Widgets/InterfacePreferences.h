// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
#include "Widgets/InterfaceTypes.h"
#include "InterfacePreferences.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class RHI_API JInterfacePreferences final : public Private::JObjectBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JInterfacePreferences)

public:

    /** A scale scalar that applies to the entire application. */
    float ApplicationScale = 1.0f;

private:

    const float BaseDpi = 96.0f;

    float CalculateDpiScale(const LViewportSize& InViewportSize) const;

    mutable TOptional<LViewportSize> LastViewportSize;
    mutable float CalculatedDpiScale = 1.0f;
};

} /* ~Namespace Jafg */
