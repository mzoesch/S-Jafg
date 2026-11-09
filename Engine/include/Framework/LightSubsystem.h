// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/WorldSubsystem.h"
#include "Rhi/Light.h"
#include "LightSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API JLightSubsystem : public JWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JLightSubsystem)

public:

    FORCEINLINE void RegisterLight(LLight const& Light) noexcept
    {
        check(Light.bCanEverAffectWorld)
        check(!algo::contains(this->Lights, &Light))
        this->Lights.emplace_back(&Light);
    }
    FORCEINLINE void UnregisterLight(LLight const& Light) noexcept
    {
        algo::erase_exactly_once_checked(&this->Lights, &Light);
    }

    FORCEINLINE constexpr TArray<LLight const*> const& GetLights() const noexcept
    {
        return this->Lights;
    }

private:

    // TODO: Replace with hive and make the relationship inverted.
    TArray<LLight const*> Lights;
};

} /* ~Namespace Jafg */
