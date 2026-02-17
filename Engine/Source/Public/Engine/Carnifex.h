// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Engine/CxxClass.h"

namespace Jafg
{

class JCxxClass;

//#
//# A carnifex is an object used to mascara all garbage children.
//# At the very end of every tick, it will look for them and kill them.
//#
class LCarnifex final
{
public:

    constexpr LCarnifex() noexcept = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LCarnifex)
    ~LCarnifex() = default;

    FORCEINLINE void AddGarbageChild(TUnique<JCxxClass> Child) noexcept { this->GarbageChildren.emplace_back(std::move(Child)); }

    ENGINE_API void KillAllGarbageChildren();
    ENGINE_API void DevourGarbageChildNow(TUnique<JCxxClass> Child);

    FORCEINLINE auto const& GetGarbageChildren() const noexcept { return this->GarbageChildren; }

private:

    TArray<TUnique<JCxxClass>> GarbageChildren;
};

} /* ~Namespace Jafg */
