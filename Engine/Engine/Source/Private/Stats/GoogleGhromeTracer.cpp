// Copyright mzoesch. All rights reserved.

#include <bits/fs_path.h>
#if WITH_STATS && JAFG_STATS_USE_GOOGLE_CHROME_TRACER

#include "Stats/GoogleChromeTracer.h"
#include "Stats/StatsCore.h"
#include "System/Finder.h"
#include "System/EnginePath.h"

Jafg::Stats::Vendor::LGoogleChromeTracer::~LGoogleChromeTracer()
{
    if (this->Session.IsValid())
    {
        this->EndSession();
    }

    return;
}

void Jafg::Stats::Vendor::LGoogleChromeTracer::BeginSession(LString&& InName)
{
    std::unique_lock Lock(this->Mutex);

    check( this->Session.IsValid() == false )
    this->Session.Name = std::move(InName);

    const LString Path = this->GetPath();

    // Do not use finder, as it may not exist yet.
    const std::filesystem::path P{Path.ToC()};
    std::filesystem::create_directories(P.parent_path());

    this->Stream.open(Path.ToC(), std::ios::out | std::ios::trunc);
    this->Stream << R"({"otherData": {},"traceEvents":[)";

    return;
}

void Jafg::Stats::Vendor::LGoogleChromeTracer::AddEvent(Private::LSessionEvent&& InEvent)
{
    std::unique_lock Lock(this->Mutex);

    checkSlow( this->Session.IsValid() )

    if (this->bFirstEvent == false)
    {
        this->Stream << ',';
    }
    else
    {
        this->bFirstEvent = false;
    }

    this->Stream << R"({"cat":"function","dur":)";
    this->Stream << InEvent.EndTime - InEvent.BeginTime;
    this->Stream << R"(,"name":")";
    this->Stream << InEvent.Name;
    this->Stream << R"(","ph":"X","pid":0,"tid":)";
    this->Stream << InEvent.ThreadId;
    this->Stream << ",\"ts\":";
    this->Stream << InEvent.BeginTime;
    this->Stream << "}";

    return;
}

void Jafg::Stats::Vendor::LGoogleChromeTracer::TryEndSession()
{
    if (this->Session.IsValid())
    {
        this->EndSession();
    }

    return;
}

void Jafg::Stats::Vendor::LGoogleChromeTracer::EndSession()
{
    std::unique_lock Lock(this->Mutex);

    check( this->Session.IsValid() )

    this->Stream << "]}";
    this->Stream.flush();
    this->Stream.close();

    if (this->Stream.fail())
    {
        LOG_ERROR(LogStats, "Failed to write file: [{}].", this->GetPath().ToC())
    }

    this->Session.Name.Empty();

    this->bFirstEvent = true;

    return;
}

Jafg::LString Jafg::Stats::Vendor::LGoogleChromeTracer::GetPath() const
{
    check( this->Session.IsValid() )

    LPath Path = Finder::GetSavedDir();
    Path /= "GoogleChrome";
    Path /= this->Session.Name.ToC();
    Path.AddExtension(".json");

    return { Path.GetPath().ToPtr() };
}

#endif /* WITH_STATS && JAFG_STATS_USE_GOOGLE_CHROME_TRACER */
