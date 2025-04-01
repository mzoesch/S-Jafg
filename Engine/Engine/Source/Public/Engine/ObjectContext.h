// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

class JObjectBase;
class LEngine;
class LCarnifex;

namespace Private
{

struct LObjectMiscellaneousAccessor;

} /* ~Namespace Private */

enum EGlobalCarnifex { GlobalCarnifex };

//#
//# Object context that is used to determine the context and lifetimes of jafg objects.
//#
class ENGINE_API LObjectContext
{
    friend JObjectBase;
    friend LCarnifex;
    friend Private::LObjectMiscellaneousAccessor;

public:

    LObjectContext() noexcept = default;
    LObjectContext(EGlobalCarnifex);
    LObjectContext& operator=(EGlobalCarnifex);
    void DeferredInitialize(LCarnifex* InCarnifex) { check( this->Carnifex == nullptr ) this->Carnifex = InCarnifex; }
    PROHIBIT_REALLOC_OF_ANY_FORM(LObjectContext)
    virtual ~LObjectContext() { check( this->Carnifex == nullptr ) }

    FORCEINLINE bool IsValid() const { return this->IsCarnifexValid(); }

    virtual void TearDownContext();

    FORCEINLINE bool IsCarnifexValid() const { return this->Carnifex != nullptr; }
    FORCEINLINE auto GetCarnifex() const -> LCarnifex* { return this->Carnifex; }

    FORCEINLINE auto SetHumanReadableName(const LSimpleString& InS) -> void { check( this->IsValid() ) this->HumanReadableName = InS; }
    FORCEINLINE auto GetHumanReadableName() const -> const LSimpleString& { return this->HumanReadableName; }

private:

    LSimpleString HumanReadableName;

    //# The carnifex that is used to mascara all children within this context.
    LCarnifex* Carnifex = nullptr;

    //#
    //# The employees that are working within this context.
    //# If this context dies, the employees will kill themselves.
    //#
    TArray<JObjectBase*> Employees;
};

} /* ~Namespace Jafg */
