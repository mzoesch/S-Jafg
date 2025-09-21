// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

//#
//# Uniquely defines a voxel in the application.
//#
typedef u32 voxel_t;

//#
//# Uniquely defines a voxel in the application.
//# Use this sparingly and never store this type in a variable, only use it for loops or similar.
//#
typedef i64 voxel_t_signed;
typedef i32 voxel_t_it;

//#
//# Uniquely defines a loaded texture in the application.
//# @see MyWorld/TextureSubsystem.h
//#
typedef u32 LTextureIndex;

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

typedef u8 LNormalLookup;

//#
//# The normal lookup used by the texture subsystem.
//#
namespace ENormalLookup
{

enum Type : u8
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

ENGINE_API extern LString NorthStr;    /* "North"    */
ENGINE_API extern LString EastStr;     /* "East"     */
ENGINE_API extern LString SouthStr;    /* "South"    */
ENGINE_API extern LString WestStr;     /* "West"     */
ENGINE_API extern LString UpStr;       /* "Up"       */
ENGINE_API extern LString DownStr;     /* "Down"     */
ENGINE_API extern LString VerticalStr; /* "Vertical" */
ENGINE_API extern LString SidesStr;    /* "Sides"    */

FORCEINLINE bool IsValid(const LString& InNormal)
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

FORCEINLINE ENormalLookup::Type FromString(const LString& InNormal)
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

FORCEINLINE LString LexToString(const ENormalLookup::Type InNormal)
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

//#
//# The normal mapping used by the graphics engine.
//#
namespace ENormalShadingMap
{

enum Type : u8
{
    Up       = 0x00,
    Down     = 0x01,
    Forward  = 0x02,
    Backward = 0x03,
    Right    = 0x04,
    Left     = 0x05,
};

} /* ~Namespace ENormalShadingMap */

} /* ~Namespace Jafg */
