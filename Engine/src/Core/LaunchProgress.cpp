// Copyright mzoesch. All rights reserved.

#include "Platform/Surface.h"
#include "Core/LaunchProgress.h"
#include "Framework/Frontend.h"
#include "Stats/Stats.h"

namespace Jafg::LaunchProgress
{

ENGINE_API f32       Private::GProgress            = 0.0f;
ENGINE_API f64       Private::GProgressStep        = 0.0;
ENGINE_API LString   Private::GProgressName        = "Intermediate";
ENGINE_API LString   Private::GProgressDescription = "Intermediate";

} /* ~Namespace Jafg::LaunchProgress */

void Jafg::LaunchProgress::PrepareBeginProgress()
{
    // STAT_CYCLE_FUNCTION() [[deprecated]]
}

void Jafg::LaunchProgress::BeginProgress(
    const LString& Name,
    const LString& Description,
    const f32 Progress, /* = 0.0f */
    const f64 ProgressStep /* = 0.0 */
)
{
    if (maths::eq_e(Progress, 0.0f) == false)
    {
        Private::GProgress = maths::clamp(Progress, 0.0f, 1.0f);
    }

    Private::GProgressName        = Name;
    Private::GProgressDescription = Description;
    Private::GProgressStep        = ProgressStep;

    LOG_VERBOSE(LogLaunch, "Launch progressed to {} - {} with {:.2f}%.", Private::GProgressName, Private::GProgressDescription, Progress * 100.0f)

    return;
}

void Jafg::LaunchProgress::BeginProgress(const LString& Description, const f32 Progress)
{
    if (maths::eq_e(Progress, 0.0f) == false)
    {
        Private::GProgress = maths::clamp(Progress, 0.0f, 1.0f);
    }
    else if (Private::GProgressStep > 0.0)
    {
        Private::GProgress += static_cast<f32>(Private::GProgressStep);
    }

    Private::GProgressDescription = Description;

    LOG_VERBOSE(LogLaunch, "Launch progressed to {} - {} with {:.2f}%.", Private::GProgressName, Private::GProgressDescription, Progress * 100.0f)

    return;
}

void Jafg::LaunchProgress::FinishAndGiveUpMemory()
{
    Private::GProgress = 0.0f;
    Private::GProgressStep = 0.0;
    algo::orphan(&Private::GProgressName);
    algo::orphan(&Private::GProgressDescription);

    return;
}
