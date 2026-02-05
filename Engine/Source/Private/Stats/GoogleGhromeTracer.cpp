// Copyright mzoesch. All rights reserved.

#include "Stats/StatsForward.h"

#if WITH_STATS && JAFG_STATS_USE_GOOGLE_CHROME_TRACER

#include "Stats/GoogleChromeTracer.h"
#include "Stats/StatsCore.h"

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

    const LString Path { this->GetPath() };

    // Do not use finder, as it may not exist yet.
    const LPath P{ Path };
    std::filesystem::create_directories(P.parent_path());

    this->Stream.open(Path.c_str(), std::ios::out | std::ios::trunc);
    this->Stream << R"({"traceEvents":[)";

    Lock.unlock();

    const bool bOut = Tasks::Private::AddThreadsToCurrentTracerSession();
    check( bOut )

    return;
}

void Jafg::Stats::Vendor::LGoogleChromeTracer::AddNamedThread(Private::LThread&& InThread)
{
    std::unique_lock Lock(this->Mutex);

    check( this->Session.IsValid() )

    if (this->bFirstEvent == false)
    {
        this->Stream << ',';
    }
    else
    {
        this->bFirstEvent = false;
    }

    this->Stream << R"({"name":"thread_name","ph":"M","pid":1,"tid":)";
    this->Stream << InThread.ThreadId;
    this->Stream << R"(,"args":{"name":")";
    this->Stream << InThread.Name;
    this->Stream << R"("}})";

    return;
}

void Jafg::Stats::Vendor::LGoogleChromeTracer::AddBookmark(Private::LBookmark&& InBookmark)
{
    std::unique_lock Lock(this->Mutex);

    check( this->Session.IsValid() )

    if (this->bFirstEvent == false)
    {
        this->Stream << ',';
    }
    else
    {
        this->bFirstEvent = false;
    }

    this->Stream << R"({"name":")";
    this->Stream << InBookmark.Name;
    this->Stream << R"(","ph":"i","cat":"bookmark","s":"t","pid":0,"tid":)";
    this->Stream << InBookmark.ThreadId;
    this->Stream << R"(,"ts":)";
    this->Stream << InBookmark.BeginTime;
    this->Stream << "}";

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
    this->Stream << R"(","ph":"X","pid":1,"tid":)";
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
        LOG_ERROR(LogStats, "Failed to write file: [{}].", this->GetPath())
    }

    algo::orphan(&this->Session.Name);

    this->bFirstEvent = true;

    return;
}

LString Jafg::Stats::Vendor::LGoogleChromeTracer::GetPath() const
{
    check( this->Session.IsValid() )

    LPath Path = Finder::GetSavedDir();
    Path /= "GoogleChrome";
    Path /= this->Session.Name;
    Path.concat(".json");

    return Path.generic_string();
}

#endif /* WITH_STATS && JAFG_STATS_USE_GOOGLE_CHROME_TRACER */
