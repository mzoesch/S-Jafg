// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

enum struct ECursor
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

inline LString LexToString(ECursor Cursor)
{
    switch (Cursor)
    {
    case ECursor::None:       return "None";
    case ECursor::Default:    return "Default";
    case ECursor::Arrow:      return "Arrow";
    case ECursor::Beam:       return "Beam";
    case ECursor::Crosshair:  return "Crosshair";
    case ECursor::Hand:       return "Hand";
    case ECursor::ResizeNS:   return "ResizeNS";
    case ECursor::ResizeEW:   return "ResizeEW";
    case ECursor::ResizeNESW: return "ResizeNESW";
    case ECursor::ResizeNWSE: return "ResizeNWSE";
    case ECursor::ResizeOmni: return "ResizeOmni";
    case ECursor::NotAllowed: return "NotAllowed";
    default:                       return "Custom";
    }
}

} /* ~Namespace Jafg */
