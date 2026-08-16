// Copyright mzoesch. All rights reserved.

#include "Framework/WorldObject.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

#if JAFG_DO_CHECKS
void Jafg::AWorldObject::BeginLife()
{
    Super::BeginLife();

    check(this->GetOuter().IsWorld())
    checkCode
    (
        bool bFound{};
        for (auto const& Track : GEngine->GetTracks())
        {
            if (Track.ChildWorld.get() == &this->GetOuter())
            {
                check(bFound == false)
                bFound = true;
            }
            continue;
        }
        check(bFound)
    )

    return;
}
#endif /* JAFG_DO_CHECKS */

Jafg::AWorldObject& Jafg::AWorldObject::CloneEntry() const noexcept
{
    return this->CloneImpl(SpawnDeferredObject(LWorldDynamicInit{.Outer=this->GetMutableOuter().AsWorld(),.Class=this->GetVirtualTable()}).release());
}

Jafg::AWorldObject& Jafg::AWorldObject::CloneImpl(AWorldObject* Object) const noexcept
{
    LOG_VERBOSE(LogJxx, "[{}]: Cloning.", this->GetVirtualTable().GetFullyQualifiedName())

    check(Object)
    check(!Object->_HasBegunLife())
    check(&this->GetVirtualTable() == &Object->GetVirtualTable())

    for (auto& Field: Object->GetVirtualTable().FieldIter())
    {
        LOG_TRACE(LogJxx, "Cloning field [{}].", Field.Identifier)
        if (!(Field.Flags & EJxxFieldBits::Transient))
        {
            Field.FastClone(*this, &*Object);
        }
    }

    return *Object;
}
