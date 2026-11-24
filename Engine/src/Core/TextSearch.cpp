// Copyright mzoesch. All rights reserved.

#include "Core/TextSearch.h"

namespace
{

NODISCARD constexpr char ToLowerAscii(char Char) noexcept
{
    return Char >= 'A' && Char <= 'Z' ? static_cast<char>(Char - 'A' + 'a') : Char;
}
NODISCARD constexpr bool IsUpperAscii(char Char) noexcept { return Char >= 'A' && Char <= 'Z'; }
NODISCARD constexpr bool IsLowerAscii(char Char) noexcept { return Char >= 'a' && Char <= 'z'; }
NODISCARD constexpr bool IsDigitAscii(char Char) noexcept { return Char >= '0' && Char <= '9'; }
NODISCARD constexpr bool IsSpaceAscii(char Char) noexcept { return Char == ' ' || Char == '\t' || Char == '\n' || Char == '\r'; }
NODISCARD constexpr bool IsSeparator(char Char) noexcept
{
    return IsSpaceAscii(Char) || Char == '/' || Char == '\\' || Char == '.' || Char == '_' || Char == '-' || Char == ':' || Char == '#';
}

/* Tuned so that consecutive characters at word starts always win over scattered ones. */
constexpr i32 MatchScore{ 16 };
constexpr i32 WordStartBonus{ 24 };
constexpr i32 CamelHumpBonus{ 16 };
constexpr i32 ConsecutiveBonus{ 16 };
constexpr i32 GapPenalty{ 1 };
constexpr i32 Unmatched{ std::numeric_limits<i32>::min() / 4 };

//# The bonus for a match at this index of the text.
NODISCARD constexpr i32 GetPositionBonus(LStringView Text, std::size_t Idx) noexcept
{
    if (Idx == 0uz || IsSeparator(Text[Idx - 1uz]))
    {
        return WordStartBonus;
    }
    char const Previous{Text[Idx - 1uz]};
    char const Current{Text[Idx]};
    if ((IsUpperAscii(Current) && IsLowerAscii(Previous)) || (IsDigitAscii(Current) != IsDigitAscii(Previous)))
    {
        return CamelHumpBonus;
    }
    return 0;
}

} /* ~Namespace <Anonymous> */

LString Jafg::TextSearch::ToLowerAscii(LStringView Text)
{
    LString Result{Text};
    for (auto& Char: Result)
    {
        Char = ::ToLowerAscii(Char);
    }
    return Result;
}

TArray<LString> Jafg::TextSearch::Tokenize(LStringView Query)
{
    TArray<LString> Tokens;
    LString Current;
    for (char const Char: Query)
    {
        if (IsSpaceAscii(Char))
        {
            if (!Current.empty())
            {
                Tokens.push_back(std::move(Current));
                Current.clear();
            }
        }
        else
        {
            Current.push_back(::ToLowerAscii(Char));
        }
    }
    if (!Current.empty())
    {
        Tokens.push_back(std::move(Current));
    }
    return Tokens;
}

std::optional<i32> Jafg::TextSearch::FuzzyScore(LStringView Text, LStringView Token)
{
    if (Token.empty())
    {
        return 0;
    }
    if (Token.size() > Text.size())
    {
        return {};
    }

    /*
     * The best alignment. Previous[Idx] / Current[Idx] is the best score of the token up to the current character
     * with that character matched at the index of the text.
     */
    TArray<i32> Previous(Text.size(), Unmatched);
    TArray<i32> Current(Text.size(), Unmatched);

    for (auto TokenIdx{0uz}; TokenIdx < Token.size(); ++TokenIdx)
    {
        /* Max of (Previous[K] + K) for K <= Idx - 2. A gap of (Idx - K - 1) characters costs that many penalties. */
        i32 BestBeforeGap{Unmatched};
        for (auto Idx{0uz}; Idx < Text.size(); ++Idx)
        {
            if (TokenIdx > 0uz && Idx >= 2uz && Previous[Idx - 2uz] != Unmatched)
            {
                BestBeforeGap = maths::max(BestBeforeGap, Previous[Idx - 2uz] + static_cast<i32>(Idx - 2uz) * GapPenalty);
            }

            Current[Idx] = Unmatched;
            if (::ToLowerAscii(Text[Idx]) != Token[TokenIdx])
            {
                continue;
            }

            i32 const Gain{MatchScore + GetPositionBonus(Text, Idx)};
            if (TokenIdx == 0uz)
            {
                Current[Idx] = Gain;
                continue;
            }

            i32 Best{Unmatched};
            if (Idx >= 1uz && Previous[Idx - 1uz] != Unmatched)
            {
                Best = Previous[Idx - 1uz] + ConsecutiveBonus;
            }
            if (BestBeforeGap != Unmatched)
            {
                Best = maths::max(Best, BestBeforeGap - static_cast<i32>(Idx - 1uz) * GapPenalty);
            }
            if (Best != Unmatched)
            {
                Current[Idx] = Best + Gain;
            }
        }
        std::swap(Previous, Current);
    }

    i32 const Result{*std::ranges::max_element(Previous)};
    if (Result == Unmatched)
    {
        return {};
    }
    return Result;
}
