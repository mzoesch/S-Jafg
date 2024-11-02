// Copyright mzoesch. All rights reserved.

#pragma once

/*----------------------------------------------------------------------------
    Low level includes.
----------------------------------------------------------------------------*/

#include "Build/Build.h"
#include "Containers/ContainerBuild.h"


/*----------------------------------------------------------------------------
    Low level forwards.
----------------------------------------------------------------------------*/

#include "Containers/ContainerForward.h"


/*----------------------------------------------------------------------------
    Core includes.
----------------------------------------------------------------------------*/

#include "HAL/GenericPlatformTypes.h"
#include "HAL/Platform.h"
#include "Core/CoreMacros.h"
#include "Core/CoreDefines.h"


/*----------------------------------------------------------------------------
    Core Forwards.
----------------------------------------------------------------------------*/

#include "Maths/MathDefines.h"
#include "Maths/MathForward.h"
#include "Maths/MathUtility.h"


/*----------------------------------------------------------------------------
    Commonly used headers.
----------------------------------------------------------------------------*/

///////////////////////////////////////////////////////////////////////////////
// Generic
#include "Runtime/AssertionMacros.h"

///////////////////////////////////////////////////////////////////////////////
// Logging
#include "Logging/LogVerbosity.h"
#include "Logging/LogMacros.h"
#include "Logging/LogPrivate.h"
#include "Logging/CoreCategories.h"

///////////////////////////////////////////////////////////////////////////////
// Functors
#include "Core/Function.h"
#include "Delegates/DelegatePrivate.h"
#include "Delegates/DelegateMacros.h"

///////////////////////////////////////////////////////////////////////////////
// Containers
#include "Containers/ContainerMacros.h"
#include "Containers/Iterator.h"
#include "Containers/Array.h"
#include "Containers/AsciiString.h"

///////////////////////////////////////////////////////////////////////////////
// Misc
#include "Misc/Optional.h"

///////////////////////////////////////////////////////////////////////////////
// Maths
#include "Maths/Vector2.h"
#include "Maths/Vector.h"
#include "Maths/IntVector2.h"
#include "Maths/IntVector.h"
#include "Maths/Color.h"


/*----------------------------------------------------------------------------
    Standard, Platform-APIs and Third-Party.
----------------------------------------------------------------------------*/

#include "PartyPCH.h"
