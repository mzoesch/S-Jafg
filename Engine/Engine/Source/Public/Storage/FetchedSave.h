// Copyright mzoesch. All rights reserved.

#pragma once

#include "System/Path.h"
#include "Rhi/Texture2.h"

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
    bool bCorrupted { false };

    //#
    //# The display name of the save. Unavailable if the save is corrupted.
    //#
    LString DisplayName;

    LString Description;

    //#
    //# If a preview texture is available, it will be set here.
    //#
    LTexture2 PreviewTexture;

    FORCEINLINE bool IsValid() const noexcept { return this->Path.IsEmpty() == false; }
    FORCEINLINE bool IsCorrupted() const noexcept { return this->DisplayName.IsEmpty(); }
    FORCEINLINE bool IsPreviewTextureValid() const noexcept { return this->PreviewTexture.IsValid(); }

    FORCEINLINE void ValueCopy(const LFetchedSave& Other) noexcept
    {
        this->Path = Other.Path;
        this->bCorrupted = Other.bCorrupted;
        this->DisplayName = Other.DisplayName;
        this->Description = Other.Description;

        return;
    }
};

} /* ~Namespace Jafg */
