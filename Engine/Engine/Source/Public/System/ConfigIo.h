// Copyright mzoesch. All rights reserved.

#pragma once

#include "EnginePath.h"

namespace Jafg::ConfigIo
{

struct Entry
{
    LString InSection;
    LString InKey;
    LString InValue;
};

//#
//# Serialize a value to a configuration file.
//#
//# @param InPath    The file path (has to be writable).
//# @param InSection The section to write to.
//# @param InKey     The unique key of said section.
//# @param InValue   The string representation of the value to serialize.
//# @return True if something was written, changed or updated, false otherwise.
//#
ENGINE_API bool Serialize(const LPath& InPath, const LStringView& InSection, const LStringView& InKey, const LStringView& InValue, const bool bDoBackup = true);
ENGINE_API bool SerializeBulk(const LPath& InPath, const TArray<Entry>& InEntries, const bool bDoBackup = true);

//#
//# Deserialize a value from a configuration file.
//#
//# @param InPath    The file path (has to be writable).
//# @param InSection The section to read from.
//# @param InKey     The unique key of said section.
//# @return The deserialized value if it was found, nothing otherwise.
//#
ENGINE_API TOptional<LString> Deserialize(const LPath& InPath, const LStringView& InSection, const LStringView& InKey);

} /* ~Namespace Jafg::ConfigIo */
