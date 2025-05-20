// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Foreign/PluginForward.h"

namespace Jafg
{

class JObjectBase;
class LEngine;
class LCarnifex;
struct LPrivateLaunch;

namespace Private
{

struct LObjectMiscellaneousAccessor;

} /* ~Namespace Private */

enum EGlobalCarnifex { GlobalCarnifex };
enum EDeferredGlobalCarnifex { DeferredGlobalCarnifex };

//#
//# Object context that is used to determine the context and lifetimes of jafg objects.
//#
class ENGINE_API LObjectContext
{
    friend JObjectBase;
    friend LCarnifex;
    friend Private::LObjectMiscellaneousAccessor;
    friend LPrivateLaunch;

public:

    LObjectContext() noexcept = default;
    LObjectContext(EGlobalCarnifex);
    LObjectContext(EDeferredGlobalCarnifex);
    LObjectContext& operator=(EGlobalCarnifex);
    void DeferredInitialize(LCarnifex* InCarnifex);

    PROHIBIT_REALLOC_OF_ANY_FORM(LObjectContext)

    virtual ~LObjectContext() { check( this->IsValid() == false ) }

    FORCEINLINE bool IsValid() const { return this->IsCarnifexValid(); }

    virtual void TearDownContext();

    virtual bool IsWorld() const { return false; }

    FORCEINLINE bool IsCarnifexValid() const { return this->Carnifex != nullptr; }
    FORCEINLINE auto GetCarnifex() const -> LCarnifex* { return this->Carnifex; }

    FORCEINLINE auto SetHumanReadableName(const LString& InS) -> void { check( this->IsValid() ) this->HumanReadableName = InS; }
    FORCEINLINE auto GetHumanReadableName() const -> const LString& { return this->HumanReadableName; }

    FORCEINLINE bool IsHiredHere(const JObjectBase* InObject) const { return InObject && this->Employees.Contains(InObject); }

    void SeparateAndKillEmployees(const LLoadedPluginHandle InPluginHandle);

private:

    enum ENoEngineRegistration { NoEngineRegistration = 9009, };
    explicit LObjectContext(const ENoEngineRegistration InValue);
    explicit LObjectContext(const ENoEngineRegistration InValue, LCarnifex* InCarnifex);

    void TearDownContextNoEngineUnregistration();

    void InitializeWithGlobal();
    void Initialize(LCarnifex* InCarnifex, const bool bRegister = true);
    void RegisterToEngine();
    void UnregisterFromEngine();

    LString HumanReadableName;

    //# The carnifex that is used to mascara all children within this context.
    LCarnifex* Carnifex { nullptr };

    //#
    //# The employees that are working within this context.
    //# If this context dies, the employees will kill themselves.
    //#
    TArray<JObjectBase*> Employees;
};

} /* ~Namespace Jafg */
