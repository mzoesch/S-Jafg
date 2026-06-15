// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/World.h"
#include "WorldObject.generated.h"

namespace Jafg
{

class LWorld;

//# Initializer for the dynamic ctors for AWorldObjects.
typedef Detail::TCxxDynamicInit<LWorld> LWorldDynamicInit;
//# Initializer for the static ctors for AWorldObjects.
template<typename TCxxClass>
using TWorldStaticInit = Detail::TCxxStaticInitBase<LWorld, TCxxClass>;

namespace Detail
{

struct LOuter2WorldProj
{
    NODISCARD inline CONSTEXPR_CHECK LWorldDynamicInit operator()(LCxxDynamicInit const& Init) const noexcept
    {
        check(Init.Outer.GetUserData() == nullptr)
        return {.Outer=Init.Outer.AsWorld(),.Class=Init.Class};
    }
};

} /* ~Namespace Detail */

//#
//# An object that uses a world as its outer.
//# Additionally, defines utility functions to help with world and engine access.
//# If you want a simpler base class, use JCxxClass. There is nothing that prevents JCxxClass
//# classes to be spawned in worlds too.
//#
DECLARE_JAFG_CLASS(EJxxClassBits::Abstract)
class ENGINE_API AWorldObject : public JCxxClass
{
    GENERATED_CLASS_BODY()

protected:

    explicit AWorldObject(LWorldDynamicInit const& Init) noexcept
        : Super{LCxxDynamicInit{.Outer=Init.Outer,.Class=Init.Class}} {}
    template<typename TCxxClass>
    explicit AWorldObject(TWorldStaticInit<TCxxClass> const& Init) noexcept
        : Super{TCxxStaticInit<TCxxClass>{.Outer=Init.Outer}} {}

public:

    virtual void BeginLife() override
#if JAFG_DO_CHECKS
    ;
#else /* JAFG_DO_CHECKS */
    {}
#endif /* !JAFG_DO_CHECKS */

    FORCEINLINE LWorld& GetWorld() noexcept { return this->GetOuter().AsWorld(); }
    FORCEINLINE LWorld const& GetWorld() const noexcept { return this->GetOuter().AsWorld(); }

#if JAFG_WITH_EDITOR
    //#
    //# DisplayName is for editor purposes only. Do not query for it, try to find objects with the name.
    //# A display name can change at any time and might be localized ect.
    //#
    LString EditorName;
    NODISCARD FORCEINLINE LString const& GetEditorNameOrDefault() const noexcept
    {
        if (this->EditorName.empty())
        {
            return this->GetNameAsString();
        }
        return this->EditorName;
    }
#endif /* JAFG_WITH_EDITOR */
};

//# Do not use NewObject for AWorldObjects; instead use these.
inline constexpr Detail::NewDeferredObjectFn<LWorldDynamicInit, TWorldStaticInit, Detail::TDeferredObjectExec, AWorldObject> SpawnDeferredObject{};
inline constexpr Detail::NewObjectFn<decltype(SpawnDeferredObject), LWorldDynamicInit, TWorldStaticInit, AWorldObject> SpawnObject{SpawnDeferredObject};

} /* ~Namespace Jafg */
