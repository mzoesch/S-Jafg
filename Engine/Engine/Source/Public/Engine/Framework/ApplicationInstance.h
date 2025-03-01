// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
#include "Engine/ObjectContext.h"
#include "Subsystems/SubsystemCollection.h"

namespace Jafg
{

class LObjectClass;
class LEngine;
class JApplicationInstanceSubsystem;

class LApplicationInstance final
{
    friend LEngine;

    LApplicationInstance() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LApplicationInstance)
    ~LApplicationInstance() = default;

    void Initialize();
    void TearDown();

public:

    ENGINE_API auto GetEngine()       ->       LEngine*;
    ENGINE_API auto GetEngine() const -> const LEngine*;

    template <typename T>
    FORCEINLINE auto GetSubsystem() -> T*
    {
        return CheckedStaticCast<T, std::remove_pointer_t<decltype(this->GetSubsystem(T::StaticClass()))>, true>(this->GetSubsystem(T::StaticClass()));
    }
    template <typename T>
    FORCEINLINE auto GetSubsystem() const -> const T*
    {
        return CheckedStaticCast<T, std::remove_pointer_t<decltype(this->GetSubsystem(T::StaticClass()))>, true>(this->GetSubsystem(T::StaticClass()));
    }
    template <typename T>
    FORCEINLINE auto GetCheckedSubsystem() -> T* { return CheckedStaticCast<T>(this->GetCheckedSubsystem(T::StaticClass())); }
    template <typename T>
    FORCEINLINE auto GetCheckedSubsystem() const -> const T* { return CheckedStaticCast<T>(this->GetCheckedSubsystem(T::StaticClass())); }

    FORCEINLINE auto GetSubsystem(const LObjectClass* InStaticClass) -> JApplicationInstanceSubsystem* { return this->Collection.GetSubsystem<JApplicationInstanceSubsystem>(InStaticClass); }
    FORCEINLINE auto GetSubsystem(const LObjectClass* InStaticClass) const -> const JApplicationInstanceSubsystem* { return this->Collection.GetSubsystem<JApplicationInstanceSubsystem>(InStaticClass); }
    FORCEINLINE auto GetCheckedSubsystem(const LObjectClass* InStaticClass) -> JApplicationInstanceSubsystem* { return this->Collection.GetCheckedSubsystem<JApplicationInstanceSubsystem>(InStaticClass); }
    FORCEINLINE auto GetCheckedSubsystem(const LObjectClass* InStaticClass) const -> const JApplicationInstanceSubsystem* { return this->Collection.GetCheckedSubsystem<JApplicationInstanceSubsystem>(InStaticClass); }

private:

    LObjectContext       Context;
    LSubsystemCollection Collection;
};

} /* ~Namespace Jafg */
