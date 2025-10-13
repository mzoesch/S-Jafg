// Copyright mzoesch. All rights reserved.

#pragma once

namespace Trait
{

//#
//# Inherit from this to declare a new trait.
//#
struct Base { };

} /* Namespace Trait */

namespace Traits
{

template <typename T>
concept CTrait = std::is_base_of_v<Trait::Base, T>;

template <CTrait Trait, typename T>
struct Implements_t : std::false_type
{
};

//#
//# Use this to check if a type implements a trait.
//#
template <CTrait Trait, typename T>
inline constexpr bool Implements = Implements_t<Trait, T>::value;

} /* ~Namespace Traits */

#define DECLARE_TRAIT(_TraitName)                                     \
    namespace Trait                                                   \
    {                                                                 \
        struct _TraitName : public Trait::Base { };                   \
        template <typename T>                                         \
        concept LAL_JOIN_OUTER_TWO(C, _TraitName) =                   \
            ::Traits::Implements<_TraitName, std::remove_cvref_t<T>>; \
    } /* ~Namespace Trait */

#define IMPLEMENT_TRAIT(_TraitName, _Type)                                \
    namespace Traits                                                      \
    {                                                                     \
        template <>                                                       \
        struct Implements_t<                                              \
            LAL_JOIN_SCOPE_RESOLUTION_OUTER_TWO(Trait, _TraitName),       \
            _Type> : std::true_type { };                                  \
    } /* ~Namespace Traits */

#define ACCEPT_TRAIT_TEMPLATE(_TraitName, _Type, ...)                     \
    namespace Traits                                                      \
    {                                                                     \
        template <__VA_ARGS__>                                            \
        struct Implements_t<                                              \
            LAL_JOIN_SCOPE_RESOLUTION_OUTER_TWO(Trait, _TraitName),       \
            _Type<__VA_ARGS__> : std::true_type { };                      \
    } /* ~Namespace Traits */
