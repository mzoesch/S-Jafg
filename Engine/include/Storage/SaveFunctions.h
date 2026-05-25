// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg::Saves
{

struct LMinimalMetaData
{
    LString DisplayName;
    LString Description;
};

//#
//# Create a new save.
//# This includes the directory of the save as well as the minimum default boilerplate for the save structure.
//# @param InPath The path to the directory that should hold the new save files associated with it.
//#
ENGINE_API bool CreateNewSave(const LPath& InPath, const LMinimalMetaData& Meta, LString* OutError = nullptr);

ENGINE_API std::optional<LString> GetDisplayName(const LPath& InPath, LString* OutError = nullptr);

} /* ~Namespace Jafg::Saves */
