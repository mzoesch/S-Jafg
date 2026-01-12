// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi.h"

namespace Jafg
{

struct LFetchedSave
{
    //#
    //# The path to the directory containing the save files.
    //#
    LPath Path;

    //#
    //# True if the save is corrupted in any way.
    //#
    bool bCorrupted{ false };

    //#
    //# The display name of the save. Unavailable if the save is corrupted.
    //#
    LString DisplayName;

    LString Description;

    //#
    //# If a preview texture is available, it will be set here.
    //#
    std::optional<LImage> PreviewTexture;

    FORCEINLINE bool IsValid() const noexcept { return this->Path.empty() == false; }
    FORCEINLINE bool IsCorrupted() const noexcept { return this->DisplayName.empty(); }
};

} /* ~Namespace Jafg */
