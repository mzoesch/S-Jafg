// Copyright mzoesch. All rights reserved.

#pragma once

#ifndef INCLUDED_AUTOMATIC_BUILD_PRIVATE
    #error "This file should not be included directly. Include Build/Build.h instead."
#endif /* !INCLUDED_AUTOMATIC_BUILD_PRIVATE */
#if !REFLECT_MANUAL_BUILD_OVERRIDES
    #error "This file should not be included when REFLECT_MANUAL_BUILD is false."
#endif /* !REFLECT_MANUAL_BUILD */


/*-----------------------------------------------------------------------------
    Override build configurations manually here.
-----------------------------------------------------------------------------*/

/** Whether to ever do checks. This affects all build configurations. */
#define DO_EVER_CHECKS                  1
#define DO_CHECKS_IN_SHIPPING           0
/** Whether to ever do assertions. This affects all build configurations. */
#define DO_EVER_ASSERTS                 1
/** Whether to evaporate assertions in shipping builds. */
#define DO_STRIP_ASSERTS_IN_SHIPPING    0
