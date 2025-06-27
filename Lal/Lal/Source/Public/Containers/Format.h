// Copyright mzoesch. All rights reserved.

#pragma once

#if PLATFORM_WASM

namespace Jafg
{

template <typename TString, typename ... ArgTy>
NODISCARD TString Format(const char* Format, const ArgTy&... Args);

template <typename ... ArgTy>
NODISCARD LStringLegacy FormatLegacy(const char* Format, const ArgTy& ... Args);

template <typename ArgTy>
inline auto FormatArgLegacy(const ArgTy Arg);

template <> NODISCARD inline auto FormatArgLegacy(char* const Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const char* const Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const LStringLegacy Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(LStringLegacy& Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const LStringLegacy& Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(LStringLegacy&& Arg) { return std::forward<LStringLegacy>(Arg); }
template <> NODISCARD inline auto FormatArgLegacy(const LStringLegacy&& Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const LStringView Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const LStringView& Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const LString Arg) { LStringLegacy Out = Arg.ToC(); return Out; }
template <> NODISCARD inline auto FormatArgLegacy(const LString& Arg) { return Arg.ToC(); }
template <> NODISCARD inline auto FormatArgLegacy(const LSimpleString Arg) { LStringLegacy Out = Arg.ToC(); return Out; }
template <> NODISCARD inline auto FormatArgLegacy(const LSimpleString& Arg) { return Arg.ToC(); }
template <> NODISCARD inline auto FormatArgLegacy(const float Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const double Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const bool Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const i8 Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const i16 Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const i32 Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const i64 Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const u8 Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const u16 Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const u32 Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const u64 Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const long int Arg) { return Arg; }
template <> NODISCARD inline auto FormatArgLegacy(const unsigned long int Arg) { return Arg; }

template <typename TString, typename ... ArgTy>
NODISCARD TString Format(const char* Format, const ArgTy&... Args)
{
    TString Out = Jafg::FormatLegacy(Format, Args...).c_str();
    return Out;
}

template <typename ... ArgTy>
NODISCARD LStringLegacy FormatLegacy(const char* Format, const ArgTy& ... Args)
{
    if constexpr (sizeof ... (Args) == 0)
    {
        return Format;
    }

    if constexpr (sizeof ... (Args) == 1)
    {
        auto FArg = Jafg::FormatArgLegacy(Args ...);
        return std::vformat(Format, std::make_format_args(FArg));
    }

    auto Tuple = std::make_tuple(Jafg::FormatArgLegacy(Args) ...);
    return std::apply( [Format] (const auto& ... LambdaUnpacked)
        {
            return std::vformat(Format, std::make_format_args(LambdaUnpacked ...));
        },
        Tuple
    );
}

} /* ~Namespace Jafg */

#endif /* PLATFORM_WASM */
