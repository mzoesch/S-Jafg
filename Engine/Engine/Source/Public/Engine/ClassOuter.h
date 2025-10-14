// Copyright mzoesch. All rights reserved.

#pragma once

#include "CxxClass.h"
#include "Engine/CxxRecordTearDownReason.h"
#include "Foreign/PluginForward.h"

namespace Jafg
{

class LWorld;
class JCxxClass;

namespace Private
{

struct LClassOuterMiscellaneousAccessor;

} /* ~Namespace Private */

//#
//# Object context used to determine the context and lifetimes of reflected classes.
//#
class LClassOuter
{
    friend JCxxClass;
    friend Private::LClassOuterMiscellaneousAccessor;

public:

    inline LClassOuter() noexcept : HumanReadableName("<anonymous>") { }
    ENGINE_API explicit LClassOuter(LString HumanReadableName, bool bRegisterToEngine = true) noexcept;
    PROHIBIT_REALLOC_OF_ANY_FORM(LClassOuter)
    inline virtual ~LClassOuter() noexcept { this->TearDown(); }

    ENGINE_API void TearDown() noexcept;

#if LAL_DO_CHECKS
    ENGINE_API void CheckValidityForCDRStates() const noexcept;
#endif /* LAL_DO_CHECKS */

    FORCEINLINE void SetHumanReadableName(LString const& S) noexcept { this->HumanReadableName = S; }
    FORCEINLINE LString const& GetHumanReadableName() const noexcept { return this->HumanReadableName; }

    FORCEINLINE bool IsHiredHere(JCxxClass const* Employee) const noexcept { return algo::contains(this->Employees, Employee, &TUnique<JCxxClass>::get); }
    FORCEINLINE TArray<TUnique<JCxxClass>> const& GetEmployees() const noexcept { return this->Employees; }

    //# @return How many employees were hired and killed in this outer.
    LSize KillEmployeesFromForeignPlugin(
          LLoadedPluginHandle PluginHandle
        , ECxxRecordTearDownReason::Type Reason = ECxxRecordTearDownReason::PluginUnload
        );

    virtual bool IsWorld() const noexcept { return false; }
    FORCEINLINE LWorld const* AsWorld() const noexcept;
    FORCEINLINE LWorld* AsWorld() noexcept;

    FORCEINLINE TUnique<JCxxClass> Poach(JCxxClass* Employee) noexceptcheck
    {
        check( Employee )
        auto It{ algo::find(this->Employees, Employee, algo::unique_raw{}) };
        check( It != this->Employees.end() )

        TUnique Out{ std::move(*It) };
        check( *It == nullptr )
        check( Out.get() == Employee )
        this->Employees.erase(It);

        return Out;
    }

    FORCEINLINE TUnique<JCxxClass> PoachToNull(JCxxClass* Employee) noexceptcheck
    {
        check( Employee )
        auto It{ algo::find(this->Employees, Employee, algo::unique_raw{}) };
        check( It != this->Employees.end() )

        TUnique Out{ std::move(*It) };
        check( *It == nullptr )
        check( Out.get() == Employee )

        return Out;
    }

protected:

    virtual void OnTearDown() { }

private:

    void RegisterToEngine();
    void UnregisterFromEngine();

    LString HumanReadableName;
    TArray<TUnique<JCxxClass>> Employees;
    bool bWasRegisteredToEngine : 1 { false };
};

} /* ~Namespace Jafg */
