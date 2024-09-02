// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Logging/LogPrivate.h"
#include "HAL/Paths.h"

bool bInitialized = false;

JafgLog::Private::JafgLogInstance* JafgLog::Private::GJafgLogInstance = nullptr;

JafgLog::Private::JafgLogInstance::JafgLogInstance()
{
    if (GJafgLogInstance)
    {
        delete this;
        return;
    }

    GJafgLogInstance = this;

    this->ConsoleSink = std::make_shared<TConsoleSink>();
    this->ConsoleSink->set_pattern("[%M.%S][%t]%n: %L: %v");
    this->FileSink    = std::make_shared<TFileSink>(Paths::Combine(Paths::EngineRootDir(), "Jafg.log"));
    this->FileSink->set_pattern("[%Y.%m.%d-%H.%M.%S][%t]%n: %l: %v");

    return;
}

JafgLog::Private::JafgLogInstance::~JafgLogInstance()
{
    if (GJafgLogInstance != this)
    {
        return;
    }

    spdlog::apply_all( [] (const std::shared_ptr<TLogger>& Logger)
    {
        Logger->flush();
    });

    spdlog::drop_all();

    if (GJafgLogInstance)
    {
        GJafgLogInstance = nullptr;
    }

    return;
}

bool JafgLog::Private::JafgLogInstance::RegisterNewLogger(const String& Category, const TLogLevel Level)
{
    if (spdlog::get(Category))
    {
        return false;
    }

    const std::shared_ptr<TLogger> Logger = std::make_shared<TLogger>(Category);

    spdlog::register_logger(Logger);

    Logger->set_level(Level);
    Logger->sinks().emplace_back(this->ConsoleSink);
    Logger->sinks().emplace_back(this->FileSink);

    return true;
}

bool JafgLog::Private::JafgLogInstance::RegisterNewLogger(const String& Category, const ELogVerbosity Level)
{
    return this->RegisterNewLogger(Category, static_cast<TLogLevel>(Level));
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool JafgLog::Private::JafgLogInstance::UnregisterLogger(const String& Category)
{
    /*
     * We do not want to make this static because if this singleton is destroyed, the application will be in a state
     * to not log any messages.
     */

    if (spdlog::get(Category) == nullptr)
    {
        return false;
    }

    spdlog::drop(Category);

    return true;
}

EPlatformExit::Type JafgLog::Private::Init()
{
    if (bInitialized)
    {
        return EPlatformExit::Fatal;
    }
    bInitialized = true;

    new JafgLogInstance();

    return EPlatformExit::Success;
}

void JafgLog::Private::TearDown()
{
    if (GJafgLogInstance)
    {
        delete GJafgLogInstance;
        GJafgLogInstance = nullptr;
    }

    return;
}
