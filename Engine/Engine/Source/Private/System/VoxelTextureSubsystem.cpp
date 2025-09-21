// Copyright mzoesch. All rights reserved.

#include "Lal.afx"
#include "System/VoxelTextureSubsystem.h"
#include "System/EnginePath.h"
#include "System/VoxelSubsystem.h"
#include "User/UserPreferences.h"

LString Jafg::LDiskVoxelTexture::GetVoxelName() const
{
    LString Out;

    for (const char& Rune : this->Name)
    {
        if (Rune == LDiskVoxelTexture::TexSectionDividerChar)
        {
            break;
        }

        Out.Append(Rune);

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
        LString Copy = this->Name;
        Copy.InlineRightCut(Copy.FindFirst(LDiskVoxelTexture::TexSectionDividerChar) + 1);

        if (ENormalLookup::IsValid(Copy))
        {
            return ENormalLookup::FromString(Copy);
        }

        return ENormalLookup::Omnia;
    }

    check( this->Name.Count(LDiskVoxelTexture::TexSectionDividerChar) == 2 )

    LString Copy = this->Name;
    const auto FirstOccurrence = Copy.FindFirst(LDiskVoxelTexture::TexSectionDividerChar) + 1;
    Copy.InlineCut(FirstOccurrence, Copy.FindLast(LDiskVoxelTexture::TexSectionDividerChar));

    if (ENormalLookup::IsValid(Copy))
    {
        return ENormalLookup::FromString(Copy);
    }

    Copy = this->Name;
    const auto SecondOccurrence = Copy.FindLast(LDiskVoxelTexture::TexSectionDividerChar);
    Copy.InlineCut(SecondOccurrence, SecondOccurrence - 1);

    if (ENormalLookup::IsValid(Copy))
    {
        return ENormalLookup::FromString(Copy);
    }

    panicMsgf( "Encountered invalid texture name '{}'.", this->Name )

    return ENormalLookup::None;
}

Jafg::LTextureIndex Jafg::LDiskVoxelTexture::GetBlendLookUpBasedOfFileName(const TArray<LDiskBlendTexture>& InCurrentUsedBlends) const
{
    if (this->Name.Count(LDiskVoxelTexture::TexSectionDividerChar) > 2)
    {
        panicMsgf( "Encountered invalid texture name '{}'.", this->Name )
        return ENormalLookup::None;
    }

    if (this->Name.Count(LDiskVoxelTexture::TexSectionDividerChar) == 0)
    {
        const  TArray<LDiskBlendTexture>::SizeType Idx { InCurrentUsedBlends.FindIndexByPredicate([](const LDiskBlendTexture& InElement) -> bool
        {
            return InElement.Name == "None";
        })};
        check( Idx != InCurrentUsedBlends.end_idx() )
        return static_cast<LTextureIndex>(Idx);
    }

    if (this->Name.Count(LDiskVoxelTexture::TexSectionDividerChar) == 1)
    {
        LString Copy = this->Name;
        Copy.InlineCut(Copy.FindFirst(LDiskVoxelTexture::TexSectionDividerChar) + 1, Copy.end());

        const TArray<LDiskBlendTexture>::SizeType MaybeIdx { InCurrentUsedBlends.FindIndexByPredicate([Copy](const LDiskBlendTexture& InElement) -> bool
        {
            return InElement.Name == Copy;
        })};
        if (MaybeIdx != InCurrentUsedBlends.end_idx())
        {
            return static_cast<LTextureIndex>(MaybeIdx);
        }
        const TArray<LDiskBlendTexture>::SizeType NoneIdx { InCurrentUsedBlends.FindIndexByPredicate([](const LDiskBlendTexture& InElement) -> bool
        {
            return InElement.Name == "None";
        })};
        check( NoneIdx != InCurrentUsedBlends.end_idx() )
        return static_cast<LTextureIndex>(MaybeIdx);
    }

    check( this->Name.Count(LDiskVoxelTexture::TexSectionDividerChar) == 2 )

    LString Copy = this->Name;
    const auto FirstOccurrence = Copy.FindFirst(LDiskVoxelTexture::TexSectionDividerChar) + 1;
    Copy.InlineCut(FirstOccurrence, Copy.FindLast(LDiskVoxelTexture::TexSectionDividerChar));

    if (const TArray<LDiskBlendTexture>::SizeType MaybeIdx { InCurrentUsedBlends.FindIndexByPredicate([Copy](const LDiskBlendTexture& InElement) -> bool
    {
        return InElement.Name == Copy;
    })}; MaybeIdx != InCurrentUsedBlends.end_idx())
    {
        return static_cast<LTextureIndex>(MaybeIdx);
    }

    Copy = this->Name;
    const auto SecondOccurrence = Copy.FindLast(LDiskVoxelTexture::TexSectionDividerChar);
    Copy.InlineRightCut(SecondOccurrence + 1);

    if (const TArray<LDiskBlendTexture>::SizeType MaybeIdx { InCurrentUsedBlends.FindIndexByPredicate([Copy](const LDiskBlendTexture& InElement) -> bool
    {
        return InElement.Name == Copy;
    })}; MaybeIdx != InCurrentUsedBlends.end_idx())
    {
        return static_cast<LTextureIndex>(MaybeIdx);
    }

    panicMsgf( "Encountered invalid texture name '{}'.", this->Name )

    const TArray<LDiskBlendTexture>::SizeType NoneIdx { InCurrentUsedBlends.FindIndexByPredicate([](const LDiskBlendTexture& InElement) -> bool
    {
        return InElement.Name == "None";
    })};
    check( NoneIdx != InCurrentUsedBlends.end_idx() )
    return static_cast<LTextureIndex>(NoneIdx);
}

void Jafg::JVoxelTextureSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Collection.InitializeDependency<JVoxelSubsystem>();
    Super::Initialize(Collection);

    return;
}

void Jafg::JVoxelTextureSubsystem::TearDown()
{
    Super::TearDown();
}

TArray<Jafg::LDiskVoxelTexture> Jafg::JVoxelTextureSubsystem::FindMeaningFullVoxelTextureNames() const
{
    const LPath Voxels { LEnginePath{EEnginePaths::Voxels}.ResolvePath() };
    TArray<LString> Names = Finder::FindFiles(Voxels, false, ".png");

    TArray<LDiskVoxelTexture> Out;
    for (LString& Name : Names)
    {
        Out.Emplace(std::move(Name));
    }

    return Out;
}

TArray<Jafg::LDiskBlendTexture> Jafg::JVoxelTextureSubsystem::FindMeaningBlendTextureNames() const
{
    const LPath Blends { LEnginePath{EEnginePaths::Blends}.ResolvePath() };
    TArray<LString> Names = Finder::FindFiles(Blends, false, ".png");

    TArray<LDiskBlendTexture> Out;
    for (LString& Name : Names)
    {
        Out.Emplace(std::move(Name));
    }

    return Out;
}
