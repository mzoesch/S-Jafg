// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

namespace Private
{

class JObjectBase;

} /* ~Namespace Private */

/**
 * A carnifex is an object used to mascara all garbage children.
 * At the very end of every tick, it will look for them and kill them.
 */
class ENGINEFRAMEWORK_API LCarnifex final
{
public:

    LCarnifex() = default;
    PROHIBIT_REALLOC_OF_ANY_FROM(LCarnifex)
    ~LCarnifex() = default;

    FORCEINLINE void AddGarbageChild(Private::JObjectBase* Child) { this->GarbageChildren.Add(Child); }

    void KillAllGarbageChildren();
    void DevourGarbageChildNow(Private::JObjectBase* Child);

private:

    void FreeChild(Private::JObjectBase* Child);

    TdhArray<Private::JObjectBase*> GarbageChildren;
};

} /* ~Namespace Jafg */
