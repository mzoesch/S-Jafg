#pragma once
#include "Build.generated.h"
#define PLATFORM_CALLSPEC_IN __declspec( dllimport )
#define PLATFORM_CALLSPEC_OUT __declspec( dllexport )

TESTERFORWARD_API int DontDiscardMe();
