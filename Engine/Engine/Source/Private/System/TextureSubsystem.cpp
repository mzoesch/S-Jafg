// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "System/TextureSubsystem.h"
#include "System/EnginePath.h"
#include "System/VoxelSubsystem.h"
#include "User/UserPreferences.h"

Jafg::LSimpleString Jafg::LDiskVoxelTexture::GetVoxelName() const
{
    LSimpleString Out;

    for (const LChar& Rune : this->Name)
    {
        if (Rune == LDiskVoxelTexture::TexSectionDividerChar)
        {
            break;
        }

        Out.Add(Rune);

        continue;
    }

    check( Out.IsEmpty() == false )

    return Out;
}

Jafg::ENormalLookup::Type Jafg::LDiskVoxelTexture::GetNormalLookUpBasedOfFileName() const
{
    if (this->Name.Count(LDiskVoxelTexture::TexSectionDividerChar) > 2)
    {
        panicMsgf( "Encountered invalid texture name '{}'.", this->Name )
        return ENormalLookup::None;
    }

    if (this->Name.Count(LDiskVoxelTexture::TexSectionDividerChar) == 0)
    {
        return ENormalLookup::Omnia;
    }

    if (this->Name.Count(LDiskVoxelTexture::TexSectionDividerChar) == 1)
    {
        LSimpleString Copy = this->Name;
        Copy.InlineSubIdx(Copy.FindFirst(LDiskVoxelTexture::TexSectionDividerChar) + 1, Copy.GetRuneCount());

        if (ENormalLookup::IsValid(Copy))
        {
            return ENormalLookup::FromString(Copy);
        }

        return ENormalLookup::Omnia;
    }

    check( this->Name.Count(LDiskVoxelTexture::TexSectionDividerChar) == 2 )

    LSimpleString Copy = this->Name;
    const int32 FirstOccurrence = Copy.FindFirst(LDiskVoxelTexture::TexSectionDividerChar) + 1;
    Copy.InlineSub(FirstOccurrence, Copy.FindLast(LDiskVoxelTexture::TexSectionDividerChar) - FirstOccurrence);

    if (ENormalLookup::IsValid(Copy))
    {
        return ENormalLookup::FromString(Copy);
    }

    Copy = this->Name;
    const int32 SecondOccurrence = Copy.FindLast(LDiskVoxelTexture::TexSectionDividerChar);
    Copy.InlineSub(SecondOccurrence, Copy.GetRuneCount() - SecondOccurrence - 1);

    if (ENormalLookup::IsValid(Copy))
    {
        return ENormalLookup::FromString(Copy);
    }

    panicMsgf( "Encountered invalid texture name '{}'.", this->Name )

    return ENormalLookup::None;
}

Jafg::LTextureIndex Jafg::LDiskVoxelTexture::GetBlendLookUpBasedOfFileName(const TdhArray<LDiskBlendTexture>& InCurrentUsedBlends) const
{
    if (this->Name.Count(LDiskVoxelTexture::TexSectionDividerChar) > 2)
    {
        panicMsgf( "Encountered invalid texture name '{}'.", this->Name )
        return ENormalLookup::None;
    }

    if (this->Name.Count(LDiskVoxelTexture::TexSectionDividerChar) == 0)
    {
        const DefaultContainerSizeType Idx = InCurrentUsedBlends.FindIndexByPredicate([] (const LDiskBlendTexture& InElement) -> bool
        {
            return InElement.Name == "None";
        });
        check( Idx != INDEX_NONE )
        return static_cast<LTextureIndex>(Idx);
    }

    if (this->Name.Count(LDiskVoxelTexture::TexSectionDividerChar) == 1)
    {
        LSimpleString Copy = this->Name;
        Copy.InlineSubIdx(Copy.FindFirst(LDiskVoxelTexture::TexSectionDividerChar) + 1, Copy.GetRuneCount());

        const DefaultContainerSizeType MaybeIdx = InCurrentUsedBlends.FindIndexByPredicate([Copy] (const LDiskBlendTexture& InElement) -> bool
        {
            return InElement.Name == Copy;
        });
        if (MaybeIdx != INDEX_NONE)
        {
            return static_cast<LTextureIndex>(MaybeIdx);
        }
        const DefaultContainerSizeType NoneIdx = InCurrentUsedBlends.FindIndexByPredicate([] (const LDiskBlendTexture& InElement) -> bool
        {
            return InElement.Name == "None";
        });
        check( NoneIdx != INDEX_NONE )
        return static_cast<LTextureIndex>(MaybeIdx);
    }

    check( this->Name.Count(LDiskVoxelTexture::TexSectionDividerChar) == 2 )

    LSimpleString Copy = this->Name;
    const int32 FirstOccurrence = Copy.FindFirst(LDiskVoxelTexture::TexSectionDividerChar) + 1;
    Copy.InlineSub(FirstOccurrence, Copy.FindLast(LDiskVoxelTexture::TexSectionDividerChar) - FirstOccurrence);

    if (const DefaultContainerSizeType MaybeIdx = InCurrentUsedBlends.FindIndexByPredicate([Copy] (const LDiskBlendTexture& InElement) -> bool
    {
        return InElement.Name == Copy;
    }); MaybeIdx != INDEX_NONE)
    {
        return static_cast<LTextureIndex>(MaybeIdx);
    }

    Copy = this->Name;
    const int32 SecondOccurrence = Copy.FindLast(LDiskVoxelTexture::TexSectionDividerChar);
    Copy.InlineSub(SecondOccurrence + 1, Copy.GetRuneCount() - SecondOccurrence - 1);

    if (const DefaultContainerSizeType MaybeIdx = InCurrentUsedBlends.FindIndexByPredicate([Copy] (const LDiskBlendTexture& InElement) -> bool
    {
        return InElement.Name == Copy;
    }); MaybeIdx != INDEX_NONE)
    {
        return static_cast<LTextureIndex>(MaybeIdx);
    }

    panicMsgf( "Encountered invalid texture name '{}'.", this->Name )

    const DefaultContainerSizeType NoneIdx = InCurrentUsedBlends.FindIndexByPredicate([] (const LDiskBlendTexture& InElement) -> bool
    {
        return InElement.Name == "None";
    });
    check( NoneIdx != INDEX_NONE )
    return static_cast<LTextureIndex>(NoneIdx);
}

void Jafg::JTextureSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Collection.InitializeDependency<JVoxelSubsystem>();
    Super::Initialize(Collection);

    return;
}

void Jafg::JTextureSubsystem::TearDown()
{
    Super::TearDown();
}

Jafg::TdhArray<Jafg::LDiskVoxelTexture> Jafg::JTextureSubsystem::FindMeaningFullVoxelTextureNames() const
{
    TdhArray<LSimpleString> Names = Finder::FindFiles(EEnginePaths::Voxels, *GetDefault<JUserPreferences>(), false, ".png");

    TdhArray<LDiskVoxelTexture> Out;
    for (LSimpleString& Name : Names)
    {
        Out.Emplace(Name.MoveOut());
    }

    return Out;
}

Jafg::TdhArray<Jafg::LDiskBlendTexture> Jafg::JTextureSubsystem::FindMeaningBlendTextureNames() const
{
    TdhArray<LSimpleString> Names = Finder::FindFiles(EEnginePaths::Blends, *GetDefault<JUserPreferences>(), false, ".png");

    TdhArray<LDiskBlendTexture> Out;
    for (LSimpleString& Name : Names)
    {
        Out.Emplace(Name.MoveOut());
    }

    return Out;
}
