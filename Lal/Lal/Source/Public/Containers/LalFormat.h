// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

#if LAL_WITH_CLANG
namespace Private
{
template<typename TChar, typename TTraits, typename TAlloc>
using StringSink = std::__format::_Seq_sink<TSimpleString<TChar, TTraits, TAlloc>>;
} /* ~Namespace Private */
#endif /* LAL_WITH_CLANG */

template<typename... TArgs>
FORCEINLINE LString SprintF(LChar const* Format, TArgs&&... Args) noexcept
{
#if LAL_WITH_CLANG
    typedef Private::StringSink<LString::value_type, LString::traits_type, LString::allocator_type> LSink;
    LSink Sink;
    std::vformat_to(Sink.out(), Format, std::make_format_args(Args...));
    return std::move(Sink).get();
#else /* LAL_WITH_CLANG */
    /* return std::vformat(Format, std::make_format_args(Args...)); */
    #error "Missing implementation for compiler."
#endif /* !LAL_WITH_CLANG */
}

} /* ~Namespace Lal */
