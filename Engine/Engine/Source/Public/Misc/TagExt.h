// Copyright mzoesch. All rights reserved.

#pragma once

#include "Misc/Tag.h"
#include "Async/TaskUtility.h"

namespace Jafg
{

#if LAL_DO_CHECKS

template<typename TTag, typename TAllocator = TArray<LString>>
struct LMasterThreadTagRegistry : public Lal::TTagRegistry<TTag, TAllocator>
{
    using Super = Lal::TTagRegistry<TTag, TAllocator>;

    FORCEINLINE typename Super::TagType RegisterOrGet(Trait::CString auto&& InRepr) noexcept
    {
        check( !Tasks::HasMasterThread() || Tasks::IsOnMasterThread() )
        return Super::RegisterOrGet(std::forward<decltype(InRepr)>(InRepr));
    }
    template<LSize N>
    FORCEINLINE typename Super::TagType RegisterOrGet(const char(&InRepr)[N]) noexcept
    {
        return this->RegisterOrGet(LStringView{InRepr, N - 1});
    }
};
#else /* LAL_DO_CHECKS */

template<typename TTag, typename TAllocator = TArray<LString>>
using LMasterThreadTagRegistry = Lal::TTagRegistry<TTag, TAllocator>

#endif /* LAL_DO_CHECKS */

} /* ~Namespace Jafg */
