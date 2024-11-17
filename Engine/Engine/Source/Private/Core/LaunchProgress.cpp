// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Platform/Surface.h"
#include "Core/LaunchProgress.h"

#include "Forward/EngineForward.h"
#include "Rhi/RendererApplier.h"

namespace Jafg::LaunchProgress
{

ENGINE_API float          Private::GProgress            = 0.0f;
ENGINE_API double         Private::GProgressStep        = 0.0;
ENGINE_API LSimpleString  Private::GProgressName        = "Intermediate";
ENGINE_API LSimpleString  Private::GProgressDescription = "Intermediate";
ENGINE_API LNativeWindow* Private::GProgressWindow      = nullptr;
ENGINE_API bool           Private::bOwnerShipToken      = false;

} /* ~Namespace Jafg::LaunchProgress */

void Jafg::LaunchProgress::PrepareBeginProgress()
{
    check( Private::GProgressWindow == nullptr )

    if (RendererApplier::IsGlfwInitialized() == false)
    {
        const bool bOk = RendererApplier::InitializeGlfw();
        if (bOk == false)
        {
            JAFG_ENGINE_FORWARD_REQUEST_EXIT(EPlatformExit::Fatal, "Failed to initialize glfw window.")
            return;
        }
    }

#if PLATFORM_DESKTOP
    Private::GProgressWindow = LDesktopPlatform::CreateNativeWindow(LDesktopSurfaceProps());
#else
    #error "Could not resolve platform."
#endif /* PLATFORM_DESKTOP */

    RendererApplier::ApplyOpenGlToWindow(Private::GProgressWindow);

    return;
}

void Jafg::LaunchProgress::BeginProgress(
    const LSimpleString& Name,
    const LSimpleString& Description,
    const float Progress, /* = 0.0f */
    const double ProgressStep /* = 0.0 */
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

void Jafg::LaunchProgress::BeginProgress(const LSimpleString& Description, const float Progress)
{
    if (Maths::IsNearlyEqual(Progress, 0.0f) == false)
    {
        Private::GProgress = Maths::Clamp(Progress, 0.0f, 1.0f);
    }
    else if (Private::GProgressStep > 0.0)
    {
        Private::GProgress += static_cast<float>(Private::GProgressStep);
    }

    Private::GProgressDescription = Description;

    LOG_VERBOSE(LogGuardedMain, "Launch progressed to {} - {} with {:.2f}%.", Private::GProgressName, Private::GProgressDescription, Progress * 100.0f)

    return;
}

void Jafg::LaunchProgress::FinishAndGiveUpMemory()
{
    Private::GProgressDescription.Empty();
    Private::GProgressName.Empty();
    Private::GProgress = 0.0f;
    Private::GProgressStep = 0.0;

    if (Private::GProgressWindow == nullptr || Private::bOwnerShipToken)
    {
        Private::GProgressWindow = nullptr;
        Private::bOwnerShipToken = false;
        return;
    }

    panic( "Not implemented yet." )

    return;
}
