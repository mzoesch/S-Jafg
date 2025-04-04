// Copyright mzoesch. All rights reserved.

#pragma once

#include "Platform/SurfaceForward.h"

namespace Jafg::LaunchProgress
{

ENGINE_API void PrepareBeginProgress();
ENGINE_API void BeginProgress(const LString& Name, const LString& Description, const f32 Progress = 0.0f, const f64 ProgressStep = 0.0);
ENGINE_API void BeginProgress(const LString& Description, const f32 Progress = 0.0f);
ENGINE_API void FinishAndGiveUpMemory();

namespace Private
{

ENGINE_API extern f32       GProgress;
ENGINE_API extern f64       GProgressStep;
ENGINE_API extern LString   GProgressName;
ENGINE_API extern LString   GProgressDescription;
ENGINE_API extern LSurface* GProgressSurface;
ENGINE_API extern bool      bOwnerShipToken;

} /* ~Namespace Private */

} /* ~Namespace Jafg::LaunchProgress */
