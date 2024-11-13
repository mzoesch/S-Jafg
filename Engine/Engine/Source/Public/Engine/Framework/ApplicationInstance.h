// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

namespace Private
{

class LObjectContext;

} /* ~Namespace Private */

class LObjectClass;
class LEngine;
class JApplicationInstanceSubsystem;
struct LSubsystemCollection;

class LApplicationInstance final
{
    friend LEngine;

    LApplicationInstance() = default;
    PROHIBIT_REALLOC_OF_ANY_FROM(LApplicationInstance)
    ~LApplicationInstance() = default;

    void Initialize();
    void TearDown();

public:

    ENGINE_API auto GetEngine()       ->       LEngine*;
    ENGINE_API auto GetEngine() const -> const LEngine*;

    template <typename T>
    FORCEINLINE auto GetSubsystem() const -> JApplicationInstanceSubsystem* { return CheckedStaticCast<T, true>(this->GetSubsystem(T::StaticClass())); }
    template <typename T>
    FORCEINLINE auto GetCheckedSubsystem() const -> JApplicationInstanceSubsystem* { return CheckedStaticCast<T>(this->GetCheckedSubsystem(T::StaticClass())); }
    ENGINE_API auto GetSubsystem(const LObjectClass* InStaticClass)       ->       JApplicationInstanceSubsystem*;
    ENGINE_API auto GetSubsystem(const LObjectClass* InStaticClass) const -> const JApplicationInstanceSubsystem*;
    ENGINE_API auto GetCheckedSubsystem(const LObjectClass* InStaticClass)       ->       JApplicationInstanceSubsystem*;
    ENGINE_API auto GetCheckedSubsystem(const LObjectClass* InStaticClass) const -> const JApplicationInstanceSubsystem*;

private:

    Private::LObjectContext* Context      = nullptr;
    LSubsystemCollection*    Collection = nullptr;
};

} /* Namespace Jafg */
