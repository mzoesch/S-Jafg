// Copyright mzoesch. All rights reserved.

#include "Lal.afx"
#include "Platform/Surface.h"
#include "Core/LaunchProgress.h"
#include "Forward/EngineForward.h"
#include "Stats/Stats.h"

namespace Jafg::LaunchProgress
{

ENGINE_API f32       Private::GProgress            = 0.0f;
ENGINE_API f64       Private::GProgressStep        = 0.0;
ENGINE_API LString   Private::GProgressName        = "Intermediate";
ENGINE_API LString   Private::GProgressDescription = "Intermediate";
ENGINE_API LSurface* Private::GProgressSurface     = nullptr;
ENGINE_API bool      Private::bOwnerShipToken      = false;

} /* ~Namespace Jafg::LaunchProgress */

void Jafg::LaunchProgress::PrepareBeginProgress()
{
    STAT_CYCLE_FUNCTION()

    check( Private::GProgressSurface == nullptr )
    check( Private::bOwnerShipToken == false )

    Private::GProgressSurface = new LSurface();
    Private::GProgressSurface->Initialize();

    return;
}

void Jafg::LaunchProgress::BeginProgress(
    const LString& Name,
    const LString& Description,
    const f32 Progress, /* = 0.0f */
    const f64 ProgressStep /* = 0.0 */
)
{
    if (Maths::IsNearlyEqual(Progress, 0.0f) == false)
    {
        Private::GProgress = Maths::Clamp(Progress, 0.0f, 1.0f);
    }

    Private::GProgressName        = Name;
    Private::GProgressDescription = Description;
    Private::GProgressStep        = ProgressStep;

    LOG_VERBOSE(LogGuardedMain, "Launch progressed to {} - {} with {:.2f}%.", Private::GProgressName, Private::GProgressDescription, Progress * 100.0f)

    return;
}

void Jafg::LaunchProgress::BeginProgress(const LString& Description, const f32 Progress)
{
    if (Maths::IsNearlyEqual(Progress, 0.0f) == false)
    {
        Private::GProgress = Maths::Clamp(Progress, 0.0f, 1.0f);
    }
    else if (Private::GProgressStep > 0.0)
    {
        Private::GProgress += static_cast<f32>(Private::GProgressStep);
    }

    Private::GProgressDescription = Description;

    LOG_VERBOSE(LogGuardedMain, "Launch progressed to {} - {} with {:.2f}%.", Private::GProgressName, Private::GProgressDescription, Progress * 100.0f)

    return;
}

void Jafg::LaunchProgress::FinishAndGiveUpMemory()
{
    Private::GProgress = 0.0f;
    Private::GProgressStep = 0.0;
    Private::GProgressName.Empty();
    Private::GProgressDescription.Empty();

    if (Private::GProgressSurface == nullptr || Private::bOwnerShipToken)
    {
        Private::GProgressSurface = nullptr;
        Private::bOwnerShipToken = false;
        return;
    }

    delete Private::GProgressSurface;
    Private::GProgressSurface = nullptr;

    return;
}
