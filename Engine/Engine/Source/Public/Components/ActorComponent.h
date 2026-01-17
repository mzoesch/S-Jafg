// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/CxxClass.h"
#include "ActorComponent.generated.h"

namespace Jafg
{

class AActor;
struct LRenderInfo;

DECLARE_JAFG_CLASS()
class ENGINE_API JActorComponent : public JCxxClass
{
    GENERATED_CLASS_BODY()

    friend AActor;

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JActorComponent)

public:

    virtual void OnAttach(AActor* InOwner)
    {
        check( InOwner )

#if LAL_DO_CHECKS
        check( this->bHasExecutedOnAttach == false )
        this->bHasExecutedOnAttach = true;
#endif /* LAL_DO_CHECKS */

        this->Owner = InOwner;

        return;
    }

    constexpr void SetShouldRender(bool b) noexcept { this->bRender = b; }
    constexpr bool ShouldRender() const noexcept { return this->bRender; }
    virtual void Render(LRenderInfo const& Info) noexcept {}

protected:

    FORCEINLINE AActor* GetOwner() const noexcept { check( this->Owner ) return this->Owner; }

private:

#if LAL_DO_CHECKS
    bool bHasExecutedOnAttach{};
#endif /* LAL_DO_CHECKS */

    AActor* Owner{};
    bool bRender:1{};
};

} /* ~Namespace Jafg */
