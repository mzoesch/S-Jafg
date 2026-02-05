// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template<typename... TArgs>
FORCEINLINE LString SprintF(LJafgChar const* Format, TArgs&&... Args) noexcept
{
    return std::vformat(Format, std::make_format_args(Args...));
}

template<typename... TArgs>
FORCEINLINE LWString WSprintF(LWideChar const* Format, TArgs&&... Args) noexcept
{
    return std::vformat(Format, std::make_format_args(Args...));
}

template<typename... TArgs>
FORCEINLINE auto NativeSprintF(LChar const* Format, TArgs&&... Args) noexcept
{
    return std::vformat(Format, std::make_format_args(Args...));
}

} /* ~Namespace Jafg */
