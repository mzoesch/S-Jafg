// Copyright mzoesch. All rights reserved.

#pragma once

#define JAFG_COMMA                                        ,

#define JAFG_COMBINE_ANY_INNER(...)                                     __VA_ARGS__
#define JAFG_COMBINE_ANY_OUTER(...)                                     JAFG_COMBINE_ANY_INNER(__VA_ARGS__)
#define JAFG_MAKE_STRING_INNER(...)                                     #__VA_ARGS__
#define JAFG_MAKE_STRING_OUTER(...)                                     JAFG_MAKE_STRING_INNER(__VA_ARGS__)

#define JAFG_JOIN_INNER_TWO(   A, B                      )              A##B
#define JAFG_JOIN_INNER_THREE( A, B, C                   )              A##B##C
#define JAFG_JOIN_INNER_FOUR(  A, B, C, D                )              A##B##C##D
#define JAFG_JOIN_INNER_FIVE(  A, B, C, D, E             )              A##B##C##D##E
#define JAFG_JOIN_INNER_SIX(   A, B, C, D, E, F          )              A##B##C##D##E##F
#define JAFG_JOIN_INNER_SEVEN( A, B, C, D, E, F, G       )              A##B##C##D##E##F##G
#define JAFG_JOIN_INNER_EIGHT( A, B, C, D, E, F, G, H    )              A##B##C##D##E##F##G##H
#define JAFG_JOIN_INNER_NINE(  A, B, C, D, E, F, G, H, I )              A##B##C##D##E##F##G##H##I

#define JAFG_JOIN_OUTER_TWO(   A, B                      )              JAFG_JOIN_INNER_TWO(   A, B                      )
#define JAFG_JOIN_OUTER_THREE( A, B, C                   )              JAFG_JOIN_INNER_THREE( A, B, C                   )
#define JAFG_JOIN_OUTER_FOUR(  A, B, C, D                )              JAFG_JOIN_INNER_FOUR(  A, B, C, D                )
#define JAFG_JOIN_OUTER_FIVE(  A, B, C, D, E             )              JAFG_JOIN_INNER_FIVE(  A, B, C, D, E             )
#define JAFG_JOIN_OUTER_SIX(   A, B, C, D, E, F          )              JAFG_JOIN_INNER_SIX(   A, B, C, D, E, F          )
#define JAFG_JOIN_OUTER_SEVEN( A, B, C, D, E, F, G       )              JAFG_JOIN_INNER_SEVEN( A, B, C, D, E, F, G       )
#define JAFG_JOIN_OUTER_EIGHT( A, B, C, D, E, F, G, H    )              JAFG_JOIN_INNER_EIGHT( A, B, C, D, E, F, G, H    )
#define JAFG_JOIN_OUTER_NINE(  A, B, C, D, E, F, G, H, I )              JAFG_JOIN_INNER_NINE(  A, B, C, D, E, F, G, H, I )

#define JAFG_CAT_INNER_TWO(   A, B                      )               A B
#define JAFG_CAT_INNER_THREE( A, B, C                   )               A B C
#define JAFG_CAT_INNER_FOUR(  A, B, C, D                )               A B C D
#define JAFG_CAT_INNER_FIVE(  A, B, C, D, E             )               A B C D E
#define JAFG_CAT_INNER_SIX(   A, B, C, D, E, F          )               A B C D E F
#define JAFG_CAT_INNER_SEVEN( A, B, C, D, E, F, G       )               A B C D E F G
#define JAFG_CAT_INNER_EIGHT( A, B, C, D, E, F, G, H    )               A B C D E F G H
#define JAFG_CAT_INNER_NINE(  A, B, C, D, E, F, G, H, I )               A B C D E F G H I

#define JAFG_CAT_OUTER_TWO(   A, B                      )               JAFG_CAT_INNER_TWO(   A, B                      )
#define JAFG_CAT_OUTER_THREE( A, B, C                   )               JAFG_CAT_INNER_THREE( A, B, C                   )
#define JAFG_CAT_OUTER_FOUR(  A, B, C, D                )               JAFG_CAT_INNER_FOUR(  A, B, C, D                )
#define JAFG_CAT_OUTER_FIVE(  A, B, C, D, E             )               JAFG_CAT_INNER_FIVE(  A, B, C, D, E             )
#define JAFG_CAT_OUTER_SIX(   A, B, C, D, E, F          )               JAFG_CAT_INNER_SIX(   A, B, C, D, E, F          )
#define JAFG_CAT_OUTER_SEVEN( A, B, C, D, E, F, G       )               JAFG_CAT_INNER_SEVEN( A, B, C, D, E, F, G       )
#define JAFG_CAT_OUTER_EIGHT( A, B, C, D, E, F, G, H    )               JAFG_CAT_INNER_EIGHT( A, B, C, D, E, F, G, H    )
#define JAFG_CAT_OUTER_NINE(  A, B, C, D, E, F, G, H, I )               JAFG_CAT_INNER_NINE(  A, B, C, D, E, F, G, H, I )

//#
//# We cannot concat :: in GCC, because it is too strict for that kind of operation and actually thinks that :: is a
//# fucking operator. In the godlike MSVC world, we can do that without any problems. Why tf.
//#
#define JAFG_JOIN_SCOPE_RESOLUTION_INNER_TWO(   A, B                      ) A::B
#define JAFG_JOIN_SCOPE_RESOLUTION_INNER_THREE( A, B, C                   ) A::B::C
#define JAFG_JOIN_SCOPE_RESOLUTION_INNER_FOUR(  A, B, C, D                ) A::B::C::D
#define JAFG_JOIN_SCOPE_RESOLUTION_INNER_FIVE(  A, B, C, D, E             ) A::B::C::D::E
#define JAFG_JOIN_SCOPE_RESOLUTION_INNER_SIX(   A, B, C, D, E, F          ) A::B::C::D::E::F
#define JAFG_JOIN_SCOPE_RESOLUTION_INNER_SEVEN( A, B, C, D, E, F, G       ) A::B::C::D::E::F::G
#define JAFG_JOIN_SCOPE_RESOLUTION_INNER_EIGHT( A, B, C, D, E, F, G, H    ) A::B::C::D::E::F::G::H
#define JAFG_JOIN_SCOPE_RESOLUTION_INNER_NINE(  A, B, C, D, E, F, G, H, I ) A::B::C::D::E::F::G::H::I

#define JAFG_JOIN_SCOPE_RESOLUTION_OUTER_TWO(   A, B                      ) JAFG_JOIN_SCOPE_RESOLUTION_INNER_TWO(   A, B                      )
#define JAFG_JOIN_SCOPE_RESOLUTION_OUTER_THREE( A, B, C                   ) JAFG_JOIN_SCOPE_RESOLUTION_INNER_THREE( A, B, C                   )
#define JAFG_JOIN_SCOPE_RESOLUTION_OUTER_FOUR(  A, B, C, D                ) JAFG_JOIN_SCOPE_RESOLUTION_INNER_FOUR(  A, B, C, D                )
#define JAFG_JOIN_SCOPE_RESOLUTION_OUTER_FIVE(  A, B, C, D, E             ) JAFG_JOIN_SCOPE_RESOLUTION_INNER_FIVE(  A, B, C, D, E             )
#define JAFG_JOIN_SCOPE_RESOLUTION_OUTER_SIX(   A, B, C, D, E, F          ) JAFG_JOIN_SCOPE_RESOLUTION_INNER_SIX(   A, B, C, D, E, F          )
#define JAFG_JOIN_SCOPE_RESOLUTION_OUTER_SEVEN( A, B, C, D, E, F, G       ) JAFG_JOIN_SCOPE_RESOLUTION_INNER_SEVEN( A, B, C, D, E, F, G       )
#define JAFG_JOIN_SCOPE_RESOLUTION_OUTER_EIGHT( A, B, C, D, E, F, G, H    ) JAFG_JOIN_SCOPE_RESOLUTION_INNER_EIGHT( A, B, C, D, E, F, G, H    )
#define JAFG_JOIN_SCOPE_RESOLUTION_OUTER_NINE(  A, B, C, D, E, F, G, H, I ) JAFG_JOIN_SCOPE_RESOLUTION_INNER_NINE(  A, B, C, D, E, F, G, H, I )

#define JAFG_INIT_ONE(   DefaultValue ) { DefaultValue }
#define JAFG_INIT_TWO(   DefaultValue ) { DefaultValue, DefaultValue }
#define JAFG_INIT_THREE( DefaultValue ) { DefaultValue, DefaultValue, DefaultValue }
#define JAFG_INIT_FOUR(  DefaultValue ) { DefaultValue, DefaultValue, DefaultValue, DefaultValue }
#define JAFG_INIT_FIVE(  DefaultValue ) { DefaultValue, DefaultValue, DefaultValue, DefaultValue, DefaultValue }
#define JAFG_INIT_SIX(   DefaultValue ) { DefaultValue, DefaultValue, DefaultValue, DefaultValue, DefaultValue, DefaultValue }
#define JAFG_INIT_SEVEN( DefaultValue ) { DefaultValue, DefaultValue, DefaultValue, DefaultValue, DefaultValue, DefaultValue, DefaultValue }
#define JAFG_INIT_EIGHT( DefaultValue ) { DefaultValue, DefaultValue, DefaultValue, DefaultValue, DefaultValue, DefaultValue, DefaultValue, DefaultValue }
#define JAFG_INIT_NINE(  DefaultValue ) { DefaultValue, DefaultValue, DefaultValue, DefaultValue, DefaultValue, DefaultValue, DefaultValue, DefaultValue, DefaultValue }

//#
//# Map a number of elements against #Transform until a maximum of 512 elements.
//# This was generated with #Programs/MapGenerator.py. So if you need more...
//# But hopefully with cxx26 ts will become obsolete.
//#
#include "JafgMap.h"

//#
//# Use this macro for formatting raw string literals.
//# @remark MSVC is by far more permissive, so it supports this feature natively as they always decay raw string
//#         literals (const char[n]) as const char pointers. While GCC and Clang are more strict (in terms of the C++
//#         type system) and they do not allow this kind of implicit conversion - therefore, we have to explicitly cast
//#         the literal to our formatter.
//#
#if JAFG_WITH_MSVC || JAFG_WITH_CLANG /* We do not use L on win so yea... */
    #define FMT(X)                                                      X
#elif JAFG_WITH_GCC
    #define FMT(X)                                                      static_cast<const char*>(X)
#else /* JAFG_WITH_GCC */
    #error "Missing implementation for this platform."
#endif /* !JAFG_WITH_GCC */

//#
//# Advice the preprocessor to exclude the following code without the interactive intellisense mocking at us
//# that we excluded content of a condition that is always false.
//#
#define PREPROCESSOR_EXCLUDE_FF             0

//# A struct must always be at least one byte in size.
#define UNREACHABLE_BYTE_SIZE_FOR_STRUCT    0x00
#define UNREACHABLE_BYTE_SIZE_FOR_CLASS     0x00
#define UNREACHABLE_BYTE_SIZE_FOR_TYPE      0x00

//# Causes a compile time error if this specialization of a templated function is used.
#if JAFG_WITH_MSVC
    #define UNSUPPORTED_TEMPLATED_SPECIALIZATION(Ty, ...)                                                           \
        {                                                                                                           \
            static_assert(sizeof(Ty) == UNREACHABLE_BYTE_SIZE_FOR_TYPE, "Templated specialization not supported."); \
            __VA_ARGS__;                                                                                          \
        }
#elif JAFG_WITH_GCC || JAFG_WITH_CLANG
    #define UNSUPPORTED_TEMPLATED_SPECIALIZATION(Ty, ...)                                                           \
        {                                                                                                           \
            static_assert(sizeof(Ty) == UNREACHABLE_BYTE_SIZE_FOR_TYPE, "Templated specialization not supported."); \
            __VA_ARGS__;                                                                                            \
        }
#else /* JAFG_WITH_GCC || JAFG_WITH_CLANG */
    #error "Missing implementation for this platform."
#endif /* !JAFG_WITH_GCC && !JAFG_WITH_CLANG */

#define UNREACHABLE_CONTROL_PATH_STATIC(Cond) static_assert((Cond), "Unreachable control path.");

//# Prohibits copying and moving of a specific type T.
#define PROHIBIT_REALLOC_OF_ANY_FORM(Ty) \
    PROHIBIT_COPY(Ty)                    \
    PROHIBIT_MOVE(Ty)

#define PROHIBIT_REALLOC_OF_ANY_FORM_NAMESPACED(TSpacedTy, Ty) \
    PROHIBIT_COPY_NAMESPACED(TSpacedTy, Ty)                    \
    PROHIBIT_MOVE_NAMESPACED(TSpacedTy, Ty)

//# Prohibits copying of a specific type T.
#define PROHIBIT_COPY(Type)                                             \
    Type(      Type& JAFG_JOIN_INNER_TWO(_, Type))            = delete; \
    Type(const Type& JAFG_JOIN_INNER_TWO(_, Type))            = delete; \
    Type& operator=(const Type& JAFG_JOIN_INNER_TWO(_, Type)) = delete;

//# Prohibits copying of a specific type T.
#if JAFG_WITH_MSVC || JAFG_WITH_CLANG
    #define PROHIBIT_COPY_NAMESPACED(TSpacedType, Type)                                   \
        TSpacedType(TSpacedType      & JAFG_JOIN_INNER_TWO(_, Type))            = delete; \
        TSpacedType(TSpacedType const& JAFG_JOIN_INNER_TWO(_, Type))            = delete; \
        TSpacedType& operator=(TSpacedType const& JAFG_JOIN_INNER_TWO(_, Type)) = delete;
#elif JAFG_WITH_GCC
    #define PROHIBIT_COPY_NAMESPACED(TSpacedType, Type)                            \
        Type(TSpacedType      & JAFG_JOIN_INNER_TWO(_, Type))            = delete; \
        Type(TSpacedType const& JAFG_JOIN_INNER_TWO(_, Type))            = delete; \
        Type& operator=(const TSpacedType& JAFG_JOIN_INNER_TWO(_, Type)) = delete;
#else /* JAFG_WITH_GCC */
    #error "Missing implementation for this platform."
#endif /* !JAFG_WITH_GCC */

//# Prohibits moving of a specific type T.
#define PROHIBIT_MOVE(Type)                                        \
    Type(Type&& JAFG_JOIN_INNER_TWO(_, Type))            = delete; \
    Type& operator=(Type&& JAFG_JOIN_INNER_TWO(_, Type)) = delete;

//# Prohibits moving of a specific type T.
#if JAFG_WITH_MSVC || JAFG_WITH_CLANG
    #define PROHIBIT_MOVE_NAMESPACED(TSpacedType, Type)                              \
        TSpacedType(TSpacedType&& JAFG_JOIN_INNER_TWO(_, Type))            = delete; \
        TSpacedType& operator=(TSpacedType&& JAFG_JOIN_INNER_TWO(_, Type)) = delete;
#elif JAFG_WITH_GCC
    #define PROHIBIT_MOVE_NAMESPACED(TSpacedType, Type)                       \
        Type(TSpacedType&& JAFG_JOIN_INNER_TWO(_, Type))            = delete; \
        Type& operator=(TSpacedType&& JAFG_JOIN_INNER_TWO(_, Type)) = delete;
#else /* JAFG_WITH_GCC */
    #error "Missing implementation for this platform."
#endif /* !JAFG_WITH_GCC */

//# Defaults the copy and move operations of a specific type T.
#define DEFAULT_REALLOC_OF_ANY_FORM(Ty) \
    DEFAULT_COPY(Ty)                    \
    DEFAULT_MOVE(Ty)
#define DEFAULT_CONSTEXPR_REALLOC_OF_ANY_FORM(Ty) \
    DEFAULT_CONSTEXPR_COPY(Ty)                    \
    DEFAULT_CONSTEXPR_MOVE(Ty)

#define DEFAULT_REALLOC_OF_ANY_FORM_NAMESPACED(TSpacedTy, Ty) \
    DEFAULT_COPY_NAMESPACED(TSpacedTy, Ty)                    \
    DEFAULT_MOVE_NAMESPACED(TSpacedTy, Ty)
#define DEFAULT_CONSTEXPR_REALLOC_OF_ANY_FORM_NAMESPACED(TSpacedTy, Ty) \
    DEFAULT_CONSTEXPR_COPY_NAMESPACED(TSpacedTy, Ty)                    \
    DEFAULT_CONSTEXPR_MOVE_NAMESPACED(TSpacedTy, Ty)

//# Defaults the copy operations of a specific type T.
#define DEFAULT_COPY(Type)                                                        \
    Type(Type const& JAFG_JOIN_INNER_TWO(_, Type))            noexcept = default; \
    Type& operator=(Type const& JAFG_JOIN_INNER_TWO(_, Type)) noexcept = default;
#define DEFAULT_CONSTEXPR_COPY(Type)                                                        \
    constexpr Type(Type const& JAFG_JOIN_INNER_TWO(_, Type))            noexcept = default; \
    constexpr Type& operator=(Type const& JAFG_JOIN_INNER_TWO(_, Type)) noexcept = default;

//# Defaults the copy operations of a specific type T.
#if JAFG_WITH_MSVC || JAFG_WITH_CLANG
    #define DEFAULT_COPY_NAMESPACED(TSpacedType, Type)                                              \
        TSpacedType(TSpacedType const& JAFG_JOIN_INNER_TWO(_, Type))            noexcept = default; \
        TSpacedType& operator=(TSpacedType const& JAFG_JOIN_INNER_TWO(_, Type)) noexcept = default;
    #define DEFAULT_CONSTEXPR_COPY_NAMESPACED(TSpacedType, Type)                                              \
        constexpr TSpacedType(TSpacedType const& JAFG_JOIN_INNER_TWO(_, Type))            noexcept = default; \
        constexpr TSpacedType& operator=(TSpacedType const& JAFG_JOIN_INNER_TWO(_, Type)) noexcept = default;
#elif JAFG_WITH_GCC
    #define DEFAULT_COPY_NAMESPACED(TSpacedType, Type)                                       \
        Type(TSpacedType const& JAFG_JOIN_INNER_TWO(_, Type))            noexcept = default; \
        Type& operator=(TSpacedType const& JAFG_JOIN_INNER_TWO(_, Type)) noexcept = default;
    #define DEFAULT_CONSTEXPR_COPY_NAMESPACED(TSpacedType, Type)                                       \
        constexpr Type(TSpacedType const& JAFG_JOIN_INNER_TWO(_, Type))            noexcept = default; \
        constexpr Type& operator=(TSpacedType const& JAFG_JOIN_INNER_TWO(_, Type)) noexcept = default;
#else /* JAFG_WITH_GCC */
    #error "Missing implementation for this platform."
#endif /* !JAFG_WITH_GCC */

//# Defaults the move operations of a specific type T.
#define DEFAULT_MOVE(Type)                                                  \
    Type(Type&& JAFG_JOIN_INNER_TWO(_, Type))            noexcept = default; \
    Type& operator=(Type&& JAFG_JOIN_INNER_TWO(_, Type)) noexcept = default;
#define DEFAULT_CONSTEXPR_MOVE(Type)                                                   \
    constexpr Type(Type&& JAFG_JOIN_INNER_TWO(_, Type))            noexcept = default; \
    constexpr Type& operator=(Type&& JAFG_JOIN_INNER_TWO(_, Type)) noexcept = default;

//# Defaults the move operations of a specific type T.
#if JAFG_WITH_MSVC || JAFG_WITH_CLANG
    #define DEFAULT_MOVE_NAMESPACED(TSpacedType, Type)                                          \
        TSpacedType(TSpacedType&& JAFG_JOIN_INNER_TWO(_, Type))            noexcept  = default; \
        TSpacedType& operator=(TSpacedType&& JAFG_JOIN_INNER_TWO(_, Type)) noexcept  = default;
    #define DEFAULT_CONSTEXPR_MOVE_NAMESPACED(TSpacedType, Type)                                          \
        constexpr TSpacedType(TSpacedType&& JAFG_JOIN_INNER_TWO(_, Type))            noexcept  = default; \
        constexpr TSpacedType& operator=(TSpacedType&& JAFG_JOIN_INNER_TWO(_, Type)) noexcept  = default;
#elif JAFG_WITH_GCC
    #define DEFAULT_MOVE_NAMESPACED(TSpacedType, Type)                                  \
        Type(TSpacedType&& JAFG_JOIN_INNER_TWO(_, Type))            noexcept = default; \
        Type& operator=(TSpacedType&& JAFG_JOIN_INNER_TWO(_, Type)) noexcept = default;
    #define DEFAULT_CONSTEXPR_MOVE_NAMESPACED(TSpacedType, Type)                                  \
        constexpr Type(TSpacedType&& JAFG_JOIN_INNER_TWO(_, Type))            noexcept = default; \
        constexpr Type& operator=(TSpacedType&& JAFG_JOIN_INNER_TWO(_, Type)) noexcept = default;
#else /* JAFG_WITH_GCC */
    #error "Missing implementation for this platform."
#endif /* !JAFG_WITH_GCC */

//# For static classes.
#define UTILITY_CLASS(Type) UTILITY_STRUCT(Type)
#define UTILITY_STRUCT(Type)           \
    Type() = delete;                   \
    PROHIBIT_REALLOC_OF_ANY_FORM(Type) \
    ~Type() = delete;

//# Bitwise flagging operations for an enum namespace classes.
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

//# Allows one to use "anonymous" structs in templated paths.
#define JAFG_ANONYMOUS_STRUCT(...) \
    decltype([]         \
    {                   \
        struct _        \
        {               \
            __VA_ARGS__ \
        } _ret;         \
        return _ret;    \
    }())

#ifndef DO_PURE_VIRTUAL_COMPILER_CHECKS
    #define DO_PURE_VIRTUAL_COMPILER_CHECKS                 0
#endif /* !DO_PURE_VIRTUAL_COMPILER_CHECKS */

#ifndef PURE_VIRTUAL
    #if DO_PURE_VIRTUAL_COMPILER_CHECKS
        #define PURE_VIRTUAL(...)           = 0;
    #else /* DO_PURE_VIRTUAL_COMPILER_CHECKS */
        //# Define a RetTy for non-void members if needed.
        #define PURE_VIRTUAL(...)           { panic("Pure virtual function was encountered.") __VA_ARGS__; }
    #endif /* !DO_PURE_VIRTUAL_COMPILER_CHECKS */
#endif /* !PURE_VIRTUAL */
