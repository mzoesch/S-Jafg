// Copyright mzoesch. All rights reserved.

#pragma once

#include <spdlog/spdlog.h>

#define J_TRACE       spdlog::level::level_enum::trace
#define J_DEBUG       spdlog::level::level_enum::debug
#define J_DISPLAY     spdlog::level::level_enum::info
#define J_WARNING     spdlog::level::level_enum::warn
#define J_ERROR       spdlog::level::level_enum::err
#define J_FATAL       spdlog::level::level_enum::critical

enum ELogVerbosity : uint8
{
    Trace           = J_TRACE     ,
    Debug           = J_DEBUG     ,
    Display        = J_DISPLAY   ,
    Warning         = J_WARNING   ,
    Error           = J_ERROR     ,
    Fatal           = J_FATAL     ,
};
