// Copyright mzoesch. All rights reserved.

#pragma once

/*----------------------------------------------------------------------------
    Low level includes.
----------------------------------------------------------------------------*/

#include "Build/Build.h"


/*----------------------------------------------------------------------------
    Core includes.
----------------------------------------------------------------------------*/

#include "Hal/Platform.h"
#include "Runtime/Asan.h"
#include "Core/CoreMacros.h"
#include "Core/CoreDefines.h"
#include "Core/Traits.h"
#include "Core/CoreConcepts.h"
#include "Core/CoreFunctors.h"


/*----------------------------------------------------------------------------
    Core Forwards.
----------------------------------------------------------------------------*/

#include "Containers/ContainerBuild.h"
#include "Containers/ContainerLegacyForward.h"
#include "Containers/ContainerConcepts.h"
#include "Containers/ContainerEarlyForward.h"
#include "Containers/LalIteratorTraits.h"
#include "Containers/LalIterator.h"
#include "Containers/LalArrayAllocators.h"
#include "Containers/ContainerArrayForward.h"
#include "Containers/LalArray.h"
#include "Containers/LalStringEncoding.h"
#include "Containers/LalStringForward.h"
#include "Containers/LalString.h"
#include "Maths/MathDefines.h"
#include "Maths/MathForward.h"


/*----------------------------------------------------------------------------
    Standard, Platform-APIs and Third-Party.
----------------------------------------------------------------------------*/

#include "PartyPch.h"


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
#include "Logging/LogUtility.h"

///////////////////////////////////////////////////////////////////////////////
// Memory
#include "Memory/Smart.h"

///////////////////////////////////////////////////////////////////////////////
// Maths core
#include "Maths/MathUtility.h"

///////////////////////////////////////////////////////////////////////////////
// Containers
#include "Containers/ContainerMacros.h"
#include "Containers/LalArrayAllocatorsImpl.h"
#include "Containers/LalArrayImpl.h"
#include "Containers/LalStringEncodingImpl.h"
#include "Containers/LalStringImpl.h"
#include "Containers/ComplexQueue.h"
#include "Containers/SimpleQueue.h"
#include "Containers/Format.h"
#include "Containers/StringUtility.h"
#include "Containers/Optional.h"
#include "System/LalPath.h"
#include "System/LalFinder.h"

///////////////////////////////////////////////////////////////////////////////
// Functors
#include "Core/Function.h"
#include "Delegates/DelegatePrivate.h"
#include "Delegates/DelegateMacros.h"

///////////////////////////////////////////////////////////////////////////////
// Maths
#include "Maths/Vector2.h"
#include "Maths/Vector3.h"
#include "Maths/Vector4.h"
#include "Maths/IntVector2.h"
#include "Maths/IntVector.h"
#include "Maths/Color.h"
#include "Maths/Rotator.h"
#include "Maths/Plane.h"
#include "Maths/Matrix4.h"
#include "Maths/Transform.h"
#include "Maths/Matrix3.h"

///////////////////////////////////////////////////////////////////////////////
// Backward implementations
#include "Containers/LalIteratorImpl.h"
#include "System/LalPathImpl.h"

#if !LAL_PLATFORM_SUPPORTS_EXTERN_TEMPLATE_SPECIFICATIONS
    #define PRIVATE_JAFG_MAKE_INLINE
    #include "Lal/Lal/Source/Private/Math/MathDefines.cpp"
#endif /* !PLATFORM_USES_INLINE_MATH_DEFINES */
