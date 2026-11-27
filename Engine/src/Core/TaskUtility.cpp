// Copyright mzoesch. All rights reserved.

#include "Framework/Runnable.h"
#include "Core/TaskUtility.h"
#include "Core/App.h"

void Tasks::RenameMe(LString HumanReadableName)
{
#if JAFG_WITH_STATS
    #if JAFG_PLATFORM_LINUX
        if (HumanReadableName.size() * sizeof(LString::value_type) >= 16)
        {
            LOG_WARNING(LogTaskUtility
                , "[{}]: Thread name exceeds 16 bytes."
                , HumanReadableName
                )
            HumanReadableName.resize(15);
        }
    #endif /* JAFG_PLATFORM_LINUX */
    tracy::SetThreadName(HumanReadableName.c_str());
#else /* JAFG_WITH_STATS */
    #if JAFG_PLATFORM_WINDOWS
        wchar_t buf[256];
        mbstowcs(buf, HumanReadableName.c_str(), 256);
        ::SetThreadDescription(::GetCurrentThread(), buf);
        LOG_VERBOSE(LogTaskUtility, "[{}]: Renamed thread.", HumanReadableName)
    #elif JAFG_PLATFORM_LINUX
        pthread_setname_np(pthread_self(), HumanReadableName.c_str());
        LOG_VERBOSE(LogTaskUtility, "[{}]: Renamed thread.", HumanReadableName)
    #else /* JAFG_PLATFORM_LINUX */
        LOG_WARNING(LogTaskUtility, "[{}]: Failed to rename thread.", HumanReadableName)
    #endif /* !JAFG_PLATFORM_LINUX */
#endif /* JAFG_WITH_STATS */
}

namespace Tasks
{

ENGINE_API LThreadTaskOrchestrator Master{Detail::GetMasterThreadId()};
ENGINE_API LThreadTaskOrchestrator Worker{[]() -> bool { return IsOnWorkerThread(); }};

namespace
{

thread_local bool GIsWorkerThread{};
//# A hint only, the correctness of ParallelFor et al. never depends on it.
std::atomic<std::size_t> GWorkerThreadCount{0};

} /* ~Namespace <Anonymous> */

bool IsOnWorkerThread() noexcept
{
    return GIsWorkerThread;
}

std::size_t GetWorkerThreadCount() noexcept
{
    return GWorkerThreadCount.load(std::memory_order_relaxed);
}

namespace Detail
{

LThreadId GetMasterThreadId()
{
    static LThreadId MasterThreadId{std::this_thread::get_id()};
    check(MasterThreadId != LThreadId{})
    return MasterThreadId;
}
void SetIsWorkerThread(bool bWorker) noexcept
{
    if (GIsWorkerThread == bWorker)
    {
        return;
    }
    GIsWorkerThread = bWorker;
    bWorker
        ? GWorkerThreadCount.fetch_add(1, std::memory_order_relaxed)
        : GWorkerThreadCount.fetch_sub(1, std::memory_order_relaxed)
        ;
}

struct LRunnableLauncher final
{
    UTILITY_STRUCT(LRunnableLauncher)

    static EExit::Type LaunchRunnable(Jafg::LRunnable& Runnable, TFunction2<void(Jafg::LRunnable&)> Delete)
    {
        STAT_CUSTOM_ZONE_FMT("LaunchRunnable::{}", Runnable.GetHumanReadableName())

        /*
         * Even if the stop is already requested, go through the whole pipeline, and then it's
         * the responsibility of the runnable to exit.
         */
        check(!Runnable.Joinable())

        if (auto Result{Runnable.Initialize()}; Result != EExit::Success)
        {
            if (Result >= EExit::Failure)
            {
                LOG_FATAL(LogTaskSystem
                    , "[{}]: Thread with aggregator [{}] failed to initialize with a non transient failure code: {}[{}]."
                    , Runnable.GetHumanReadableName()
                    , std::this_thread::get_id()
                    , Tasks::LexToString(Result), static_cast<LTaskExit>(Result)
                    )
            }
            if (Result >= EExit::SanitizedFailure)
            {
                LOG_ERROR(LogTaskSystem
                    , "[{}]: Thread with aggregator [{}] failed to initialize with a sanitized failure code: {}[{}]."
                    , Runnable.GetHumanReadableName()
                    , std::this_thread::get_id()
                    , Tasks::LexToString(Result), static_cast<LTaskExit>(Result)
                    )
                Jafg::App::RequestEngineExit(EPlatformExit::Fatal, "Thread aggregation failed.");
                if (Delete)
                {
                    Delete(Runnable);
                }
                return Result;
            }
            if (Result >= EExit::TransientFailure)
            {
                LOG_WARNING(LogTaskSystem
                    , "[{}]: Thread with aggregator [{}] failed to initialize with a transient failure code: {}[{}]."
                    , Runnable.GetHumanReadableName()
                    , std::this_thread::get_id()
                    , Tasks::LexToString(Result), static_cast<LTaskExit>(Result)
                    )
                if (Delete)
                {
                    Delete(Runnable);
                }
                return Result;
            }
            LOG_ERROR(LogTaskSystem
                , "[{}]: Thread with aggregator [{}] failed to initialize with an unknown code: {}[{}]."
                , Runnable.GetHumanReadableName()
                , std::this_thread::get_id()
                , Tasks::LexToString(Result), static_cast<LTaskExit>(Result)
                )
            if (Delete)
            {
                Delete(Runnable);
            }
            return Result;
        }

        Runnable.Thread = std::jthread{[Delete=std::move(Delete), &Runnable](std::stop_token Stoken) mutable
        {
            RenameMe(Runnable.GetHumanReadableName());
            STAT_CUSTOM_ZONE_FMT("Runnable::{}", Runnable.GetHumanReadableName())

            algo::raii_leave _([&Runnable, Delete=std::move(Delete)] mutable
            {
                {
                    STAT_CUSTOM_ZONE_FMT("Runnable::{}::Exit", Runnable.GetHumanReadableName())
                    Runnable.Exit();
                }

                /*
                 * Only a self-owned runnable is joined from here, as nobody else may join or destroy it. Every
                 * other runnable is joined by its owner, which would race with a join scheduled from here.
                 */
                if (Delete)
                {
                    Master.Task([&Runnable, Delete=std::move(Delete)] mutable
                    {
                        Runnable.Join();
                        Delete(Runnable);
                    });
                }
            });

            if (EExit::Type Result{Runnable.Run(std::move(Stoken))}; Result != EExit::Success)
            {
                if (Result >= EExit::Failure)
                {
                    LOG_FATAL(LogTaskSystem
                        , "[{}[{}]]: Thread failed to run with a non transient failure code: {}[{}]."
                        , Runnable.GetHumanReadableName(), std::this_thread::get_id()
                        , Tasks::LexToString(Result), static_cast<LTaskExit>(Result)
                        )
                }
                if (Result >= EExit::SanitizedFailure)
                {
                    LOG_ERROR(LogTaskSystem
                        , "[{}[{}]]: Thread failed to run with a sanitized failure code: {}[{}]."
                        , Runnable.GetHumanReadableName(), std::this_thread::get_id()
                        , Tasks::LexToString(Result), static_cast<LTaskExit>(Result)
                        )
                    Jafg::App::RequestEngineExit(EPlatformExit::Fatal, "Thread aggregation failed.");
                    return;
                }
                if (Result >= EExit::TransientFailure)
                {
                    LOG_WARNING(LogTaskSystem
                        , "[{}[{}]]: Thread failed to run with a transient failure code: {}[{}]."
                        , Runnable.GetHumanReadableName(), std::this_thread::get_id()
                        , Tasks::LexToString(Result), static_cast<LTaskExit>(Result)
                        )
                    return;
                }
                LOG_ERROR(LogTaskSystem
                    , "[{}[{}]]: Thread failed to run with an unknown code: {}[{}]."
                    , Runnable.GetHumanReadableName(), std::this_thread::get_id()
                    , Tasks::LexToString(Result), static_cast<LTaskExit>(Result)
                    )
            }
        }};

        return EExit::Success;
    }
};

} /* ~Namespace Detail */

} /* ~Namespace Jafg */

bool Jafg::LRunnable::Stop(ERunnableStopReason Reason)
{
    check(Tasks::IsOnMasterThread())
    if (this->Thread.joinable() && this->Thread.request_stop())
    {
        this->OnStop(Reason);
        return true;
    }
    return false;
}

Tasks::EExit::Type Tasks::LaunchRunnable(Jafg::LRunnable& Runnable, TFunction2<void(Jafg::LRunnable&)> Delete)
{
    return Detail::LRunnableLauncher::LaunchRunnable(Runnable, std::move(Delete));
}
