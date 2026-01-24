// Copyright mzoesch. All rights reserved.

#pragma once

#include "Misc/Tag.h"
#include "Async/TaskUtility.h"

namespace Jafg
{

#if JAFG_DO_CHECKS

template<typename TTag, typename TAllocator = TArray<LString>>
struct LMasterThreadTagRegistry : public Jafg::TTagRegistry<TTag, TAllocator>
{
    using Super = Jafg::TTagRegistry<TTag, TAllocator>;

    FORCEINLINE typename Super::TagType RegisterOrGet(auto&& InRepr) noexcept requires std::is_constructible_v<LStringView, decltype(InRepr)>
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

#else /* JAFG_DO_CHECKS */

template<typename TTag, typename TAllocator = TArray<LString>>
using LMasterThreadTagRegistry = Jafg::TTagRegistry<TTag, TAllocator>

#endif /* JAFG_DO_CHECKS */

} /* ~Namespace Jafg */
