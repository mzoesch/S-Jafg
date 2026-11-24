// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

//# Helpers to match search queries that users type against texts.
namespace TextSearch
{

//# A lower case copy. ASCII only, other bytes are kept.
NODISCARD ENGINE_API LString ToLowerAscii(LStringView Text);

//# Splits a query at whitespace into lower case tokens.
NODISCARD ENGINE_API TArray<LString> Tokenize(LStringView Query);

//#
//# Fuzzy matching: Whether the characters of the token occur in this order in the text, but not necessarily next to
//# each other. Case-insensitive. E.g., "mt", "mplt" and "metal" all match "MetalPlate".
//#
//# Matches score higher when their characters are next to each other and when they start words (after a separator
//# like '/', '.', '_', '-', a space, or at a camel case hump like the 'P' of "MetalPlate"). Gaps cost a little.
//#
//# @param Token Lower case. @see #Tokenize
//# @return Empty, if the token does not match. Higher is better. An empty token matches everything with zero.
//#
NODISCARD ENGINE_API std::optional<i32> FuzzyScore(LStringView Text, LStringView Token);

} /* ~Namespace TextSearch */

} /* ~Namespace Jafg */
