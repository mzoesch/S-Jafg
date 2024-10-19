// Copyright mzoesch. All rights reserved.

#pragma once

#ifndef PRIVATE_INCLUDED_AUTOMATIC_BUILD
    #error "This file should not be included directly. Include Build/Build.h instead."
#endif /* !INCLUDED_AUTOMATIC_BUILD_PRIVATE */
#if !REFLECT_MANUAL_BUILD_OVERRIDES
    #error "This file should not be included when REFLECT_MANUAL_BUILD is false."
#endif /* !REFLECT_MANUAL_BUILD */


/*-----------------------------------------------------------------------------
    Override build configurations manually here.
-----------------------------------------------------------------------------*/

/** Whether to ever do checks. This affects all build configurations. */
#ifndef DO_EVER_CHECKS
    #define DO_EVER_CHECKS                              1
#endif /* !DO_EVER_CHECKS */

/** Whether to override the default behavior to strip checks in shipping builds. */
#ifndef DO_CHECKS_IN_SHIPPING
    #define DO_CHECKS_IN_SHIPPING                       0
#endif /* !DO_CHECKS_IN_SHIPPING */

/** Whether to override the default behavior to strip checks in debug builds. */
#ifndef ALLOW_SLOW_CHECKS
    #define ALLOW_SLOW_CHECKS                           1
#endif /* !ALLOW_SLOW_CHECKS */

/**
 * Whether slow checks should share the same lifetime as normal checks among build configurations.
 * Default is zero, so slow checks only get evaluated in debug builds.
 */
#ifndef LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME
    #define LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME        0
#endif /* !LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME */

/** Whether to ever do assertions. This affects all build configurations. */
#ifndef DO_EVER_ASSERTS
    #define DO_EVER_ASSERTS                             1
#endif /* !DO_EVER_ASSERTS */

/**
 * Whether to evaporate assertions in shipping builds. Turing this on may cause undefined behavior
 * when encountering an assertion in shipping builds.
 * Note that panics are not affected by this setting.
 */
#ifndef DO_STRIP_ASSERTS_IN_SHIPPING
    #define DO_STRIP_ASSERTS_IN_SHIPPING                0
#endif /* !DO_STRIP_ASSERTS_IN_SHIPPING */
