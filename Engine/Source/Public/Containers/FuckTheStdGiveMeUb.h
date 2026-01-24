// Copyright mzoesch. All rights reserved.

#pragma once
#include <vector>

namespace Jafg
{

namespace HellYeaThisIsAsFuckedUpAsItCanBeLol
{

template<typename T>
class TAccessProtectedMembers : public std::vector<T>
{
    UTILITY_CLASS(TAccessProtectedMembers)

public:

    NODISCARD
    consteval
    static typename std::vector<T>::size_type GetOffsetOfBegin() noexcept
    {
        return offsetof(TAccessProtectedMembers, _M_impl._M_start);
    }

    NODISCARD
    consteval
    static typename std::vector<T>::size_type GetOffsetOfSlack() noexcept
    {
        return offsetof(TAccessProtectedMembers, _M_impl._M_finish);
    }

    NODISCARD
    consteval
    static typename std::vector<T>::size_type GetOffsetOfEnd() noexcept
    {
        return offsetof(TAccessProtectedMembers, _M_impl._M_end_of_storage);
    }
};

} /* HellYeaThisIsAsFuckedUpAsItCanBeLol */

template <typename TContainer>
void AddUninitialized(TContainer* Container, const typename TContainer::size_type Count = 1) noexcept
{
#if JAFG_DO_CHECKS
    typename TContainer::size_type OldSize{ Container->size() };
#endif /* JAFG_DO_CHECKS */

    Container->reserve(Container->size() + Count);

    typename TContainer::pointer& Slack =
        *reinterpret_cast<typename TContainer::pointer*>
        (
            reinterpret_cast<u8*>(Container) + HellYeaThisIsAsFuckedUpAsItCanBeLol::TAccessProtectedMembers<typename TContainer::value_type>::GetOffsetOfSlack()
        );

    JAFG_FWD_CHECK([Slack]{return Slack != nullptr;})
    JAFG_FWD_CHECK([Container, Slack]{return static_cast<void const*>(std::to_address(Container->end())) == static_cast<void const*>(Slack);})

    Slack += Count;

#if JAFG_DO_CHECKS
    JAFG_FWD_CHECK([Container, OldSize, Count]{return Container->size() == OldSize + Count;})
    JAFG_FWD_CHECK([Container]{return Container->size() <= Container->capacity();})
#endif /* JAFG_DO_CHECKS */

    return;
}

} /* ~Namespace Jafg */
