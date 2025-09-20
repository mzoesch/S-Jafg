// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Input/InputTypes.h"

LString Jafg::EKeys::ToString(const LKey Key)
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

    case EKeys::LeftMouseButton:    return "LeftMouseButton";
    case EKeys::RightMouseButton:   return "RightMouseButton";
    case EKeys::MiddleMouseButton:  return "MiddleMouseButton";
    case EKeys::ThumbMouseButton1:  return "ThumbMouseButton1";
    case EKeys::ThumbMouseButton2:  return "ThumbMouseButton2";

    case EKeys::BackSpace:          return "BackSpace";
    case EKeys::Tab:                return "Tab";
    case EKeys::Enter:              return "Enter";
    case EKeys::Pause:              return "Pause";
    case EKeys::CapsLock:           return "CapsLock";
    case EKeys::Escape:             return "Escape";
    case EKeys::Space:              return "Space";
    case EKeys::PageUp:             return "PageUp";
    case EKeys::PageDown:           return "PageDown";
    case EKeys::End:                return "End";
    case EKeys::Home:               return "Home";

    case EKeys::Left:               return "Left";
    case EKeys::Up:                 return "Up";
    case EKeys::Right:              return "Right";
    case EKeys::Down:               return "Down";

    case EKeys::Insert:             return "Insert";
    case EKeys::Delete:             return "Delete";

    case EKeys::Zero:               return "Zero";
    case EKeys::One:                return "One";
    case EKeys::Two:                return "Two";
    case EKeys::Three:              return "Three";
    case EKeys::Four:               return "Four";
    case EKeys::Five:               return "Five";
    case EKeys::Six:                return "Six";
    case EKeys::Seven:              return "Seven";
    case EKeys::Eight:              return "Eight";
    case EKeys::Nine:               return "Nine";

    case EKeys::NumPadZero:         return "NumPadZero";
    case EKeys::NumPadOne:          return "NumPadOne";
    case EKeys::NumPadTwo:          return "NumPadTwo";
    case EKeys::NumPadThree:        return "NumPadThree";
    case EKeys::NumPadFour:         return "NumPadFour";
    case EKeys::NumPadFive:         return "NumPadFive";
    case EKeys::NumPadSix:          return "NumPadSix";
    case EKeys::NumPadSeven:        return "NumPadSeven";
    case EKeys::NumPadEight:        return "NumPadEight";
    case EKeys::NumPadNine:         return "NumPadNine";
    case EKeys::NumPadLock:         return "NumPadLock";
    case EKeys::NumPadDivide:       return "NumPadDivide";
    case EKeys::NumPadMultiply:     return "NumPadMultiply";
    case EKeys::NumPadSubtract:     return "NumPadSubtract";
    case EKeys::NumPadAdd:          return "NumPadAdd";
    case EKeys::NumPadEnter:        return "NumPadEnter";
    case EKeys::NumPadDecimal:      return "NumPadDecimal";

    case EKeys::F1:                 return "F1";
    case EKeys::F2:                 return "F2";
    case EKeys::F3:                 return "F3";
    case EKeys::F4:                 return "F4";
    case EKeys::F5:                 return "F5";
    case EKeys::F6:                 return "F6";
    case EKeys::F7:                 return "F7";
    case EKeys::F8:                 return "F8";
    case EKeys::F9:                 return "F9";
    case EKeys::F10:                return "F10";
    case EKeys::F11:                return "F11";
    case EKeys::F12:                return "F12";

    case EKeys::ScrollLock:         return "ScrollLock";
    case EKeys::Print:              return "Print";

    case EKeys::LeftShift:          return "LeftShift";
    case EKeys::RightShift:         return "RightShift";
    case EKeys::LeftControl:        return "LeftControl";
    case EKeys::RightControl:       return "RightControl";
    case EKeys::LeftAlt:            return "LeftAlt";
    case EKeys::RightAlt:           return "RightAlt";
    case EKeys::LeftCommand:        return "LeftCommand";
    case EKeys::RightCommand:       return "RightCommand";

    case EKeys::Semicolon:          return "Semicolon";
    case EKeys::Equals:             return "Equals";
    case EKeys::Comma:              return "Comma";
    case EKeys::Hyphen:             return "Hyphen";
    case EKeys::Period:             return "Period";
    case EKeys::Slash:              return "Slash";
    case EKeys::Tilde:              return "Tilde";
    case EKeys::LeftBracket:        return "LeftBracket";
    case EKeys::Backslash:          return "Backslash";
    case EKeys::RightBracket:       return "RightBracket";
    case EKeys::Apostrophe:         return "Apostrophe";

    case EKeys::Unresolved:         return "Unresolved";
    case EKeys::PlatformDelete:     return "PlatformDelete";

    default:                        return "Custom";
    }
}
