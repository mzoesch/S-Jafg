// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "CommonTypes.h"

namespace Jafg
{

class JVoxelSubsystem;
class JMaterialSubsystem;

struct LVoxelMask
{
    friend JMaterialSubsystem;
    friend JVoxelSubsystem;

    LVoxelMask() = delete;
    LVoxelMask(LSimpleString Namespace, LSimpleString Name) : Namespace(std::move(Namespace)), Name(std::move(Name)) { }
    ~LVoxelMask() = default;

    FORCEINLINE const LSimpleString& GetNamespace(void) const { return this->Namespace; }
    FORCEINLINE const LSimpleString& GetName(void) const { return this->Name; }

    static LVoxelMask Null;
    static LVoxelMask Air;

    FORCEINLINE LTextureIndex FindTextureIndex(const LVector& InNormal) const;
    FORCEINLINE LTextureIndex FindTextureIndex(const LNormalLookup InNormal) const;

    FORCEINLINE LTextureIndex FindBlendTextureIndex(const LVector& InNormal) const;
    FORCEINLINE LTextureIndex FindBlendTextureIndex(const LNormalLookup InNormal) const;

private:

    LSimpleString Namespace;
    LSimpleString Name;

    struct LTextureGroup
    {
        FORCEINLINE LTextureGroup(const LNormalLookup InNormal, const LTextureIndex InIndex, const LTextureIndex InBlendIndex)
            : Normal(InNormal), Index(InIndex), BlendIndex(InBlendIndex) { }

        LNormalLookup Normal;
        LTextureIndex Index;
        LTextureIndex BlendIndex;
    };

    TdhArray<LTextureGroup> TextureGroups = { };
};

LTextureIndex LVoxelMask::FindTextureIndex(const LVector& InNormal) const
{
    return this->FindTextureIndex(ENormalLookup::FromVector(InNormal));
}

LTextureIndex LVoxelMask::FindTextureIndex(const LNormalLookup InNormal) const
{
    for (const LTextureGroup& Group : this->TextureGroups)
    {
        if (Group.Normal & InNormal)
        {
            return Group.Index;
        }

        continue;
    }

    panicMsgf( "Found malformed voxel mask in namespace '%s' with name '%s'.", this->Namespace, this->Name )

    return INDEX_NONE;
}

LTextureIndex LVoxelMask::FindBlendTextureIndex(const LVector& InNormal) const
{
    return this->FindBlendTextureIndex(ENormalLookup::FromVector(InNormal));
}

LTextureIndex LVoxelMask::FindBlendTextureIndex(const LNormalLookup InNormal) const
{
    for (const LTextureGroup& Group : this->TextureGroups)
    {
        if (Group.Normal & InNormal)
        {
            return Group.BlendIndex;
        }

        continue;
    }

    panicMsgf( "Found malformed voxel mask in namespace '%s' with name '%s'.", this->Namespace, this->Name )

    return INDEX_NONE;
}

} /* ~Namespace Jafg */
