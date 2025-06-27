// Copyright mzoesch. All rights reserved.

#pragma once

#ifndef LAL_DEBUG_DEFAULT_LOG_VERBOSITY
    #define LAL_DEBUG_DEFAULT_LOG_VERBOSITY             Trace
#endif /* LAL_DEBUG_DEFAULT_LOG_VERBOSITY */
#ifndef LAL_DEVELOPMENT_DEFAULT_LOG_VERBOSITY
    #define LAL_DEVELOPMENT_DEFAULT_LOG_VERBOSITY       Verbose
#endif /* LAL_DEVELOPMENT_DEFAULT_LOG_VERBOSITY */
#ifndef LAL_SHIPPING_DEFAULT_LOG_VERBOSITY
    #define LAL_SHIPPING_DEFAULT_LOG_VERBOSITY          Info
#endif /* LAL_SHIPPING_DEFAULT_LOG_VERBOSITY */

#ifndef LAL_DEFAULT_LOG_VERBOSITY
    #if IN_DEBUG
        #define LAL_DEFAULT_LOG_VERBOSITY               LAL_DEBUG_DEFAULT_LOG_VERBOSITY
    #elif IN_DEVELOPMENT
        #define LAL_DEFAULT_LOG_VERBOSITY               LAL_DEVELOPMENT_DEFAULT_LOG_VERBOSITY
    #elif IN_SHIPPING
        #define LAL_DEFAULT_LOG_VERBOSITY               LAL_SHIPPING_DEFAULT_LOG_VERBOSITY
    #else /* IN_SHIPPING */
        #error "Could not resolve build configuration."
    #endif  /* !IN_SHIPPING */
#endif /* !LAL_DEFAULT_LOG_VERBOSITY */

#ifndef LAL_DEFAULT_LOG_VERBOSITY_TASKS
    #if IN_DEBUG
        #define LAL_DEFAULT_LOG_VERBOSITY_TASKS         LAL_DEVELOPMENT_DEFAULT_LOG_VERBOSITY
    #elif IN_DEVELOPMENT
        #define LAL_DEFAULT_LOG_VERBOSITY_TASKS         LAL_SHIPPING_DEFAULT_LOG_VERBOSITY
    #elif IN_SHIPPING
        #define LAL_DEFAULT_LOG_VERBOSITY_TASKS         LAL_SHIPPING_DEFAULT_LOG_VERBOSITY
    #else /* IN_SHIPPING */
        #error "Could not resolve build configuration."
    #endif  /* !IN_SHIPPING */
#endif /* LAL_DEFAULT_LOG_VERBOSITY_TASKS */

DECLARE_INLINE_LOG_CATEGORY( LogCarnifex,                     LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogChunkGeneration,              LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogChunkManipulation,            LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogChunkMisc,                    LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogChunkValidation,              LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogCli,                          LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogConfigIo,                     LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogController,                   LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogEgo,                          LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogEngine,                       LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogFontSubsystem,                LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogForeign,                      LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogFrontEnd,                     LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogGenPrevAssets,                LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogGuardedMain,                  LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogHyperlane,                    LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogJafgInternal,                 LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogLackey,                       LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogMaterialSubsystem,            LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogNames,                        LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogNetwork,                      LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogObjectInternal,               LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogObjectPackager,               LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogPlatform,                     LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogPlatformMisc,                 LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogPreferences,                  LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogRecipeSystem,                 LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogRhi,                          LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogRunnable,                     LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogStats,                        LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogStrikeSystem,                 LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogStorage,                      LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogSubsystemCollection,          LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogSurface,                      LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogSystem,                       LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTasks,                        LAL_DEFAULT_LOG_VERBOSITY_TASKS                       )
DECLARE_INLINE_LOG_CATEGORY( LogTaskSystem,                   LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTaskUtility,                  LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTemporal,                     LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTesting,                      LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTestingFramework,             LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTextureSubsystem,             LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTime,                         LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogUserInput,                    LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogVoxelSubsystem,               LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogWidgetFramework,              LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogWidgets,                      LAL_DEFAULT_LOG_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogWorld,                        LAL_DEFAULT_LOG_VERBOSITY                             )
