// Copyright mzoesch. All rights reserved.

#pragma once

/*----------------------------------------------------------------------------
    Floating point constants.
----------------------------------------------------------------------------*/

///////////////////////////////////////////////////////////////////////////////
// Thresholds.
#define JAFG_THRESHOLD_NORM_VEC_F           (1.e-2f) /** Tolerance flt constant for norm check with squared magnitude of a vector. */
#define JAFG_THRESHOLD_NORM_VEC_D           (1.e-2)  /** Tolerance dbl constant for norm check with squared magnitude of a vector. */

///////////////////////////////////////////////////////////////////////////////
// Common constants.
#define JAFG_FLOAT_SMALL_NUMBER             (1.e-8f)
#define JAFG_FLOAT_NOT_SO_SMALL_NUMBER      (1.e-4f)

#define JAFG_DOUBLE_SMALL_NUMBER            (1.e-8)
#define JAFG_DOUBLE_NOT_SO_SMALL_NUMBER     (1.e-4)

#define JAFG_PI_F                           (3.1415926535897932384626433832795028841971693993751058209749445923078164f)
#define JAFG_HALF_PI_F                      (1.5707963267948966192313216916397514420985846996875529104874722961539082f)
#define JAFG_TWO_PI_F                       (6.2831853071795864769252867665590057683943387987502116419498891846156328f)
#define JAFG_PI_SQRT_F                      (1.7724538509055160272981674833411451827975494561223871282138077898529112f)
#define JAFG_PI_SQUARED_F                   (9.8696044010893586188344909998761511353136994072407906264133493762200448f)
#define JAFG_PI_D                           (3.1415926535897932384626433832795028841971693993751058209749445923078164 )
#define JAFG_HALF_PI_D                      (1.5707963267948966192313216916397514420985846996875529104874722961539082 )
#define JAFG_TWO_PI_D                       (6.2831853071795864769252867665590057683943387987502116419498891846156328 )
#define JAFG_PI_SQRT_D                      (1.7724538509055160272981674833411451827975494561223871282138077898529112 )
#define JAFG_PI_SQUARED_D                   (9.8696044010893586188344909998761511353136994072407906264133493762200448 )

#define JAFG_DEG_FULL_CIRCLE_F              (360.0f)
#define JAFG_RAD_FULL_CIRCLE_F              (JAFG_TWO_PI_F)
#define JAFG_DEG_QUARTER_CIRCLE_F           (90.0f)
#define JAFG_RAD_QUARTER_CIRCLE_F           (JAFG_HALF_PI_F)
#define JAFG_DEG_HALF_CIRCLE_F              (180.0f)
#define JAFG_RAD_HALF_CIRCLE_F              (JAFG_PI_F)
#define JAFG_DEG_THREE_THIRD_CIRCLE_F       (270.0f)
#define JAFG_RAD_THREE_THIRD_CIRCLE_F       (4.7123889803846898576939650749192543262957540990626587314624168884617246f)
#define JAFG_DEG_FULL_CIRCLE_D              (360.0)
#define JAFG_RAD_FULL_CIRCLE_D              (JAFG_TWO_PI_D)
#define JAFG_DEG_QUARTER_CIRCLE_D           (90.0)
#define JAFG_RAD_QUARTER_CIRCLE_D           (JAFG_HALF_PI_D)
#define JAFG_DEG_HALF_CIRCLE_D              (180.0)
#define JAFG_RAD_HALF_CIRCLE_D              (JAFG_PI_D)
#define JAFG_DEG_THREE_THIRD_CIRCLE_D       (270.0)
#define JAFG_RAD_THREE_THIRD_CIRCLE_D       (4.7123889803846898576939650749192543262957540990626587314624168884617246 )

#define JAFG_FLOAT_SUPREMUM                 (3.402823466e+38f)          /* Max positive flt value. */
#define JAFG_FLOAT_INFIMUM                  (1.175494351e-38f)          /* Min normalized positive flt value. */
#define JAFG_FLOAT_TRUE_MINIMUM             (1.401298464e-45f)          /* Min positive flt value. */
#define JAFG_DOUBLE_SUPREMUM                (1.7976931348623158e+308)   /* Max positive dbl value. */
#define JAFG_DOUBLE_INFIMUM                 (2.2250738585072014e-308)   /* Min normalized positive dbl value. */
#define JAFG_DOUBLE_TRUE_MINIMUM            (4.9406564584124654e-324)   /* Min positive dbl value. */

#define JAFG_FROM_PERCENT_F                 (0.01f)
#define JAFG_FROM_PERCENT_D                 (0.01 )
#define JAFG_TO_PERCENT_F                   (100.0f)
#define JAFG_TO_PERCENT_D                   (100.0 )

///////////////////////////////////////////////////////////////////////////////
// Scalar conversion constants for metric units.
#define JAFG_KM2M_F                         (1e+3f)
#define JAFG_KM2CM_F                        (1e+5f)
#define JAFG_KM2MM_F                        (1e+6f)
#define JAFG_M2KM_F                         (1e-3f)
#define JAFG_M2CM_F                         (1e+2f)
#define JAFG_M2MM_F                         (1e+3f)
#define JAFG_CM2KM_F                        (1e-5f)
#define JAFG_CM2M_F                         (1e-2f)
#define JAFG_CM2MM_F                        (1e+1f)
#define JAFG_MM2KM_F                        (1e-6f)
#define JAFG_MM2M_F                         (1e-3f)
#define JAFG_MM2CM_F                        (1e-1f)

#define JAFG_KM2M_D                         (1e+3)
#define JAFG_KM2CM_D                        (1e+5)
#define JAFG_KM2MM_D                        (1e+6)
#define JAFG_M2KM_D                         (1e-3)
#define JAFG_M2CM_D                         (1e+2)
#define JAFG_M2MM_D                         (1e+3)
#define JAFG_CM2KM_D                        (1e-5)
#define JAFG_CM2M_D                         (1e-2)
#define JAFG_CM2MM_D                        (1e+1)
#define JAFG_MM2KM_D                        (1e-6)
#define JAFG_MM2M_D                         (1e-3)
#define JAFG_MM2CM_D                        (1e-1)

///////////////////////////////////////////////////////////////////////////////
// Scalar conversion constants for squared metric units.
#define JAFG_KM2_TO_M2_F                    (1e+6f)
#define JAFG_KM2_TO_CM2_F                   (1e+10f)
#define JAFG_KM2_TO_MM2_F                   (1e+12f)
#define JAFG_M2_TO_KM2_F                    (1e-6f)
#define JAFG_M2_TO_CM2_F                    (1e+4f)
#define JAFG_M2_TO_MM2_F                    (1e+6f)
#define JAFG_CM2_TO_KM2_F                   (1e-10f)
#define JAFG_CM2_TO_M2_F                    (1e-4f)
#define JAFG_CM2_TO_MM2_F                   (1e+2f)
#define JAFG_MM2_TO_KM2_F                   (1e-12f)
#define JAFG_MM2_TO_M2_F                    (1e-6f)
#define JAFG_MM2_TO_CM2_F                   (1e-2f)

#define JAFG_KM2_TO_M2_D                    (1e+6)
#define JAFG_KM2_TO_CM2_D                   (1e+10)
#define JAFG_KM2_TO_MM2_D                   (1e+12)
#define JAFG_M2_TO_KM2_D                    (1e-6)
#define JAFG_M2_TO_CM2_D                    (1e+4)
#define JAFG_M2_TO_MM2_D                    (1e+6)
#define JAFG_CM2_TO_KM2_D                   (1e-10)
#define JAFG_CM2_TO_M2_D                    (1e-4)
#define JAFG_CM2_TO_MM2_D                   (1e+2)
#define JAFG_MM2_TO_KM2_D                   (1e-12)
#define JAFG_MM2_TO_M2_D                    (1e-6)
#define JAFG_MM2_TO_CM2_D                   (1e-2)

///////////////////////////////////////////////////////////////////////////////
// Scalar conversion constants for cubed metric units.
#define JAFG_KM3_TO_M3_F                    (1e+9f)
#define JAFG_KM3_TO_CM3_F                   (1e+15f)
#define JAFG_KM3_TO_MM3_F                   (1e+18f)
#define JAFG_M3_TO_KM3_F                    (1e-9f)
#define JAFG_M3_TO_CM3_F                    (1e+6f)
#define JAFG_M3_TO_MM3_F                    (1e+9f)
#define JAFG_CM3_TO_KM3_F                   (1e-15f)
#define JAFG_CM3_TO_M3_F                    (1e-6f)
#define JAFG_CM3_TO_MM3_F                   (1e+3f)
#define JAFG_MM3_TO_KM3_F                   (1e-18f)
#define JAFG_MM3_TO_M3_F                    (1e-9f)
#define JAFG_MM3_TO_CM3_F                   (1e-3f)

#define JAFG_KM3_TO_M3_D                    (1e+9)
#define JAFG_KM3_TO_CM3_D                   (1e+15)
#define JAFG_KM3_TO_MM3_D                   (1e+18)
#define JAFG_M3_TO_KM3_D                    (1e-9)
#define JAFG_M3_TO_CM3_D                    (1e+6)
#define JAFG_M3_TO_MM3_D                    (1e+9)
#define JAFG_CM3_TO_KM3_D                   (1e-15)
#define JAFG_CM3_TO_M3_D                    (1e-6)
#define JAFG_CM3_TO_MM3_D                   (1e+3)
#define JAFG_MM3_TO_KM3_D                   (1e-18)
#define JAFG_MM3_TO_M3_D                    (1e-9)
#define JAFG_MM3_TO_CM3_D                   (1e-3)

///////////////////////////////////////////////////////////////////////////////
// Time conversion constants.
#define JAFG_NS2MUS_F                       (1e-3f)
#define JAFG_NS2MS_F                        (1e-6f)
#define JAFG_NS2S_F                         (1e-9f)
#define JAFG_NS2M_F                         (1.6667e-11f)
#define JAFG_NS2H_F                         (2.7778e-13f)
#define JAFG_MUS2NS_F                       (1e+3f)
#define JAFG_MUS2MS_F                       (1e-3f)
#define JAFG_MUS2S_F                        (1e-6f)
#define JAFG_MUS2M_F                        (1.6667e-8f)
#define JAFG_MUS2H_F                        (2.7778e-10f)
#define JAFG_MS2NS_F                        (1e+6f)
#define JAFG_MS2MUS_F                       (1e+3f)
#define JAFG_MS2S_F                         (1e-3f)
#define JAFG_MS2M_F                         (1.6667e-5f)
#define JAFG_MS2H_F                         (2.7778e-7f)
#define JAFG_S2NS_F                         (1e+9f)
#define JAFG_S2MUS_F                        (1e+6f)
#define JAFG_S2MS_F                         (1e+3f)
#define JAFG_S2M_F                          (1.f / 60.f)
#define JAFG_S2H_F                          (1.f / 3600.f)
#define JAFG_M2NS_F                         (6e+10f)
#define JAFG_M2MUS_F                        (6e+7f)
#define JAFG_M2MS_F                         (60000.0f)
#define JAFG_M2S_F                          (60.0f)
#define JAFG_M2H_F                          (0.016666666666666666666666666666667f)
#define JAFG_H2NS_F                         (3.6e+12f)
#define JAFG_H2MUS_F                        (3.6e+9f)
#define JAFG_H2MS_F                         (3.6e+6f)
#define JAFG_H2S_F                          (3600.0f)
#define JAFG_H2M_F                          (60.0f)

#define JAFG_NS2MUS_D                       (1e-3)
#define JAFG_NS2MS_D                        (1e-6)
#define JAFG_NS2S_D                         (1e-9)
#define JAFG_NS2M_D                         (1.6667e-11)
#define JAFG_NS2H_D                         (2.7778e-13)
#define JAFG_MUS2NS_D                       (1e+3)
#define JAFG_MUS2MS_D                       (1e-3)
#define JAFG_MUS2S_D                        (1e-6)
#define JAFG_MUS2M_D                        (1.6667e-8)
#define JAFG_MUS2H_D                        (2.7778e-10)
#define JAFG_MS2NS_D                        (1e+6)
#define JAFG_MS2MUS_D                       (1e+3)
#define JAFG_MS2S_D                         (1e-3)
#define JAFG_MS2M_D                         (1.6667e-5)
#define JAFG_MS2H_D                         (2.7778e-7)
#define JAFG_S2NS_D                         (1e+9)
#define JAFG_S2MUS_D                        (1e+6)
#define JAFG_S2MS_D                         (1e+3)
#define JAFG_S2M_D                          (1. / 60.)
#define JAFG_S2H_D                          (1. / 3600.)
#define JAFG_M2NS_D                         (6e+10)
#define JAFG_M2MUS_D                        (6e+7)
#define JAFG_M2MS_D                         (60000.0)
#define JAFG_M2S_D                          (60.0)
#define JAFG_M2H_D                          (0.016666666666666666666666666666667)
#define JAFG_H2NS_D                         (3.6e+12)
#define JAFG_H2MUS_D                        (3.6e+9)
#define JAFG_H2MS_D                         (3.6e+6)
#define JAFG_H2S_D                          (3600.0)
#define JAFG_H2M_D                          (60.0)


/*----------------------------------------------------------------------------
    Aliases.
----------------------------------------------------------------------------*/

#define JAFG_SMALL_NUMBER                   JAFG_FLOAT_SMALL_NUMBER
#define JAFG_NOT_SO_SMALL_NUMBER            JAFG_FLOAT_NOT_SO_SMALL_NUMBER
#define JAFG_PI                             JAFG_PI_F
#define JAFG_TWO_PI                         JAFG_TWO_PI_F
#define JAFG_HALF_PI                        JAFG_HALF_PI_F
#define JAFG_PI_SQRT                        JAFG_PI_SQRT_F
#define JAFG_PI_SQUARED                     JAFG_PI_SQUARED_F
#define JAFG_DEB_FULL_CIRCLE                JAFG_DEG_FULL_CIRCLE_F
#define JAFG_RAD_FULL_CIRCLE                JAFG_RAD_FULL_CIRCLE_F
#define JAFG_DEB_QUARTER_CIRCLE             JAFG_DEG_QUARTER_CIRCLE_F
#define JAFG_RAD_QUARTER_CIRCLE             JAFG_RAD_QUARTER_CIRCLE_F
#define JAFG_DEB_HALF_CIRCLE                JAFG_DEG_HALF_CIRCLE_F
#define JAFG_RAD_HALF_CIRCLE                JAFG_RAD_HALF_CIRCLE_F
#define JAFG_DEB_THREE_THIRD_CIRCLE         JAFG_DEG_THREE_THIRD_CIRCLE_F
#define JAFG_RAD_THREE_THIRD_CIRCLE         JAFG_RAD_THREE_THIRD_CIRCLE_F
#define JAFG_FROM_PERCENT                   JAFG_FROM_PERCENT_F
#define JAFG_TO_PERCENT                     JAFG_TO_PERCENT_F
#define JAFG_KM2M                           JAFG_KM2M_F
#define JAFG_KM2CM                          JAFG_KM2CM_F
#define JAFG_KM2MM                          JAFG_KM2MM_F
#define JAFG_M2KM                           JAFG_M2KM_F
#define JAFG_M2CM                           JAFG_M2CM_F
#define JAFG_M2MM                           JAFG_M2MM_F
#define JAFG_CM2KM                          JAFG_CM2KM_F
#define JAFG_CM2M                           JAFG_CM2M_F
#define JAFG_CM2MM                          JAFG_CM2MM_F
#define JAFG_MM2KM                          JAFG_MM2KM_F
#define JAFG_MM2M                           JAFG_MM2M_F
#define JAFG_MM2CM                          JAFG_MM2CM_F
#define JAFG_KM2_TO_M2                      JAFG_KM2_TO_M2_F
#define JAFG_KM2_TO_CM2                     JAFG_KM2_TO_CM2_F
#define JAFG_KM2_TO_MM2                     JAFG_KM2_TO_MM2_F
#define JAFG_M2_TO_KM2                      JAFG_M2_TO_KM2_F
#define JAFG_M2_TO_CM2                      JAFG_M2_TO_CM2_F
#define JAFG_M2_TO_MM2                      JAFG_M2_TO_MM2_F
#define JAFG_CM2_TO_KM2                     JAFG_CM2_TO_KM2_F
#define JAFG_CM2_TO_M2                      JAFG_CM2_TO_M2_F
#define JAFG_CM2_TO_MM2                     JAFG_CM2_TO_MM2_F
#define JAFG_MM2_TO_KM2                     JAFG_MM2_TO_KM2_F
#define JAFG_MM2_TO_M2                      JAFG_MM2_TO_M2_F
#define JAFG_MM2_TO_CM2                     JAFG_MM2_TO_CM2_F
#define JAFG_KM2_TO_M3                      JAFG_KM3_TO_M3_F
#define JAFG_KM2_TO_CM3                     JAFG_KM3_TO_CM3_F
#define JAFG_KM2_TO_MM3                     JAFG_KM3_TO_MM3_F
#define JAFG_M2_TO_KM3                      JAFG_M3_TO_KM3_F
#define JAFG_M2_TO_CM3                      JAFG_M3_TO_CM3_F
#define JAFG_M2_TO_MM3                      JAFG_M3_TO_MM3_F
#define JAFG_CM2_TO_KM3                     JAFG_CM3_TO_KM3_F
#define JAFG_CM2_TO_M3                      JAFG_CM3_TO_M3_F
#define JAFG_CM2_TO_MM3                     JAFG_CM3_TO_MM3_F
#define JAFG_MM2_TO_KM3                     JAFG_MM3_TO_KM3_F
#define JAFG_MM2_TO_M3                      JAFG_MM3_TO_M3_F
#define JAFG_MM2_TO_CM3                     JAFG_MM3_TO_CM3_F
#define JAFG_NS2MUS                         JAFG_NS2MUS_F
#define JAFG_NS2MS                          JAFG_NS2MS_F
#define JAFG_NS2S                           JAFG_NS2S_F
#define JAFG_NS2M                           JAFG_NS2M_F
#define JAFG_NS2H                           JAFG_NS2H_F
#define JAFG_MUS2NS                         JAFG_MUS2NS_F
#define JAFG_MUS2MS                         JAFG_MUS2MS_F
#define JAFG_MUS2S                          JAFG_MUS2S_F
#define JAFG_MUS2M                          JAFG_MUS2M_F
#define JAFG_MUS2H                          JAFG_MUS2H_F
#define JAFG_MS2NS                          JAFG_MS2NS_F
#define JAFG_MS2MUS                         JAFG_MS2MUS_F
#define JAFG_MS2S                           JAFG_MS2S_F
#define JAFG_MS2M                           JAFG_MS2M_F
#define JAFG_MS2H                           JAFG_MS2H_F
#define JAFG_S2NS                           JAFG_S2NS_F
#define JAFG_S2MUS                          JAFG_S2MUS_F
#define JAFG_S2MS                           JAFG_S2MS_F
#define JAFG_S2M                            JAFG_S2M_F
#define JAFG_S2H                            JAFG_S2H_F
#define JAFG_M2NS                           JAFG_M2NS_F
#define JAFG_M2MUS                          JAFG_M2MUS_F
#define JAFG_M2MS                           JAFG_M2MS_F
#define JAFG_M2S                            JAFG_M2S_F
#define JAFG_M2H                            JAFG_M2H_F
#define JAFG_H2NS                           JAFG_H2NS_F
#define JAFG_H2MUS                          JAFG_H2MUS_F
#define JAFG_H2MS                           JAFG_H2MS_F
#define JAFG_H2S                            JAFG_H2S_F
#define JAFG_H2M                            JAFG_H2M_F
