// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

class JObjectBase;

//#
//# A carnifex is an object used to mascara all garbage children.
//# At the very end of every tick, it will look for them and kill them.
//#
class ENGINE_API LCarnifex final
{
public:

    LCarnifex() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LCarnifex)
    ~LCarnifex() = default;

    FORCEINLINE void AddGarbageChild(JObjectBase* Child) { this->GarbageChildren.push_back(Child); }

    void KillAllGarbageChildren();
    void DevourGarbageChildNow(JObjectBase* Child);

private:

    void FreeChild(JObjectBase* Child);

    TArray<JObjectBase*> GarbageChildren;
};

} /* ~Namespace Jafg */
