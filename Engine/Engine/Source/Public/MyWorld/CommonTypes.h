// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

/**
 * Uniquely defines a voxel in the application.
 */
typedef uint32 voxel_t;

/**
 * Uniquely defines a voxel in the application.
 * Use this sparingly and never store this type in a variable, only use it for loops or similar.
 */
typedef int64 voxel_t_signed;
typedef int32 voxel_t_it;

/**
 * Uniquely defines a loaded texture in the application.
 * @see MyWorld/TextureSubsystem.h
 */
typedef uint32 LTextureIndex;

namespace ECompileTimeVoxels
{

enum Type : voxel_t
{
    Null = 0,
    Air  = 1,
    Max  = 1,
    Num  = 2,
};

} /* ~Namespace EDefinedVoxels */

typedef uint8 LNormalLookup;
namespace ENormalLookup
{

enum Type : uint8
{
    None  = 0x00 << 0,

    North = 0x01 << 0,
    East  = 0x01 << 1,
    South = 0x01 << 2,
    West  = 0x01 << 3,
    Up    = 0x01 << 4,
    Down  = 0x01 << 5,

    Vertical = Up | Down,
    Sides    = North | East | South | West,
    Omnia    = Sides | Vertical,
};

FORCEINLINE LNormalLookup FromVector(const LVector& InNormal)
{
    if (InNormal.Equals(LVector::UpVector))       { return ENormalLookup::Up; }
    if (InNormal.Equals(LVector::DownVector))     { return ENormalLookup::Down; }
    if (InNormal.Equals(LVector::ForwardVector))  { return ENormalLookup::North; }
    if (InNormal.Equals(LVector::BackwardVector)) { return ENormalLookup::South; }
    if (InNormal.Equals(LVector::RightVector))    { return ENormalLookup::East; }
    if (InNormal.Equals(LVector::LeftVector))     { return ENormalLookup::West; }

    checkNoEntry()

    return ENormalLookup::None;
}

ENGINE_API extern LSimpleString NorthStr;    /* "North"    */
ENGINE_API extern LSimpleString EastStr;     /* "East"     */
ENGINE_API extern LSimpleString SouthStr;    /* "South"    */
ENGINE_API extern LSimpleString WestStr;     /* "West"     */
ENGINE_API extern LSimpleString UpStr;       /* "Up"       */
ENGINE_API extern LSimpleString DownStr;     /* "Down"     */
ENGINE_API extern LSimpleString VerticalStr; /* "Vertical" */
ENGINE_API extern LSimpleString SidesStr;    /* "Sides"    */

FORCEINLINE bool IsValid(const LSimpleString& InNormal)
{
    return InNormal == NorthStr
        || InNormal == EastStr
        || InNormal == SouthStr
        || InNormal == WestStr
        || InNormal == UpStr
        || InNormal == DownStr
        || InNormal == VerticalStr
        || InNormal == SidesStr;
}

FORCEINLINE ENormalLookup::Type FromString(const LSimpleString& InNormal)
{
    if (InNormal == NorthStr)    { return ENormalLookup::North; }
    if (InNormal == EastStr)     { return ENormalLookup::East; }
    if (InNormal == SouthStr)    { return ENormalLookup::South; }
    if (InNormal == WestStr)     { return ENormalLookup::West; }
    if (InNormal == UpStr)       { return ENormalLookup::Up; }
    if (InNormal == DownStr)     { return ENormalLookup::Down; }
    if (InNormal == VerticalStr) { return ENormalLookup::Vertical; }
    if (InNormal == SidesStr)    { return ENormalLookup::Sides; }

    checkNoEntry()

    return ENormalLookup::None;
}

} /* ~Namespace ENormalLookup */
ENUM_CLASS_FLAGS(ENormalLookup::Type)

FORCEINLINE LSimpleString LexToString(const ENormalLookup::Type InNormal)
{
    switch (InNormal)
    {
    case ENormalLookup::North:    { return ENormalLookup::NorthStr; }
    case ENormalLookup::East:     { return ENormalLookup::EastStr; }
    case ENormalLookup::South:    { return ENormalLookup::SouthStr; }
    case ENormalLookup::West:     { return ENormalLookup::WestStr; }
    case ENormalLookup::Up:       { return ENormalLookup::UpStr; }
    case ENormalLookup::Down:     { return ENormalLookup::DownStr; }
    case ENormalLookup::Vertical: { return ENormalLookup::VerticalStr; }
    case ENormalLookup::Sides:    { return ENormalLookup::SidesStr; }
    default: { jassertNoEntry() return { }; }
    }
}

} /* ~Namespace Jafg */
