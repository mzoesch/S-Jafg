// Copyright mzoesch. All rights reserved.

#pragma once

#include "Platform/SurfaceForward.h"

namespace Jafg::LaunchProgress
{

ENGINE_API void PrepareBeginProgress();
ENGINE_API void BeginProgress(const LSimpleString& Name, const LSimpleString& Description, const float Progress = 0.0f, const double ProgressStep = 0.0);
ENGINE_API void BeginProgress(const LSimpleString& Description, const float Progress = 0.0f);
ENGINE_API void FinishAndGiveUpMemory();

namespace Private
{

ENGINE_API extern float          GProgress;
ENGINE_API extern double         GProgressStep;
ENGINE_API extern LSimpleString  GProgressName;
ENGINE_API extern LSimpleString  GProgressDescription;
ENGINE_API extern LNativeWindow* GProgressWindow;
ENGINE_API extern bool           bOwnerShipToken;

} /* ~Namespace Private */

} /* ~Namespace Jafg::LaunchProgress */
