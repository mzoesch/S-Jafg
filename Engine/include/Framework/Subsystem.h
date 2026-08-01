// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Jxx.h"
#include "Subsystem.generated.h"

namespace Jafg
{

struct LSubsystemCollection;

//#
//# A subsystem that shares its lifetime with their owning object.
//#
//# To create your own subsystem lifetime:
//#
DECLARE_JAFG_CLASS(EJxxClassBits::Abstract)
class ENGINE_API JSubsystem : public JCxxClass
{
    friend LSubsystemCollection;

    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JSubsystem)
    ENGINE_API ~JSubsystem() override;

    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override final
    {
        Super::OnGarbage(Reason);
        check(this->bRequestingDependency == false)

        if (this->IsInitialized())
        {
            this->TearDown();
        }

        return;
    }

    //# Weather the subsystem should be created.
    inline virtual bool ShouldCreateSubsystem() const noexcept { return true; }

    //# Initialize the subsystem. Only called if #ShouldCreateSubsystem returns true.
    inline virtual void Initialize(LSubsystemCollection& Collection)
    {
        check(Tasks::IsOnMasterThread() && this->bIsInitialized == false)
        this->bIsInitialized = true;
    }

    inline virtual void TearDown() {}

    FORCEINLINE constexpr bool IsInitialized() const noexcept { return this->bIsInitialized; }
    FORCEINLINE constexpr bool IsPriorityTearDown() const noexcept { return this->bPriorityTearDown; }
    //# Please see the #bPriorityTearDown documentation for more information. DO NOT JUST SET THIS TO TRUE.
    FORCEINLINE constexpr void SetPriorityTearDown(bool b) noexcept { this->bPriorityTearDown = b; }

private:

    //#
    //# This flag is generally only for subsystems that have very special rules and states attached to them.
    //# In general, this **ONLY** applies to subsystems that are threaded.
    //# If you see yourself setting this flag on a NON-THREADED subsystem, please reconsider your design choice.
    //#
    bool bPriorityTearDown:1{};

    //# Whether #ShouldCreateSubsystem returned true and the subsystem was, therefore, initialized.
    bool bIsInitialized:1{};

#if JAFG_DO_CHECKS
    bool bRequestingDependency:1{};
#endif /* JAFG_DO_CHECKS */
};

} /* Namespace Jafg */

//# Include for children as they will need this always.
#include "Framework/SubsystemCollection.h"
