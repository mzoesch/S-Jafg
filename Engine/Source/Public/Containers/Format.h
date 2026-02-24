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
#if JAFG_PLATFORM_USES_UTF8
    return std::vformat(Format, std::make_format_args(Args...));
#else /* JAFG_PLATFORM_USES_UTF8 */
    return std::vformat(Format, std::make_wformat_args(Args...));
#endif /* !JAFG_PLATFORM_USES_UTF8 */
}

} /* ~Namespace Jafg */
