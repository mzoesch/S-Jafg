// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

namespace EMouseCursor
{

enum Type : uint8
{

    None,       // Do not affect the current cursor state.
    Default,    // Default platform cursor
    Arrow,      // Arrow, pointing up, usually default, ...
    Beam,       // IBeam, TextEditBeam, ...
    Crosshair,  // Crosshair, ...
    Hand,       // Hand, usually for links, ...
    ResizeNS,   // Resize north-south, resize vertical, ...
    ResizeEW,   // Resize east-west, resize horizontal, ...
    ResizeNESW, // Resize north-east-south-west, resize diagonal, ...
    ResizeNWSE, // Resize north-west-south-east, resize other diagonal, ...
    ResizeOmni, // Resize all, move, ...
    NotAllowed, // Not allowed, no, nein, non, niet, ...
    Custom      // User-defined, ...
};

} /* ~Namespace EMouseCursor */

inline LSimpleString LexToString(const EMouseCursor::Type InCursor)
{
    switch (InCursor)
    {
    case EMouseCursor::None:       return "None";
    case EMouseCursor::Default:    return "Default";
    case EMouseCursor::Arrow:      return "Arrow";
    case EMouseCursor::Beam:       return "Beam";
    case EMouseCursor::Crosshair:  return "Crosshair";
    case EMouseCursor::Hand:       return "Hand";
    case EMouseCursor::ResizeNS:   return "ResizeNS";
    case EMouseCursor::ResizeEW:   return "ResizeEW";
    case EMouseCursor::ResizeNESW: return "ResizeNESW";
    case EMouseCursor::ResizeNWSE: return "ResizeNWSE";
    case EMouseCursor::ResizeOmni: return "ResizeOmni";
    case EMouseCursor::NotAllowed: return "NotAllowed";
    default:                       return "Custom";
    }
}

} /* ~Namespace Jafg */
