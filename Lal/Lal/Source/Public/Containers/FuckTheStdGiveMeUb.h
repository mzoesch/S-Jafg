// Copyright mzoesch. All rights reserved.

#pragma once
#include <vector>

namespace Lal
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
void AddUninitialized(TContainer* Container, const typename TContainer::size_type Count)
{
#if LAL_DO_CHECKS
    typename TContainer::size_type OldSize{ Container->size() };
#endif /* LAL_DO_CHECKS */

    Container->reserve(Container->size() + Count);

    typename TContainer::pointer& Slack =
        *reinterpret_cast<typename TContainer::pointer*>
        (
            reinterpret_cast<u8*>(Container) + HellYeaThisIsAsFuckedUpAsItCanBeLol::TAccessProtectedMembers<typename TContainer::value_type>::GetOffsetOfSlack()
        );

    LAL_FWD_CHECK([Slack]{return Slack != nullptr;})
    LAL_FWD_CHECK([Container, Slack]{return static_cast<void const*>(std::to_address(Container->end())) == static_cast<void const*>(Slack);})

    Slack += Count;

#if LAL_DO_CHECKS
    LAL_FWD_CHECK([Container, OldSize, Count]{return Container->size() == OldSize + Count;})
    LAL_FWD_CHECK([Container]{return Container->size() <= Container->capacity();})
#endif /* LAL_DO_CHECKS */

    return;
}

} /* ~Namespace Lal */
