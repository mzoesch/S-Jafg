// Copyright mzoesch. All rights reserved.

#pragma once

/**
 * Helper macro to export / import an instantiated templated dynamic heap array for
 * a specific element type T out of a shared library.
 *
 * @param Api     The api where we should look for export / import symbols.
 * @param Element The element type T of the array.
 */
#define MAKE_EXTERNAL_TEMPLATE_DHARRAY(Api, Element)                    \
    PRIVATE_JAFG_MAKE_EXTERNAL_TEMPLATE(                                \
        Api,                                                            \
        TArray<                                                         \
            Element PRIVATE_JAFG_CONTAINER_COMMA                        \
            ::Jafg::ResizePolicy::Dynamic PRIVATE_JAFG_CONTAINER_COMMA  \
            ::Jafg::AllocationPolicy::Heap PRIVATE_JAFG_CONTAINER_COMMA \
            ::Jafg::DefaultContainerSizeType                            \
        >                                                               \
    )

#define PRIVATE_JAFG_CONTAINER_COMMA                                           ,
#define PRIVATE_JAFG_CONTAINER_JOIN_INNER_TWO(   A, B                        ) A##B
#define PRIVATE_JAFG_CONTAINER_JOIN_INNER_THREE( A, B, C                     ) A##B##C
#define PRIVATE_JAFG_CONTAINER_JOIN_INNER_FOUR(  A, B, C, D                  ) A##B##C##D
#define PRIVATE_JAFG_CONTAINER_JOIN_INNER_FIVE(  A, B, C, D, E               ) A##B##C##D##E
#define PRIVATE_JAFG_CONTAINER_JOIN_INNER_SIX(   A, B, C, D, E, F            ) A##B##C##D##E##F
#define PRIVATE_JAFG_CONTAINER_JOIN_INNER_SEVEN( A, B, C, D, E, F, G         ) A##B##C##D##E##F##G
#define PRIVATE_JAFG_CONTAINER_JOIN_INNER_EIGHT( A, B, C, D, E, F, G, H      ) A##B##C##D##E##F##G##H
#define PRIVATE_JAFG_CONTAINER_JOIN_INNER_NINE(  A, B, C, D, E, F, G, H, I   ) A##B##C##D##E##F##G##H##I

#define PRIVATE_JAFG_MAKE_EXTERNAL_TEMPLATE(Api, Class) \
    PRIVATE_JAFG_CONTAINER_JOIN_INNER_TWO(Api, _EXTERN) \
    template class                                      \
    PRIVATE_JAFG_CONTAINER_JOIN_INNER_TWO(Api, _API)    \
    Class; /* Eh? Wtf? This is wrong and won't work. Do not fix this warning: */
           /*         Macro argument should be enclosed in parentheses.       */
