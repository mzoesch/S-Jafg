// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Player/InputTypes.h"

LString Jafg::EKeys::ToString(const LKey& Key)
{
    switch (Key)
    {
    case EKeys::AnyKey:             return "AnyKey";
    case EKeys::A:                  return "A";
    case EKeys::B:                  return "B";
    case EKeys::C:                  return "C";
    case EKeys::D:                  return "D";
    case EKeys::E:                  return "E";
    case EKeys::F:                  return "F";
    case EKeys::G:                  return "G";
    case EKeys::H:                  return "H";
    case EKeys::I:                  return "I";
    case EKeys::J:                  return "J";
    case EKeys::K:                  return "K";
    case EKeys::L:                  return "L";
    case EKeys::M:                  return "M";
    case EKeys::N:                  return "N";
    case EKeys::O:                  return "O";
    case EKeys::P:                  return "P";
    case EKeys::Q:                  return "Q";
    case EKeys::R:                  return "R";
    case EKeys::S:                  return "S";
    case EKeys::T:                  return "T";
    case EKeys::U:                  return "U";
    case EKeys::V:                  return "V";
    case EKeys::W:                  return "W";
    case EKeys::X:                  return "X";
    case EKeys::Y:                  return "Y";
    case EKeys::Z:                  return "Z";

    case EKeys::MouseX:             return "MouseX";
    case EKeys::MouseY:             return "MouseY";
    case EKeys::MouseXY:            return "MouseXY";
    case EKeys::MouseWheelAxis:     return "MouseWheelAxis";
    case EKeys::MouseWheelUp:       return "MouseWheelUp";
    case EKeys::MouseWheelDown:     return "MouseWheelDown";

    default:                        return "Custom";
    }
}
