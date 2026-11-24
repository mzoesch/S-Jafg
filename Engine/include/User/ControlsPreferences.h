// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/PreferenceCollection.h"
#include "User/Input/InputTypes.h"

namespace Jafg
{

struct LInputTrigger;
struct LInputAction;
struct LInputContext;

namespace Controls
{

//# Human-readable name of a key. Keys that print a character use the current keyboard layout.
NODISCARD ENGINE_API LString GetKeyDisplayName(LPhysicalKey Key) noexcept;
//# Human-readable name of a chord. E.g., "Ctrl+Shift+S".
NODISCARD ENGINE_API LString GetKeyChordDisplayName(LKeyChord const& Chord) noexcept;

//# The stable textual form of chords that is used for persistence. E.g., "W|Ctrl+Up". Chords are separated by '|'.
NODISCARD ENGINE_API LString SerializeKeyChords(TArray<LKeyChord> const& Chords) noexcept;
//# @return Empty if any chord could not be parsed. An empty text is a valid list without chords.
NODISCARD ENGINE_API std::optional<TArray<LKeyChord>> ParseKeyChords(LStringView Text) noexcept;

//#
//# Makes the collection list the contexts, actions and triggers of the user input registry, so that the user can
//# change the keys of the triggers. The collection is regenerated every time a host refreshes it.
//#
ENGINE_API void InstallControlsCollection(LPreferenceCollection& Collection) noexcept;

} /* ~Namespace Controls */

} /* ~Namespace Jafg */
