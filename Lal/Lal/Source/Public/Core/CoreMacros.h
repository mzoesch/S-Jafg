// Copyright mzoesch. All rights reserved.

#pragma once

#define PRIVATE_JAFG_CORE_COMMA                                           ,
#define PRIVATE_JAFG_CORE_JOIN_INNER_TWO(   A, B                        ) A##B
#define PRIVATE_JAFG_CORE_JOIN_INNER_THREE( A, B, C                     ) A##B##C
#define PRIVATE_JAFG_CORE_JOIN_INNER_FOUR(  A, B, C, D                  ) A##B##C##D
#define PRIVATE_JAFG_CORE_JOIN_INNER_FIVE(  A, B, C, D, E               ) A##B##C##D##E
#define PRIVATE_JAFG_CORE_JOIN_INNER_SIX(   A, B, C, D, E, F            ) A##B##C##D##E##F
#define PRIVATE_JAFG_CORE_JOIN_INNER_SEVEN( A, B, C, D, E, F, G         ) A##B##C##D##E##F##G
#define PRIVATE_JAFG_CORE_JOIN_INNER_EIGHT( A, B, C, D, E, F, G, H      ) A##B##C##D##E##F##G##H
#define PRIVATE_JAFG_CORE_JOIN_INNER_NINE(  A, B, C, D, E, F, G, H, I   ) A##B##C##D##E##F##G##H##I

/**
 * Advice the preprocessor to exclude the following code without the interactive intellisense mocking at us
 * that we excluded content of a condition that is always false.
 */
#define PREPROCESSOR_EXCLUDE_FF     0

/** A struct must always be at least one byte in size. */
#define UNREACHABLE_BYTE_SIZE_FOR_STRUCT 0x00
#define UNREACHABLE_BYTE_SIZE_FOR_CLASS  0x00
#define UNREACHABLE_BYTE_SIZE_FOR_TYPE   0x00

/**
 * Causes a compile time error if this specialization of a templated function is used.
 */
#define UNSUPPORTED_TEMPLATED_SPECIALIZATION(Ty, ...)                                                           \
    {                                                                                                           \
        static_assert(sizeof(Ty) == UNREACHABLE_BYTE_SIZE_FOR_TYPE, "Templated specialization not supported."); \
        ##__VA_ARGS__;                                                                                          \
    }

/** Prohibits copying and moving of a specific type T. */
#define PROHIBIT_REALLOC_OF_ANY_FROM(Ty) \
    PROHIBIT_COPY(Ty)                    \
    PROHIBIT_MOVE(Ty)

#define PROHIBIT_REALLOC_OF_ANY_FROM_NAMESPACED(TSpacedTy, Ty) \
    PROHIBIT_COPY_NAMESPACED(TSpacedTy, Ty)                    \
    PROHIBIT_MOVE_NAMESPACED(TSpacedTy, Ty)

/** Prohibits copying of a specific type T. */
#define PROHIBIT_COPY(Type)                                                          \
    Type(      Type& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
    Type(const Type& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
    Type& operator=(const Type& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) = delete;

/** Prohibits copying of a specific type T. */
#define PROHIBIT_COPY_NAMESPACED(TSpacedType, Type) \
    TSpacedType(      TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
    TSpacedType(const TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
    TSpacedType& operator=(const TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) = delete;

/** Prohibits moving of a specific type T. */
#define PROHIBIT_MOVE(Type)                                                     \
    Type(Type&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
    Type& operator=(Type&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) = delete;

/** Prohibits moving of a specific type T. */
#define PROHIBIT_MOVE_NAMESPACED(TSpacedType, Type) \
    TSpacedType(TSpacedType&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
    TSpacedType& operator=(TSpacedType&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) = delete;

/** Defaults the copy and move operations of a specific type T. */
#define DEFAULT_REALLOC_OF_ANY_FROM(Ty) \
    DEFAULT_COPY(Ty)                    \
    DEFAULT_MOVE(Ty)

#define DEFAULT_REALLOC_OF_ANY_FROM_NAMESPACED(TSpacedTy, Ty) \
    DEFAULT_COPY_NAMESPACED(TSpacedTy, Ty)                    \
    DEFAULT_MOVE_NAMESPACED(TSpacedTy, Ty)

/** Defaults the copy operations of a specific type T. */
#define DEFAULT_COPY(Type) \
    Type(      Type& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = default; \
    Type(const Type& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = default; \
    Type& operator=(const Type& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) = default;

/** Defaults the copy operations of a specific type T. */
#define DEFAULT_COPY_NAMESPACED(TSpacedType, Type) \
    TSpacedType(      TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = default; \
    TSpacedType(const TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = default; \
    TSpacedType& operator=(const TSpacedType& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) = default;

/** Defaults the move operations of a specific type T. */
#define DEFAULT_MOVE(Type) \
    Type(Type&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = default; \
    Type& operator=(Type&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) = default;

/** Defaults the move operations of a specific type T. */
#define DEFAULT_MOVE_NAMESPACED(TSpacedType, Type) \
    TSpacedType(TSpacedType&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = default; \
    TSpacedType& operator=(TSpacedType&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) = default;

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
