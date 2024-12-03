// Copyright mzoesch. All rights reserved.

#if JAFG_NO_GLFW3
    #error "Including Glfw library specific code without GLFW3 enabled."
#endif /* JAFG_NO_GLFW3 */

#pragma once

#include "InputTypes.h"
#include <GLFW/glfw3.h>

namespace Jafg::Glfw3
{

FORCEINLINE int32 TranslateKeyToGlfw(const LKey InKey)
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

    if (InKey >= EKeys::Zero && InKey <= EKeys::Nine)
    {
        return InKey - (EKeys::Zero - 1) + (GLFW_KEY_0 - 1);
    }

    if (InKey >= EKeys::F1 && InKey <= EKeys::F12)
    {
        return InKey - (EKeys::F1 - 1) + (GLFW_KEY_F1 - 1);
    }

    if (InKey == EKeys::Escape)
    {
        return GLFW_KEY_ESCAPE;
    }

    return INDEX_NONE;
}

} /* ~Namespace Jafg::Glfw3 */
