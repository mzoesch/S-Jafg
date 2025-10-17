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

#ifndef LAL_LOG_DEFAULT_VERBOSITY
    #if IN_DEBUG
        #define LAL_LOG_DEFAULT_VERBOSITY               LAL_DEBUG_DEFAULT_LOG_VERBOSITY
    #elif IN_DEVELOPMENT
        #define LAL_LOG_DEFAULT_VERBOSITY               LAL_DEVELOPMENT_DEFAULT_LOG_VERBOSITY
    #elif IN_SHIPPING
        #define LAL_LOG_DEFAULT_VERBOSITY               LAL_SHIPPING_DEFAULT_LOG_VERBOSITY
    #else /* IN_SHIPPING */
        #error "Could not resolve build configuration."
    #endif  /* !IN_SHIPPING */
#endif /* !LAL_LOG_DEFAULT_VERBOSITY */

#ifndef LAL_LOG_DEFAULT_VERBOSITY_TASKS
    #if IN_DEBUG
        #define LAL_LOG_DEFAULT_VERBOSITY_TASKS         LAL_DEVELOPMENT_DEFAULT_LOG_VERBOSITY
    #elif IN_DEVELOPMENT
        #define LAL_LOG_DEFAULT_VERBOSITY_TASKS         LAL_SHIPPING_DEFAULT_LOG_VERBOSITY
    #elif IN_SHIPPING
        #define LAL_LOG_DEFAULT_VERBOSITY_TASKS         LAL_SHIPPING_DEFAULT_LOG_VERBOSITY
    #else /* IN_SHIPPING */
        #error "Could not resolve build configuration."
    #endif  /* !IN_SHIPPING */
#endif /* LAL_LOG_DEFAULT_VERBOSITY_TASKS */

DECLARE_INLINE_LOG_CATEGORY( LogCarnifex,                     LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogChunkGeneration,              LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogChunkManipulation,            LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogChunkMisc,                    LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogChunkValidation,              LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogClassOuter,                   LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogCli,                          LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogConfigIo,                     LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogController,                   LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogEgo,                          LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogEngine,                       LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogFontSubsystem,                LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogForeign,                      LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogFrontEnd,                     LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogGenPrevAssets,                LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogGuardedMain,                  LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogHyperlane,                    LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogJafgInternal,                 LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogLackey,                       LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogLowLevel,                     LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogMaterialSubsystem,            LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogNames,                        LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogNetwork,                      LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogObjectInternal,               LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogPackager,                     LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogPlatform,                     LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogPlatformMisc,                 LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogPreferences,                  LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogProgramArguments,             Warning                                               )
DECLARE_INLINE_LOG_CATEGORY( LogRecipeSystem,                 LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogReST,                         LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogRhi,                          LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogRunnable,                     LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogSerialization,                LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogStats,                        LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogStrikeSystem,                 LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogStorage,                      LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogSubsystemCollection,          LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogSurface,                      LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogSystem,                       LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTags,                         LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTasks,                        LAL_LOG_DEFAULT_VERBOSITY_TASKS                       )
DECLARE_INLINE_LOG_CATEGORY( LogTaskSystem,                   LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTaskUtility,                  LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTemporal,                     LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTesting,                      LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTestingFramework,             LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTextureSubsystem,             LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTime,                         LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogUserInput,                    LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogVoxelSubsystem,               LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogWidgetFramework,              LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogWidgets,                      LAL_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogWorld,                        LAL_LOG_DEFAULT_VERBOSITY                             )
