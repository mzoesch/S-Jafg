// Copyright mzoesch. All rights reserved.

#pragma once

#ifndef LAL_CHECK_ARRAY
    #if LAL_CHECK_CONTAINER_BOUNDS
        #define LAL_CHECK_ARRAY(Expr)           jassert( Expr )
    #else /* LAL_CHECK_CONTAINER_BOUNDS */
        #define LAL_CHECK_ARRAY(Expr)
    #endif /* !LAL_CHECK_CONTAINER_BOUNDS */
#endif /* !LAL_CHECK_ARRAY */
