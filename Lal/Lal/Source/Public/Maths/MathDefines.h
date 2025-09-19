// Copyright mzoesch. All rights reserved.

#pragma once

/*----------------------------------------------------------------------------
    Floating point constants.
----------------------------------------------------------------------------*/

///////////////////////////////////////////////////////////////////////////////
// Thresholds.
#define LAL_THRESHOLD_NORM_VEC_F           (1.e-2f) /** Tolerance flt constant for norm check with squared magnitude of a vector. */
#define LAL_THRESHOLD_NORM_VEC_D           (1.e-2)  /** Tolerance dbl constant for norm check with squared magnitude of a vector. */

///////////////////////////////////////////////////////////////////////////////
// Common constants.
#define LAL_FLOAT_SMALL_NUMBER             (1.e-8f)
#define LAL_FLOAT_NOT_SO_SMALL_NUMBER      (1.e-4f)

#define LAL_DOUBLE_SMALL_NUMBER            (1.e-8)
#define LAL_DOUBLE_NOT_SO_SMALL_NUMBER     (1.e-4)

#define LAL_PI_F                           (3.1415926535897932384626433832795028841971693993751058209749445923078164f)
#define LAL_HALF_PI_F                      (1.5707963267948966192313216916397514420985846996875529104874722961539082f)
#define LAL_TWO_PI_F                       (6.2831853071795864769252867665590057683943387987502116419498891846156328f)
#define LAL_PI_SQRT_F                      (1.7724538509055160272981674833411451827975494561223871282138077898529112f)
#define LAL_PI_SQUARED_F                   (9.8696044010893586188344909998761511353136994072407906264133493762200448f)
#define LAL_PI_D                           (3.1415926535897932384626433832795028841971693993751058209749445923078164 )
#define LAL_HALF_PI_D                      (1.5707963267948966192313216916397514420985846996875529104874722961539082 )
#define LAL_TWO_PI_D                       (6.2831853071795864769252867665590057683943387987502116419498891846156328 )
#define LAL_PI_SQRT_D                      (1.7724538509055160272981674833411451827975494561223871282138077898529112 )
#define LAL_PI_SQUARED_D                   (9.8696044010893586188344909998761511353136994072407906264133493762200448 )

#define LAL_DEG_FULL_CIRCLE_F              (360.0f)
#define LAL_RAD_FULL_CIRCLE_F              (LAL_TWO_PI_F)
#define LAL_DEG_QUARTER_CIRCLE_F           (90.0f)
#define LAL_RAD_QUARTER_CIRCLE_F           (LAL_HALF_PI_F)
#define LAL_DEG_HALF_CIRCLE_F              (180.0f)
#define LAL_RAD_HALF_CIRCLE_F              (LAL_PI_F)
#define LAL_DEG_THREE_THIRD_CIRCLE_F       (270.0f)
#define LAL_RAD_THREE_THIRD_CIRCLE_F       (4.7123889803846898576939650749192543262957540990626587314624168884617246f)
#define LAL_DEG_FULL_CIRCLE_D              (360.0)
#define LAL_RAD_FULL_CIRCLE_D              (LAL_TWO_PI_D)
#define LAL_DEG_QUARTER_CIRCLE_D           (90.0)
#define LAL_RAD_QUARTER_CIRCLE_D           (LAL_HALF_PI_D)
#define LAL_DEG_HALF_CIRCLE_D              (180.0)
#define LAL_RAD_HALF_CIRCLE_D              (LAL_PI_D)
#define LAL_DEG_THREE_THIRD_CIRCLE_D       (270.0)
#define LAL_RAD_THREE_THIRD_CIRCLE_D       (4.7123889803846898576939650749192543262957540990626587314624168884617246 )

#define LAL_FLOAT_SUPREMUM                 (3.402823466e+38f)          /* Max positive flt value. */
#define LAL_FLOAT_INFIMUM                  (1.175494351e-38f)          /* Min normalized positive flt value. */
#define LAL_FLOAT_TRUE_MINIMUM             (1.401298464e-45f)          /* Min positive flt value. */
#define LAL_DOUBLE_SUPREMUM                (1.7976931348623158e+308)   /* Max positive dbl value. */
#define LAL_DOUBLE_INFIMUM                 (2.2250738585072014e-308)   /* Min normalized positive dbl value. */
#define LAL_DOUBLE_TRUE_MINIMUM            (4.9406564584124654e-324)   /* Min positive dbl value. */

#define LAL_FROM_PERCENT_F                 (0.01f)
#define LAL_FROM_PERCENT_D                 (0.01 )
#define LAL_TO_PERCENT_F                   (100.0f)
#define LAL_TO_PERCENT_D                   (100.0 )

///////////////////////////////////////////////////////////////////////////////
// Scalar conversion constants for metric units.
#define LAL_KM2M_F                         (1e+3f)
#define LAL_KM2CM_F                        (1e+5f)
#define LAL_KM2MM_F                        (1e+6f)
#define LAL_M2KM_F                         (1e-3f)
#define LAL_M2CM_F                         (1e+2f)
#define LAL_M2MM_F                         (1e+3f)
#define LAL_CM2KM_F                        (1e-5f)
#define LAL_CM2M_F                         (1e-2f)
#define LAL_CM2MM_F                        (1e+1f)
#define LAL_MM2KM_F                        (1e-6f)
#define LAL_MM2M_F                         (1e-3f)
#define LAL_MM2CM_F                        (1e-1f)

#define LAL_KM2M_D                         (1e+3)
#define LAL_KM2CM_D                        (1e+5)
#define LAL_KM2MM_D                        (1e+6)
#define LAL_M2KM_D                         (1e-3)
#define LAL_M2CM_D                         (1e+2)
#define LAL_M2MM_D                         (1e+3)
#define LAL_CM2KM_D                        (1e-5)
#define LAL_CM2M_D                         (1e-2)
#define LAL_CM2MM_D                        (1e+1)
#define LAL_MM2KM_D                        (1e-6)
#define LAL_MM2M_D                         (1e-3)
#define LAL_MM2CM_D                        (1e-1)

///////////////////////////////////////////////////////////////////////////////
// Scalar conversion constants for squared metric units.
#define LAL_KM2_TO_M2_F                    (1e+6f)
#define LAL_KM2_TO_CM2_F                   (1e+10f)
#define LAL_KM2_TO_MM2_F                   (1e+12f)
#define LAL_M2_TO_KM2_F                    (1e-6f)
#define LAL_M2_TO_CM2_F                    (1e+4f)
#define LAL_M2_TO_MM2_F                    (1e+6f)
#define LAL_CM2_TO_KM2_F                   (1e-10f)
#define LAL_CM2_TO_M2_F                    (1e-4f)
#define LAL_CM2_TO_MM2_F                   (1e+2f)
#define LAL_MM2_TO_KM2_F                   (1e-12f)
#define LAL_MM2_TO_M2_F                    (1e-6f)
#define LAL_MM2_TO_CM2_F                   (1e-2f)

#define LAL_KM2_TO_M2_D                    (1e+6)
#define LAL_KM2_TO_CM2_D                   (1e+10)
#define LAL_KM2_TO_MM2_D                   (1e+12)
#define LAL_M2_TO_KM2_D                    (1e-6)
#define LAL_M2_TO_CM2_D                    (1e+4)
#define LAL_M2_TO_MM2_D                    (1e+6)
#define LAL_CM2_TO_KM2_D                   (1e-10)
#define LAL_CM2_TO_M2_D                    (1e-4)
#define LAL_CM2_TO_MM2_D                   (1e+2)
#define LAL_MM2_TO_KM2_D                   (1e-12)
#define LAL_MM2_TO_M2_D                    (1e-6)
#define LAL_MM2_TO_CM2_D                   (1e-2)

///////////////////////////////////////////////////////////////////////////////
// Scalar conversion constants for cubed metric units.
#define LAL_KM3_TO_M3_F                    (1e+9f)
#define LAL_KM3_TO_CM3_F                   (1e+15f)
#define LAL_KM3_TO_MM3_F                   (1e+18f)
#define LAL_M3_TO_KM3_F                    (1e-9f)
#define LAL_M3_TO_CM3_F                    (1e+6f)
#define LAL_M3_TO_MM3_F                    (1e+9f)
#define LAL_CM3_TO_KM3_F                   (1e-15f)
#define LAL_CM3_TO_M3_F                    (1e-6f)
#define LAL_CM3_TO_MM3_F                   (1e+3f)
#define LAL_MM3_TO_KM3_F                   (1e-18f)
#define LAL_MM3_TO_M3_F                    (1e-9f)
#define LAL_MM3_TO_CM3_F                   (1e-3f)

#define LAL_KM3_TO_M3_D                    (1e+9)
#define LAL_KM3_TO_CM3_D                   (1e+15)
#define LAL_KM3_TO_MM3_D                   (1e+18)
#define LAL_M3_TO_KM3_D                    (1e-9)
#define LAL_M3_TO_CM3_D                    (1e+6)
#define LAL_M3_TO_MM3_D                    (1e+9)
#define LAL_CM3_TO_KM3_D                   (1e-15)
#define LAL_CM3_TO_M3_D                    (1e-6)
#define LAL_CM3_TO_MM3_D                   (1e+3)
#define LAL_MM3_TO_KM3_D                   (1e-18)
#define LAL_MM3_TO_M3_D                    (1e-9)
#define LAL_MM3_TO_CM3_D                   (1e-3)

///////////////////////////////////////////////////////////////////////////////
// Time conversion constants.
#define LAL_NS2MUS_F                       (1e-3f)
#define LAL_NS2MS_F                        (1e-6f)
#define LAL_NS2S_F                         (1e-9f)
#define LAL_NS2M_F                         (1.6667e-11f)
#define LAL_NS2H_F                         (2.7778e-13f)
#define LAL_MUS2NS_F                       (1e+3f)
#define LAL_MUS2MS_F                       (1e-3f)
#define LAL_MUS2S_F                        (1e-6f)
#define LAL_MUS2M_F                        (1.6667e-8f)
#define LAL_MUS2H_F                        (2.7778e-10f)
#define LAL_MS2NS_F                        (1e+6f)
#define LAL_MS2MUS_F                       (1e+3f)
#define LAL_MS2S_F                         (1e-3f)
#define LAL_MS2M_F                         (1.6667e-5f)
#define LAL_MS2H_F                         (2.7778e-7f)
#define LAL_S2NS_F                         (1e+9f)
#define LAL_S2MUS_F                        (1e+6f)
#define LAL_S2MS_F                         (1e+3f)
#define LAL_S2M_F                          (1.f / 60.f)
#define LAL_S2H_F                          (1.f / 3600.f)
#define LAL_M2NS_F                         (6e+10f)
#define LAL_M2MUS_F                        (6e+7f)
#define LAL_M2MS_F                         (60000.0f)
#define LAL_M2S_F                          (60.0f)
#define LAL_M2H_F                          (0.016666666666666666666666666666667f)
#define LAL_H2NS_F                         (3.6e+12f)
#define LAL_H2MUS_F                        (3.6e+9f)
#define LAL_H2MS_F                         (3.6e+6f)
#define LAL_H2S_F                          (3600.0f)
#define LAL_H2M_F                          (60.0f)

#define LAL_NS2MUS_D                       (1e-3)
#define LAL_NS2MS_D                        (1e-6)
#define LAL_NS2S_D                         (1e-9)
#define LAL_NS2M_D                         (1.6667e-11)
#define LAL_NS2H_D                         (2.7778e-13)
#define LAL_MUS2NS_D                       (1e+3)
#define LAL_MUS2MS_D                       (1e-3)
#define LAL_MUS2S_D                        (1e-6)
#define LAL_MUS2M_D                        (1.6667e-8)
#define LAL_MUS2H_D                        (2.7778e-10)
#define LAL_MS2NS_D                        (1e+6)
#define LAL_MS2MUS_D                       (1e+3)
#define LAL_MS2S_D                         (1e-3)
#define LAL_MS2M_D                         (1.6667e-5)
#define LAL_MS2H_D                         (2.7778e-7)
#define LAL_S2NS_D                         (1e+9)
#define LAL_S2MUS_D                        (1e+6)
#define LAL_S2MS_D                         (1e+3)
#define LAL_S2M_D                          (1. / 60.)
#define LAL_S2H_D                          (1. / 3600.)
#define LAL_M2NS_D                         (6e+10)
#define LAL_M2MUS_D                        (6e+7)
#define LAL_M2MS_D                         (60000.0)
#define LAL_M2S_D                          (60.0)
#define LAL_M2H_D                          (0.016666666666666666666666666666667)
#define LAL_H2NS_D                         (3.6e+12)
#define LAL_H2MUS_D                        (3.6e+9)
#define LAL_H2MS_D                         (3.6e+6)
#define LAL_H2S_D                          (3600.0)
#define LAL_H2M_D                          (60.0)


/*----------------------------------------------------------------------------
    Aliases.
----------------------------------------------------------------------------*/

#define LAL_SMALL_NUMBER                   LAL_FLOAT_SMALL_NUMBER
#define LAL_NOT_SO_SMALL_NUMBER            LAL_FLOAT_NOT_SO_SMALL_NUMBER
#define LAL_PI                             LAL_PI_F
#define LAL_TWO_PI                         LAL_TWO_PI_F
#define LAL_HALF_PI                        LAL_HALF_PI_F
#define LAL_PI_SQRT                        LAL_PI_SQRT_F
#define LAL_PI_SQUARED                     LAL_PI_SQUARED_F
#define LAL_DEB_FULL_CIRCLE                LAL_DEG_FULL_CIRCLE_F
#define LAL_RAD_FULL_CIRCLE                LAL_RAD_FULL_CIRCLE_F
#define LAL_DEB_QUARTER_CIRCLE             LAL_DEG_QUARTER_CIRCLE_F
#define LAL_RAD_QUARTER_CIRCLE             LAL_RAD_QUARTER_CIRCLE_F
#define LAL_DEB_HALF_CIRCLE                LAL_DEG_HALF_CIRCLE_F
#define LAL_RAD_HALF_CIRCLE                LAL_RAD_HALF_CIRCLE_F
#define LAL_DEB_THREE_THIRD_CIRCLE         LAL_DEG_THREE_THIRD_CIRCLE_F
#define LAL_RAD_THREE_THIRD_CIRCLE         LAL_RAD_THREE_THIRD_CIRCLE_F
#define LAL_FROM_PERCENT                   LAL_FROM_PERCENT_F
#define LAL_TO_PERCENT                     LAL_TO_PERCENT_F
#define LAL_KM2M                           LAL_KM2M_F
#define LAL_KM2CM                          LAL_KM2CM_F
#define LAL_KM2MM                          LAL_KM2MM_F
#define LAL_M2KM                           LAL_M2KM_F
#define LAL_M2CM                           LAL_M2CM_F
#define LAL_M2MM                           LAL_M2MM_F
#define LAL_CM2KM                          LAL_CM2KM_F
#define LAL_CM2M                           LAL_CM2M_F
#define LAL_CM2MM                          LAL_CM2MM_F
#define LAL_MM2KM                          LAL_MM2KM_F
#define LAL_MM2M                           LAL_MM2M_F
#define LAL_MM2CM                          LAL_MM2CM_F
#define LAL_KM2_TO_M2                      LAL_KM2_TO_M2_F
#define LAL_KM2_TO_CM2                     LAL_KM2_TO_CM2_F
#define LAL_KM2_TO_MM2                     LAL_KM2_TO_MM2_F
#define LAL_M2_TO_KM2                      LAL_M2_TO_KM2_F
#define LAL_M2_TO_CM2                      LAL_M2_TO_CM2_F
#define LAL_M2_TO_MM2                      LAL_M2_TO_MM2_F
#define LAL_CM2_TO_KM2                     LAL_CM2_TO_KM2_F
#define LAL_CM2_TO_M2                      LAL_CM2_TO_M2_F
#define LAL_CM2_TO_MM2                     LAL_CM2_TO_MM2_F
#define LAL_MM2_TO_KM2                     LAL_MM2_TO_KM2_F
#define LAL_MM2_TO_M2                      LAL_MM2_TO_M2_F
#define LAL_MM2_TO_CM2                     LAL_MM2_TO_CM2_F
#define LAL_KM2_TO_M3                      LAL_KM3_TO_M3_F
#define LAL_KM2_TO_CM3                     LAL_KM3_TO_CM3_F
#define LAL_KM2_TO_MM3                     LAL_KM3_TO_MM3_F
#define LAL_M2_TO_KM3                      LAL_M3_TO_KM3_F
#define LAL_M2_TO_CM3                      LAL_M3_TO_CM3_F
#define LAL_M2_TO_MM3                      LAL_M3_TO_MM3_F
#define LAL_CM2_TO_KM3                     LAL_CM3_TO_KM3_F
#define LAL_CM2_TO_M3                      LAL_CM3_TO_M3_F
#define LAL_CM2_TO_MM3                     LAL_CM3_TO_MM3_F
#define LAL_MM2_TO_KM3                     LAL_MM3_TO_KM3_F
#define LAL_MM2_TO_M3                      LAL_MM3_TO_M3_F
#define LAL_MM2_TO_CM3                     LAL_MM3_TO_CM3_F
#define LAL_NS2MUS                         LAL_NS2MUS_F
#define LAL_NS2MS                          LAL_NS2MS_F
#define LAL_NS2S                           LAL_NS2S_F
#define LAL_NS2M                           LAL_NS2M_F
#define LAL_NS2H                           LAL_NS2H_F
#define LAL_MUS2NS                         LAL_MUS2NS_F
#define LAL_MUS2MS                         LAL_MUS2MS_F
#define LAL_MUS2S                          LAL_MUS2S_F
#define LAL_MUS2M                          LAL_MUS2M_F
#define LAL_MUS2H                          LAL_MUS2H_F
#define LAL_MS2NS                          LAL_MS2NS_F
#define LAL_MS2MUS                         LAL_MS2MUS_F
#define LAL_MS2S                           LAL_MS2S_F
#define LAL_MS2M                           LAL_MS2M_F
#define LAL_MS2H                           LAL_MS2H_F
#define LAL_S2NS                           LAL_S2NS_F
#define LAL_S2MUS                          LAL_S2MUS_F
#define LAL_S2MS                           LAL_S2MS_F
#define LAL_S2M                            LAL_S2M_F
#define LAL_S2H                            LAL_S2H_F
#define LAL_M2NS                           LAL_M2NS_F
#define LAL_M2MUS                          LAL_M2MUS_F
#define LAL_M2MS                           LAL_M2MS_F
#define LAL_M2S                            LAL_M2S_F
#define LAL_M2H                            LAL_M2H_F
#define LAL_H2NS                           LAL_H2NS_F
#define LAL_H2MUS                          LAL_H2MUS_F
#define LAL_H2MS                           LAL_H2MS_F
#define LAL_H2S                            LAL_H2S_F
#define LAL_H2M                            LAL_H2M_F
