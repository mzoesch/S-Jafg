// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template<typename... TArgs>
FORCEINLINE LString SprintF(LChar const* Format, TArgs&&... Args) noexcept
{
    return std::vformat(Format, std::make_format_args(Args...));
}

} /* ~Namespace Lal */
