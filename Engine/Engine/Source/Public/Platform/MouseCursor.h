// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

namespace EMouseCursor
{

enum Type : u8
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

inline LString LexToString(const EMouseCursor::Type InCursor)
{
    switch (InCursor)
    {
    case EMouseCursor::None:       return FMT("None");
    case EMouseCursor::Default:    return FMT("Default");
    case EMouseCursor::Arrow:      return FMT("Arrow");
    case EMouseCursor::Beam:       return FMT("Beam");
    case EMouseCursor::Crosshair:  return FMT("Crosshair");
    case EMouseCursor::Hand:       return FMT("Hand");
    case EMouseCursor::ResizeNS:   return FMT("ResizeNS");
    case EMouseCursor::ResizeEW:   return FMT("ResizeEW");
    case EMouseCursor::ResizeNESW: return FMT("ResizeNESW");
    case EMouseCursor::ResizeNWSE: return FMT("ResizeNWSE");
    case EMouseCursor::ResizeOmni: return FMT("ResizeOmni");
    case EMouseCursor::NotAllowed: return FMT("NotAllowed");
    default:                       return FMT("Custom");
    }
}

} /* ~Namespace Jafg */
