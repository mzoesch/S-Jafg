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

/** Prohibits copying and moving of a specific type T. */
#define PROHIBIT_REALLOC_OF_ANY_FROM(Ty) \
    PROHIBIT_COPY(Ty)                    \
    PROHIBIT_MOVE(Ty)

/** Prohibits copying of a specific type T. */
#define PROHIBIT_COPY(Type)                                                          \
    Type(const Type& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
    Type& operator=(const Type& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) = delete;

/** Prohibits moving of a specific type T. */
#define PROHIBIT_MOVE(Type)                                                     \
    Type(Type&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type))            = delete; \
    Type& operator=(Type&& PRIVATE_JAFG_CORE_JOIN_INNER_TWO(_, Type)) = delete;
