// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

typedef uint8 LClassFlags;

/**
 * Class flags that can be used to describe a jafg object class.
 * @note This namespace resides outside the jafg namespace to avoid long typing when declaring new classes
 *       as the jafg namespace does not exist in the global namespace.
 */
namespace EClassFlags
{

enum Type : LClassFlags
{
    /** No flags are set. This is the default value. */
    None            = 0b0000'0000,

    /** The class is abstract and can therefore not be instantiated. */
    Abstract        = 0b0000'0001,
};

} /* ~Namespace EClassFlags */
