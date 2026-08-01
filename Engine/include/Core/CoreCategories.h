// Copyright mzoesch. All rights reserved.

#pragma once

#ifndef JAFG_DEBUG_DEFAULT_LOG_VERBOSITY
    #define JAFG_DEBUG_DEFAULT_LOG_VERBOSITY             Trace
#endif /* JAFG_DEBUG_DEFAULT_LOG_VERBOSITY */
#ifndef JAFG_DEVELOPMENT_DEFAULT_LOG_VERBOSITY
    #define JAFG_DEVELOPMENT_DEFAULT_LOG_VERBOSITY       Verbose
#endif /* JAFG_DEVELOPMENT_DEFAULT_LOG_VERBOSITY */
#ifndef JAFG_SHIPPING_DEFAULT_LOG_VERBOSITY
    #define JAFG_SHIPPING_DEFAULT_LOG_VERBOSITY          Info
#endif /* JAFG_SHIPPING_DEFAULT_LOG_VERBOSITY */

#ifndef JAFG_LOG_DEFAULT_VERBOSITY
    #if JAFG_IN_DEBUG
        #define JAFG_LOG_DEFAULT_VERBOSITY               JAFG_DEBUG_DEFAULT_LOG_VERBOSITY
    #elif JAFG_IN_DEVELOPMENT
        #define JAFG_LOG_DEFAULT_VERBOSITY               JAFG_DEVELOPMENT_DEFAULT_LOG_VERBOSITY
    #elif JAFG_IN_SHIPPING
        #define JAFG_LOG_DEFAULT_VERBOSITY               JAFG_SHIPPING_DEFAULT_LOG_VERBOSITY
    #else /* JAFG_IN_SHIPPING */
        #error "Could not resolve build configuration."
    #endif  /* !JAFG_IN_SHIPPING */
#endif /* !JAFG_LOG_DEFAULT_VERBOSITY */

#ifndef JAFG_LOG_DEFAULT_VERBOSITY_TASKS
    #if JAFG_IN_DEBUG
        #define JAFG_LOG_DEFAULT_VERBOSITY_TASKS         JAFG_DEVELOPMENT_DEFAULT_LOG_VERBOSITY
    #elif JAFG_IN_DEVELOPMENT
        #define JAFG_LOG_DEFAULT_VERBOSITY_TASKS         JAFG_SHIPPING_DEFAULT_LOG_VERBOSITY
    #elif JAFG_IN_SHIPPING
        #define JAFG_LOG_DEFAULT_VERBOSITY_TASKS         JAFG_SHIPPING_DEFAULT_LOG_VERBOSITY
    #else /* JAFG_IN_SHIPPING */
        #error "Could not resolve build configuration."
    #endif  /* !JAFG_IN_SHIPPING */
#endif /* JAFG_LOG_DEFAULT_VERBOSITY_TASKS */

DECLARE_INLINE_LOG_CATEGORY( LogAssetSubsystem,               JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogCarnifex,                     JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogChunkGeneration,              JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogChunkManipulation,            JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogChunkMisc,                    JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogChunkValidation,              JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogClassOuter,                   JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogCli,                          JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogConfigIo,                     JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogController,                   JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogEcs,                          JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogEditor,                       JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogEgo,                          JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogEngine,                       JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogFontSubsystem,                JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogForeign,                      JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogFrontend,                     JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogGenPrevAssets,                JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogHyperlane,                    JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogInformation,                  JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogJafgInternal,                 JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogLackey,                       JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogLaunch,                       JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogLowLevel,                     JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogMaterialSubsystem,            JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogNames,                        JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogNetwork,                      JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogObjectInternal,               JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogPackager,                     JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogPlatform,                     JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogPlatformMisc,                 JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogPreferences,                  JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogProgramArguments,             Warning                                                )
DECLARE_INLINE_LOG_CATEGORY( LogRecipeSystem,                 JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogReST,                         JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogRhi,                          JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogRunnable,                     JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogSerialization,                JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogShaderSubsystem,              JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogStats,                        JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogStrikeSystem,                 JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogStorage,                      JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogSubsystemCollection,          JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogSurface,                      JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogSystem,                       JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTags,                         JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTasks,                        JAFG_LOG_DEFAULT_VERBOSITY_TASKS                       )
DECLARE_INLINE_LOG_CATEGORY( LogTaskSystem,                   JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTaskUtility,                  JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTemporal,                     JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTesting,                      JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTestingFramework,             JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTextureSubsystem,             JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogTime,                         JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogUserInput,                    JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogVoxelSubsystem,               JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogVulkan,                       JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogWidgetFramework,              JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogWidgets,                      JAFG_LOG_DEFAULT_VERBOSITY                             )
DECLARE_INLINE_LOG_CATEGORY( LogWorld,                        JAFG_LOG_DEFAULT_VERBOSITY                             )
