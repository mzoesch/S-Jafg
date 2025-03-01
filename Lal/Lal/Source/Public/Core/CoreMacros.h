// Copyright mzoesch. All rights reserved.

#pragma once

#define PRIVATE_JAFG_CORE_COMMA                                         ,

#define PRIVATE_JAFG_CORE_JOIN_INNER_TWO(   A, B                      ) A##B
#define PRIVATE_JAFG_CORE_JOIN_INNER_THREE( A, B, C                   ) A##B##C
#define PRIVATE_JAFG_CORE_JOIN_INNER_FOUR(  A, B, C, D                ) A##B##C##D
#define PRIVATE_JAFG_CORE_JOIN_INNER_FIVE(  A, B, C, D, E             ) A##B##C##D##E
#define PRIVATE_JAFG_CORE_JOIN_INNER_SIX(   A, B, C, D, E, F          ) A##B##C##D##E##F
#define PRIVATE_JAFG_CORE_JOIN_INNER_SEVEN( A, B, C, D, E, F, G       ) A##B##C##D##E##F##G
#define PRIVATE_JAFG_CORE_JOIN_INNER_EIGHT( A, B, C, D, E, F, G, H    ) A##B##C##D##E##F##G##H
#define PRIVATE_JAFG_CORE_JOIN_INNER_NINE(  A, B, C, D, E, F, G, H, I ) A##B##C##D##E##F##G##H##I

#define PRIVATE_JAFG_CORE_JOIN_OUTER_TWO(   A, B                      ) PRIVATE_JAFG_CORE_JOIN_INNER_TWO(   A, B                      )
#define PRIVATE_JAFG_CORE_JOIN_OUTER_THREE( A, B, C                   ) PRIVATE_JAFG_CORE_JOIN_INNER_THREE( A, B, C                   )
#define PRIVATE_JAFG_CORE_JOIN_OUTER_FOUR(  A, B, C, D                ) PRIVATE_JAFG_CORE_JOIN_INNER_FOUR(  A, B, C, D                )
#define PRIVATE_JAFG_CORE_JOIN_OUTER_FIVE(  A, B, C, D, E             ) PRIVATE_JAFG_CORE_JOIN_INNER_FIVE(  A, B, C, D, E             )
#define PRIVATE_JAFG_CORE_JOIN_OUTER_SIX(   A, B, C, D, E, F          ) PRIVATE_JAFG_CORE_JOIN_INNER_SIX(   A, B, C, D, E, F          )
#define PRIVATE_JAFG_CORE_JOIN_OUTER_SEVEN( A, B, C, D, E, F, G       ) PRIVATE_JAFG_CORE_JOIN_INNER_SEVEN( A, B, C, D, E, F, G       )
#define PRIVATE_JAFG_CORE_JOIN_OUTER_EIGHT( A, B, C, D, E, F, G, H    ) PRIVATE_JAFG_CORE_JOIN_INNER_EIGHT( A, B, C, D, E, F, G, H    )
#define PRIVATE_JAFG_CORE_JOIN_OUTER_NINE(  A, B, C, D, E, F, G, H, I ) PRIVATE_JAFG_CORE_JOIN_INNER_NINE(  A, B, C, D, E, F, G, H, I )

#define PRIVATE_JAFG_CORE_CAT_INNER_TWO(   A, B                      ) A B
#define PRIVATE_JAFG_CORE_CAT_INNER_THREE( A, B, C                   ) A B C
#define PRIVATE_JAFG_CORE_CAT_INNER_FOUR(  A, B, C, D                ) A B C D
#define PRIVATE_JAFG_CORE_CAT_INNER_FIVE(  A, B, C, D, E             ) A B C D E
#define PRIVATE_JAFG_CORE_CAT_INNER_SIX(   A, B, C, D, E, F          ) A B C D E F
#define PRIVATE_JAFG_CORE_CAT_INNER_SEVEN( A, B, C, D, E, F, G       ) A B C D E F G
#define PRIVATE_JAFG_CORE_CAT_INNER_EIGHT( A, B, C, D, E, F, G, H    ) A B C D E F G H
#define PRIVATE_JAFG_CORE_CAT_INNER_NINE(  A, B, C, D, E, F, G, H, I ) A B C D E F G H I

#define PRIVATE_JAFG_CORE_CAT_OUTER_TWO(   A, B                      ) PRIVATE_JAFG_CORE_CAT_INNER_TWO(   A, B                      )
#define PRIVATE_JAFG_CORE_CAT_OUTER_THREE( A, B, C                   ) PRIVATE_JAFG_CORE_CAT_INNER_THREE( A, B, C                   )
#define PRIVATE_JAFG_CORE_CAT_OUTER_FOUR(  A, B, C, D                ) PRIVATE_JAFG_CORE_CAT_INNER_FOUR(  A, B, C, D                )
#define PRIVATE_JAFG_CORE_CAT_OUTER_FIVE(  A, B, C, D, E             ) PRIVATE_JAFG_CORE_CAT_INNER_FIVE(  A, B, C, D, E             )
#define PRIVATE_JAFG_CORE_CAT_OUTER_SIX(   A, B, C, D, E, F          ) PRIVATE_JAFG_CORE_CAT_INNER_SIX(   A, B, C, D, E, F          )
#define PRIVATE_JAFG_CORE_CAT_OUTER_SEVEN( A, B, C, D, E, F, G       ) PRIVATE_JAFG_CORE_CAT_INNER_SEVEN( A, B, C, D, E, F, G       )
#define PRIVATE_JAFG_CORE_CAT_OUTER_EIGHT( A, B, C, D, E, F, G, H    ) PRIVATE_JAFG_CORE_CAT_INNER_EIGHT( A, B, C, D, E, F, G, H    )
#define PRIVATE_JAFG_CORE_CAT_OUTER_NINE(  A, B, C, D, E, F, G, H, I ) PRIVATE_JAFG_CORE_CAT_INNER_NINE(  A, B, C, D, E, F, G, H, I )

/**
 * We cannot concat :: in GCC, because it is too strict for that kind of operation and actually thinks that :: is a
 * fucking operator. In the goodlike MSVC world, we can do that without any problems. Why tf.
 */
#define JAFG_CORE_JOIN_SCOPE_RESOLUTION_INNER_TWO(   A, B                      ) A::B
#define JAFG_CORE_JOIN_SCOPE_RESOLUTION_INNER_THREE( A, B, C                   ) A::B::C
#define JAFG_CORE_JOIN_SCOPE_RESOLUTION_INNER_FOUR(  A, B, C, D                ) A::B::C::D
#define JAFG_CORE_JOIN_SCOPE_RESOLUTION_INNER_FIVE(  A, B, C, D, E             ) A::B::C::D::E
#define JAFG_CORE_JOIN_SCOPE_RESOLUTION_INNER_SIX(   A, B, C, D, E, F          ) A::B::C::D::E::F
#define JAFG_CORE_JOIN_SCOPE_RESOLUTION_INNER_SEVEN( A, B, C, D, E, F, G       ) A::B::C::D::E::F::G
#define JAFG_CORE_JOIN_SCOPE_RESOLUTION_INNER_EIGHT( A, B, C, D, E, F, G, H    ) A::B::C::D::E::F::G::H
#define JAFG_CORE_JOIN_SCOPE_RESOLUTION_INNER_NINE(  A, B, C, D, E, F, G, H, I ) A::B::C::D::E::F::G::H::I

#define JAFG_CORE_JOIN_SCOPE_RESOLUTION_OUTER_TWO(   A, B                      ) JAFG_CORE_JOIN_SCOPE_RESOLUTION_INNER_TWO(   A, B                      )
#define JAFG_CORE_JOIN_SCOPE_RESOLUTION_OUTER_THREE( A, B, C                   ) JAFG_CORE_JOIN_SCOPE_RESOLUTION_INNER_THREE( A, B, C                   )
#define JAFG_CORE_JOIN_SCOPE_RESOLUTION_OUTER_FOUR(  A, B, C, D                ) JAFG_CORE_JOIN_SCOPE_RESOLUTION_INNER_FOUR(  A, B, C, D                )
#define JAFG_CORE_JOIN_SCOPE_RESOLUTION_OUTER_FIVE(  A, B, C, D, E             ) JAFG_CORE_JOIN_SCOPE_RESOLUTION_INNER_FIVE(  A, B, C, D, E             )
#define JAFG_CORE_JOIN_SCOPE_RESOLUTION_OUTER_SIX(   A, B, C, D, E, F          ) JAFG_CORE_JOIN_SCOPE_RESOLUTION_INNER_SIX(   A, B, C, D, E, F          )
#define JAFG_CORE_JOIN_SCOPE_RESOLUTION_OUTER_SEVEN( A, B, C, D, E, F, G       ) JAFG_CORE_JOIN_SCOPE_RESOLUTION_INNER_SEVEN( A, B, C, D, E, F, G       )
#define JAFG_CORE_JOIN_SCOPE_RESOLUTION_OUTER_EIGHT( A, B, C, D, E, F, G, H    ) JAFG_CORE_JOIN_SCOPE_RESOLUTION_INNER_EIGHT( A, B, C, D, E, F, G, H    )
#define JAFG_CORE_JOIN_SCOPE_RESOLUTION_OUTER_NINE(  A, B, C, D, E, F, G, H, I ) JAFG_CORE_JOIN_SCOPE_RESOLUTION_INNER_NINE(  A, B, C, D, E, F, G, H, I )

/**
 * Use this macro for formatting raw string literals.
 * @remark MSVC is by far more permissive, so it supports this feature natively as they always decay raw string
 *         literals (const char[n]) as const char pointers. While GCC and Clang are more strict (in terms of the C++
 *         type system) and they do not allow this kind of implicit conversion - therefore, we have to explicitly cast
 *         the literal to our formatter.
 */
#if WITH_MSVC
    #define FMT(X)      X
#elif WITH_GCC
    #define FMT(X)      static_cast<const char*>(X)
#else /* WITH_GCC */
    #error "Missing implementation for this platform."
#endif /* !WITH_GCC */

/**
 * Advice the preprocessor to exclude the following code without the interactive intellisense mocking at us
 * that we excluded content of a condition that is always false.
 */
#define PREPROCESSOR_EXCLUDE_FF             0

/** A struct must always be at least one byte in size. */
#define UNREACHABLE_BYTE_SIZE_FOR_STRUCT    0x00
#define UNREACHABLE_BYTE_SIZE_FOR_CLASS     0x00
#define UNREACHABLE_BYTE_SIZE_FOR_TYPE      0x00

/**
 * Causes a compile time error if this specialization of a templated function is used.
 */
#if WITH_MSVC
    #define UNSUPPORTED_TEMPLATED_SPECIALIZATION(Ty, ...)                                                           \
        {                                                                                                           \
            static_assert(sizeof(Ty) == UNREACHABLE_BYTE_SIZE_FOR_TYPE, "Templated specialization not supported."); \
            ##__VA_ARGS__;                                                                                          \
        }
#elif WITH_GCC
    #define UNSUPPORTED_TEMPLATED_SPECIALIZATION(Ty, ...)                                                           \
        {                                                                                                           \
            static_assert(sizeof(Ty) == UNREACHABLE_BYTE_SIZE_FOR_TYPE, "Templated specialization not supported."); \
            __VA_ARGS__;                                                                                            \
        }
#else
    #error "Missing implementation for the current compiler."
#endif /* WITH_GCC */

#define UNREACHABLE_CONTROL_PATH_STATIC(Cond) static_assert((Cond), "Unreachable control path.");

/** Prohibits copying and moving of a specific type T. */
#define PROHIBIT_REALLOC_OF_ANY_FORM(Ty) \
    PROHIBIT_COPY(Ty)                    \
    PROHIBIT_MOVE(Ty)

#define PROHIBIT_REALLOC_OF_ANY_FORM_NAMESPACED(TSpacedTy, Ty) \
    PROHIBIT_COPY_NAMESPACED(TSpacedTy, Ty)                    \
    PROHIBIT_MOVE_NAMESPACED(TSpacedTy, Ty)

/** Prohibits copying of a specific type T. */
#define PROHIBIT_COPY(Type)                                                          \
    Type(      Type& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
    Type(const Type& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
    Type& operator=(const Type& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) = delete;

/** Prohibits copying of a specific type T. */
#if WITH_MSVC
    #define PROHIBIT_COPY_NAMESPACED(TSpacedType, Type)                                                \
        TSpacedType(      TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
        TSpacedType(const TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
        TSpacedType& operator=(const TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) = delete;
#elif WITH_GCC
    #define PROHIBIT_COPY_NAMESPACED(TSpacedType, Type)                                         \
        Type(      TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
        Type(const TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
        Type& operator=(const TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) = delete;
#else /* WITH_GCC */
    #error "Missing implementation for this platform."
#endif /* !WITH_GCC */

/** Prohibits moving of a specific type T. */
#define PROHIBIT_MOVE(Type)                                                     \
    Type(Type&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
    Type& operator=(Type&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) = delete;

/** Prohibits moving of a specific type T. */
#if WITH_MSVC
    #define PROHIBIT_MOVE_NAMESPACED(TSpacedType, Type)                                           \
        TSpacedType(TSpacedType&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
        TSpacedType& operator=(TSpacedType&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) = delete;
#elif WITH_GCC
    #define PROHIBIT_MOVE_NAMESPACED(TSpacedType, Type)                                    \
        Type(TSpacedType&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
        Type& operator=(TSpacedType&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) = delete;
#else /* WITH_GCC */
    #error "Missing implementation for this platform."
#endif /* !WITH_GCC */

/** Defaults the copy and move operations of a specific type T. */
#define DEFAULT_REALLOC_OF_ANY_FORM(Ty) \
    DEFAULT_COPY(Ty)                    \
    DEFAULT_MOVE(Ty)

#define DEFAULT_REALLOC_OF_ANY_FORM_NAMESPACED(TSpacedTy, Ty) \
    DEFAULT_COPY_NAMESPACED(TSpacedTy, Ty)                    \
    DEFAULT_MOVE_NAMESPACED(TSpacedTy, Ty)

/** Defaults the copy operations of a specific type T. */
#define DEFAULT_COPY(Type)                                                                     \
    Type(      Type& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            noexcept = default; \
    Type(const Type& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            noexcept = default; \
    Type& operator=(const Type& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) noexcept = default;

/** Defaults the copy operations of a specific type T. */
#if WITH_MSVC
    #define DEFAULT_COPY_NAMESPACED(TSpacedType, Type)                                                           \
        TSpacedType(      TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            noexcept = default; \
        TSpacedType(const TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            noexcept = default; \
        TSpacedType& operator=(const TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) noexcept = default;
#elif WITH_GCC
    #define DEFAULT_COPY_NAMESPACED(TSpacedType, Type)                                                    \
        Type(      TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            noexcept = default; \
        Type(const TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            noexcept = default; \
        Type& operator=(const TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) noexcept = default;
#else /* WITH_GCC */
    #error "Missing implementation for this platform."
#endif /* !WITH_GCC */

/** Defaults the move operations of a specific type T. */
#define DEFAULT_MOVE(Type)                                                                \
    Type(Type&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            noexcept = default; \
    Type& operator=(Type&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) noexcept = default;

/** Defaults the move operations of a specific type T. */
#if WITH_MSVC
    #define DEFAULT_MOVE_NAMESPACED(TSpacedType, Type)                                                       \
        TSpacedType(TSpacedType&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            noexcept  = default; \
        TSpacedType& operator=(TSpacedType&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) noexcept  = default;
#elif WITH_GCC
    #define DEFAULT_MOVE_NAMESPACED(TSpacedType, Type)                                               \
        Type(TSpacedType&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            noexcept = default; \
        Type& operator=(TSpacedType&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) noexcept = default;
#else /* WITH_GCC */
    #error "Missing implementation for this platform."
#endif /* !WITH_GCC */

/** Bitwise flagging operations for an enum class. */
#define ENUM_CLASS_FLAGS(Enum)                                                                \
    FORCEINLINE           Enum& operator|=(Enum& Lhs, Enum Rhs)                               \
        { return Lhs = (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
    FORCEINLINE           Enum& operator&=(Enum& Lhs, Enum Rhs)                               \
        { return Lhs = (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
    FORCEINLINE           Enum& operator^=(Enum& Lhs, Enum Rhs)                               \
        { return Lhs = (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
    FORCEINLINE constexpr Enum  operator| (Enum  Lhs, Enum Rhs)                               \
        { return (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); }       \
    FORCEINLINE constexpr Enum  operator& (Enum  Lhs, Enum Rhs)                               \
        { return (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); }       \
    FORCEINLINE constexpr Enum  operator^ (Enum  Lhs, Enum Rhs)                               \
        { return (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); }       \
    FORCEINLINE constexpr bool  operator! (Enum  E)                                           \
        { return !(__underlying_type(Enum))E; }                                               \
    FORCEINLINE constexpr Enum  operator~ (Enum  E)                                           \
        { return (Enum)~(__underlying_type(Enum))E; }

/** This or that ::Jafg::Xor(bA, bB); */
#define XOR(A, B) ( !(!!(A)) != !(!!(B)) )
