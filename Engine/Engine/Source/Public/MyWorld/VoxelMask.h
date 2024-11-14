// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "CommonTypes.h"

namespace Jafg
{

class JMaterialSubsystem;

struct LVoxelMask
{
    /** Needs to add the normal lookup and the texture indices. */
    friend JMaterialSubsystem;

    LVoxelMask() = delete;
    LVoxelMask(LSimpleString Namespace, LAsciiString Name) : Namespace(std::move(Namespace)), Name(std::move(Name)) { }
    ~LVoxelMask() = default;

    FORCEINLINE const LSimpleString& GetNamespace(void) const { return this->Namespace; }
    FORCEINLINE const LSimpleString& GetName(void) const { return this->Name; }

    static LVoxelMask Null;
    static LVoxelMask Air;

    FORCEINLINE LTextureIndex FindTextureIndex(const LVector& InNormal) const;
    FORCEINLINE LTextureIndex FindTextureIndex(const LNormalLookup InNormal) const;

private:

    LSimpleString Namespace;
    LSimpleString Name;

    struct LTextureGroup
    {
        LNormalLookup Normal;
        LTextureIndex Index;
    };

    TdhArray<LTextureGroup> TextureGroups = { };
};

LTextureIndex LVoxelMask::FindTextureIndex(const LVector& InNormal) const
{
    return this->FindTextureIndex(ENormalLookup::FromVector(InNormal));
}

LTextureIndex LVoxelMask::FindTextureIndex(const LNormalLookup InNormal) const
{
    for (const auto& [Normal, Index] : this->TextureGroups)
    {
        if (Normal & InNormal)
        {
            return Index;
        }

        continue;
    }

    panicMsgf( "Found malformed voxel mask in namespace '%s' with name '%s'.", this->Namespace, this->Name )

    return INDEX_NONE;
}

} /* ~Namespace Jafg */
