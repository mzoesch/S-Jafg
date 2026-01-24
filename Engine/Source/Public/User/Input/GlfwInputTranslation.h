// Copyright mzoesch. All rights reserved.

#if JAFG_NO_GLFW3
    #error "Including Glfw library specific code without GLFW3 enabled."
#endif /* JAFG_NO_GLFW3 */

#pragma once

#include "Minimal.afx"
#include "InputTypes.h"
#if !JAFG_NO_GLFW3 // Glfw3 may still be linked by the current build tool.
                   // So we do not necessarily need to include it.
    #include <GLFW/glfw3.h>
#endif /* !JAFG_NO_GLFW3 */

namespace Jafg::Glfw3
{

FORCEINLINE i32 TranslateKeyToGlfw(const LKey InKey);
FORCEINLINE LKey  TranslateKeyFromGlfw(const i32 InKey);

i32 TranslateKeyToGlfw(const LKey InKey)
{
    if (InKey == EKeys::AnyKey)
    {
        LOG_WARNING(LogUserInput, "Cannot translate key [{}] to glfw3.", EKeys::ToString(InKey))
        return INDEX_NONE;
    }

    if (InKey >= EKeys::A && InKey <= EKeys::Z)
    {
        return InKey - (EKeys::A - 1) + (GLFW_KEY_A - 1);
    }

    if (InKey == EKeys::BackSpace) { return GLFW_KEY_BACKSPACE; }
    if (InKey == EKeys::Tab)       { return GLFW_KEY_TAB; }
    if (InKey == EKeys::Enter)     { return GLFW_KEY_ENTER; }
    if (InKey == EKeys::Pause)     { return GLFW_KEY_PAUSE; }
    if (InKey == EKeys::CapsLock)  { return GLFW_KEY_CAPS_LOCK; }
    if (InKey == EKeys::Escape)    { return GLFW_KEY_ESCAPE; }
    if (InKey == EKeys::Space)     { return GLFW_KEY_SPACE; }
    if (InKey == EKeys::PageUp)    { return GLFW_KEY_PAGE_UP; }
    if (InKey == EKeys::PageDown)  { return GLFW_KEY_PAGE_DOWN; }
    if (InKey == EKeys::End)       { return GLFW_KEY_END; }
    if (InKey == EKeys::Home)      { return GLFW_KEY_HOME; }

    if (InKey == EKeys::Left)      { return GLFW_KEY_LEFT; }
    if (InKey == EKeys::Up)        { return GLFW_KEY_UP; }
    if (InKey == EKeys::Right)     { return GLFW_KEY_RIGHT; }
    if (InKey == EKeys::Down)      { return GLFW_KEY_DOWN; }

    if (InKey == EKeys::Insert)    { return GLFW_KEY_INSERT; }
    if (InKey == EKeys::Delete)    { return GLFW_KEY_DELETE; }

    if (InKey >= EKeys::Zero && InKey <= EKeys::Nine)
    {
        return InKey - (EKeys::Zero - 1) + (GLFW_KEY_0 - 1);
    }

    if (InKey >= EKeys::NumPadZero && InKey <= EKeys::NumPadNine)
    {
        return InKey - (EKeys::NumPadZero - 1) + (GLFW_KEY_KP_0 - 1);
    }

    if (InKey == EKeys::NumPadLock)     { return GLFW_KEY_NUM_LOCK; }
    if (InKey == EKeys::NumPadDivide)   { return GLFW_KEY_KP_DIVIDE; }
    if (InKey == EKeys::NumPadMultiply) { return GLFW_KEY_KP_MULTIPLY; }
    if (InKey == EKeys::NumPadSubtract) { return GLFW_KEY_KP_SUBTRACT; }
    if (InKey == EKeys::NumPadAdd)      { return GLFW_KEY_KP_ADD; }
    if (InKey == EKeys::NumPadEnter)    { return GLFW_KEY_KP_ENTER; }
    if (InKey == EKeys::NumPadDecimal)  { return GLFW_KEY_KP_DECIMAL; }

    if (InKey >= EKeys::F1 && InKey <= EKeys::F12)
    {
        return InKey - (EKeys::F1 - 1) + (GLFW_KEY_F1 - 1);
    }

    if (InKey == EKeys::ScrollLock)     { return GLFW_KEY_SCROLL_LOCK;  }
    if (InKey == EKeys::Print)          { return GLFW_KEY_PRINT_SCREEN; }

    if (InKey == EKeys::LeftShift)      { return GLFW_KEY_LEFT_SHIFT; }
    if (InKey == EKeys::RightShift)     { return GLFW_KEY_RIGHT_SHIFT; }
    if (InKey == EKeys::LeftControl)    { return GLFW_KEY_LEFT_CONTROL; }
    if (InKey == EKeys::RightControl)   { return GLFW_KEY_RIGHT_CONTROL; }
    if (InKey == EKeys::LeftAlt)        { return GLFW_KEY_LEFT_ALT; }
    if (InKey == EKeys::RightAlt)       { return GLFW_KEY_RIGHT_ALT; }
    if (InKey == EKeys::LeftCommand)    { return GLFW_KEY_LEFT_SUPER; }
    if (InKey == EKeys::RightCommand)   { return GLFW_KEY_RIGHT_SUPER; }

    if (InKey == EKeys::Semicolon)      { return GLFW_KEY_SEMICOLON; }
    if (InKey == EKeys::Equals)         { return GLFW_KEY_EQUAL; }
    if (InKey == EKeys::Comma)          { return GLFW_KEY_COMMA; }
    if (InKey == EKeys::Hyphen)         { return GLFW_KEY_MINUS; }
    if (InKey == EKeys::Period)         { return GLFW_KEY_PERIOD; }
    if (InKey == EKeys::Slash)          { return GLFW_KEY_SLASH; }
    if (InKey == EKeys::Tilde)          { return GLFW_KEY_GRAVE_ACCENT; }
    if (InKey == EKeys::LeftBracket)    { return GLFW_KEY_LEFT_BRACKET; }
    if (InKey == EKeys::Backslash)      { return GLFW_KEY_BACKSLASH; }
    if (InKey == EKeys::RightBracket)   { return GLFW_KEY_RIGHT_BRACKET; }
    if (InKey == EKeys::Apostrophe)     { return GLFW_KEY_APOSTROPHE; }

    return INDEX_NONE;
}

LKey TranslateKeyFromGlfw(const i32 InKey)
{
    if (InKey >= GLFW_KEY_A && InKey <= GLFW_KEY_Z)
    {
        return static_cast<LKey>(InKey - (GLFW_KEY_A - 1) + (static_cast<i32>(EKeys::A) - 1));
    }

    if (InKey == GLFW_KEY_BACKSPACE) { return EKeys::BackSpace; }
    if (InKey == GLFW_KEY_TAB)       { return EKeys::Tab; }
    if (InKey == GLFW_KEY_ENTER)     { return EKeys::Enter; }
    if (InKey == GLFW_KEY_PAUSE)     { return EKeys::Pause; }
    if (InKey == GLFW_KEY_CAPS_LOCK) { return EKeys::CapsLock; }
    if (InKey == GLFW_KEY_ESCAPE)    { return EKeys::Escape; }
    if (InKey == GLFW_KEY_SPACE)     { return EKeys::Space; }
    if (InKey == GLFW_KEY_PAGE_UP)   { return EKeys::PageUp; }
    if (InKey == GLFW_KEY_PAGE_DOWN) { return EKeys::PageDown; }
    if (InKey == GLFW_KEY_END)       { return EKeys::End; }
    if (InKey == GLFW_KEY_HOME)      { return EKeys::Home; }

    if (InKey == GLFW_KEY_LEFT)      { return EKeys::Left; }
    if (InKey == GLFW_KEY_UP)        { return EKeys::Up; }
    if (InKey == GLFW_KEY_RIGHT)     { return EKeys::Right; }
    if (InKey == GLFW_KEY_DOWN)      { return EKeys::Down; }

    if (InKey == GLFW_KEY_INSERT)    { return EKeys::Insert; }
    if (InKey == GLFW_KEY_DELETE)    { return EKeys::Delete; }

    if (InKey >= GLFW_KEY_0 && InKey <= GLFW_KEY_9)
    {
        return static_cast<LKey>(InKey - (GLFW_KEY_0 - 1) + (static_cast<i32>(EKeys::Zero) - 1));
    }

    if (InKey >= GLFW_KEY_KP_0 && InKey <= GLFW_KEY_KP_9)
    {
        return static_cast<LKey>(InKey - (GLFW_KEY_KP_0 - 1) + (static_cast<i32>(EKeys::NumPadZero) - 1));
    }

    if (InKey == GLFW_KEY_NUM_LOCK)     { return EKeys::NumPadLock; }
    if (InKey == GLFW_KEY_KP_DIVIDE)    { return EKeys::NumPadDivide; }
    if (InKey == GLFW_KEY_KP_MULTIPLY)  { return EKeys::NumPadMultiply; }
    if (InKey == GLFW_KEY_KP_SUBTRACT)  { return EKeys::NumPadSubtract; }
    if (InKey == GLFW_KEY_KP_ADD)       { return EKeys::NumPadAdd; }
    if (InKey == GLFW_KEY_KP_ENTER)     { return EKeys::NumPadEnter; }
    if (InKey == GLFW_KEY_KP_DECIMAL)   { return EKeys::NumPadDecimal; }

    if (InKey >= GLFW_KEY_F1 && InKey <= GLFW_KEY_F12)
    {
        return static_cast<LKey>(InKey - (GLFW_KEY_F1 - 1) + (static_cast<i32>(EKeys::F1) - 1));
    }

    if (InKey == GLFW_KEY_SCROLL_LOCK)     { return EKeys::ScrollLock; }
    if (InKey == GLFW_KEY_PRINT_SCREEN)    { return EKeys::Print; }

    if (InKey == GLFW_KEY_LEFT_SHIFT)      { return EKeys::LeftShift; }
    if (InKey == GLFW_KEY_RIGHT_SHIFT)     { return EKeys::RightShift; }
    if (InKey == GLFW_KEY_LEFT_CONTROL)    { return EKeys::LeftControl; }
    if (InKey == GLFW_KEY_RIGHT_CONTROL)   { return EKeys::RightControl; }
    if (InKey == GLFW_KEY_LEFT_ALT)        { return EKeys::LeftAlt; }
    if (InKey == GLFW_KEY_RIGHT_ALT)       { return EKeys::RightAlt; }
    if (InKey == GLFW_KEY_LEFT_SUPER)      { return EKeys::LeftCommand; }
    if (InKey == GLFW_KEY_RIGHT_SUPER)     { return EKeys::RightCommand; }

    if (InKey == GLFW_KEY_SEMICOLON)      { return EKeys::Semicolon; }
    if (InKey == GLFW_KEY_EQUAL)          { return EKeys::Equals; }
    if (InKey == GLFW_KEY_COMMA)          { return EKeys::Comma; }
    if (InKey == GLFW_KEY_MINUS)          { return EKeys::Hyphen; }
    if (InKey == GLFW_KEY_PERIOD)         { return EKeys::Period; }
    if (InKey == GLFW_KEY_SLASH)          { return EKeys::Slash; }
    if (InKey == GLFW_KEY_GRAVE_ACCENT)   { return EKeys::Tilde; }
    if (InKey == GLFW_KEY_LEFT_BRACKET)   { return EKeys::LeftBracket; }
    if (InKey == GLFW_KEY_BACKSLASH)      { return EKeys::Backslash; }
    if (InKey == GLFW_KEY_RIGHT_BRACKET)  { return EKeys::RightBracket; }
    if (InKey == GLFW_KEY_APOSTROPHE)     { return EKeys::Apostrophe; }

    return EKeys::Unresolved;
}

} /* ~Namespace Jafg::Glfw3 */
