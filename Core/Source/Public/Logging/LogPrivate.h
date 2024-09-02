// Copyright mzoesch. All rights reserved.

#pragma once

#include "LogVerbosity.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace JafgLog::Private
{

struct LogCategory;

typedef spdlog::logger                      TLogger;
typedef spdlog::level::level_enum           TLogLevel;
typedef spdlog::sinks::stdout_color_sink_mt TConsoleSink;
typedef spdlog::sinks::basic_file_sink_mt   TFileSink;

template<typename ... T>
using   TStringFormat                     = spdlog::format_string_t<T...>;

extern class JafgLogInstance* GJafgLogInstance;
class JafgLogInstance
{
public:

    JafgLogInstance();
    ~JafgLogInstance();

    bool RegisterNewLogger(const String& Category, const TLogLevel Level);
    bool RegisterNewLogger(const String& Category, const ELogVerbosity Level);
    template <typename T = LogCategory>
    bool RegisterLogCategory();
    /** If global application state is able to log. */
    template <typename T = LogCategory>
    FORCEINLINE static bool TryRegisterLogCategory();

    bool UnregisterLogger(const String& Category);

    template <typename... T>
    FORCEINLINE void LogMessage(const String& Category, const TLogLevel Level, TStringFormat<T...> Format, T&&... Args);
    /** If global application state is able to log. */
    template <typename... T>
    FORCEINLINE static void TryLogMessage(const String& Category, const TLogLevel Level, TStringFormat<T...> Format, T&&... Args);

    static void UnsafeFlushEverythingFast();

private:

    std::shared_ptr<TConsoleSink> ConsoleSink = nullptr;
    std::shared_ptr<TFileSink>    FileSink    = nullptr;
};

struct LogCategory
{
protected:

    LogCategory()  = default;
    ~LogCategory() = default;

private:

    /** Let child handle the implementation of the static "virtual" method. */
    static ELogVerbosity GetCompileTimeVerbosity() { return Trace; }
    /** Let child handle the implementation of the static "virtual" method. */
    static const String& GetCategoryName() { static const String C = "LogUndeterminedCategory"; return C; }
};

template <typename T>
bool JafgLogInstance::RegisterLogCategory()
{
    return this->RegisterNewLogger(T::GetCategoryName(), T::GetCompileTimeVerbosity());
}

template <typename T>
bool JafgLogInstance::TryRegisterLogCategory()
{
    if (GJafgLogInstance)
    {
        return GJafgLogInstance->RegisterLogCategory<T>();
    }

    return false;
}

template <typename ... T>
void JafgLogInstance::LogMessage(const String& Category, const TLogLevel Level, TStringFormat<T...> Format, T&&... Args)
{
    spdlog::get(Category)->log(Level, Format, std::forward<T>(Args)...);
}

template <typename ... T>
void JafgLogInstance::TryLogMessage(const String& Category, const TLogLevel Level, TStringFormat<T...> Format, T&&... Args)
{
    if (GJafgLogInstance)
    {
        GJafgLogInstance->LogMessage(Category, Level, Format, std::forward<T>(Args)...);
    }

    return;
}

auto Init() -> EPlatformExit::Type;
auto TearDown() -> void;

}
