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

        Out.push_back(Rune);

        continue;
    }

    check( Out.empty() == false )

    return Out;
}

Jafg::ENormalLookup::Type Jafg::LDiskVoxelTexture::GetNormalLookUpBasedOfFileName() const
{
    if (algo::count(this->Name, LDiskVoxelTexture::TexSectionDividerChar) > 2)
    {
        panicMsgf( "Encountered invalid texture name '{}'.", this->Name )
        return ENormalLookup::None;
    }

    if (algo::count(this->Name, LDiskVoxelTexture::TexSectionDividerChar) == 0)
    {
        return ENormalLookup::Omnia;
    }

    if (algo::count(this->Name, LDiskVoxelTexture::TexSectionDividerChar) == 1)
    {
        if (LString const Copy { this->Name.substr(this->Name.find(LDiskVoxelTexture::TexSectionDividerChar) + 1) }; ENormalLookup::IsValid(Copy))
        {
            return ENormalLookup::FromString(Copy);
        }

        return ENormalLookup::Omnia;
    }

    check( algo::count(this->Name, LDiskVoxelTexture::TexSectionDividerChar) == 2 )

    LString Copy = this->Name;
    const auto FirstOccurrence { Copy.find(LDiskVoxelTexture::TexSectionDividerChar) + 1 };
    auto x = Copy.substr(FirstOccurrence, Copy.rfind(LDiskVoxelTexture::TexSectionDividerChar) - FirstOccurrence);
    Copy = x;

    if (ENormalLookup::IsValid(Copy))
    {
        return ENormalLookup::FromString(Copy);
    }

    Copy = this->Name;
    const auto SecondOccurrence { Copy.rfind(LDiskVoxelTexture::TexSectionDividerChar) };
    Copy = Copy.substr(SecondOccurrence + 1, SecondOccurrence);

    if (ENormalLookup::IsValid(Copy))
    {
        return ENormalLookup::FromString(Copy);
    }

    panicMsgf( "Encountered invalid texture name '{}'.", this->Name )

    return ENormalLookup::None;
}

Jafg::LTextureIndex Jafg::LDiskVoxelTexture::GetBlendLookUpBasedOfFileName(const TArray<LDiskBlendTexture>& InCurrentUsedBlends) const
{
    if (algo::count(this->Name, LDiskVoxelTexture::TexSectionDividerChar) > 2)
    {
        panicMsgf( "Encountered invalid texture name '{}'.", this->Name )
        return ENormalLookup::None;
    }

    if (algo::count(this->Name, LDiskVoxelTexture::TexSectionDividerChar) == 0)
    {
        auto const It{ algo::find(InCurrentUsedBlends, "None", &LDiskBlendTexture::Name) };
        check( It != InCurrentUsedBlends.end() )
        return static_cast<LTextureIndex>(algo::distance(InCurrentUsedBlends.begin(), It));
    }

    if (algo::count(this->Name, LDiskVoxelTexture::TexSectionDividerChar) == 1)
    {
        LString const Copy { this->Name.substr(this->Name.find(LDiskVoxelTexture::TexSectionDividerChar) + 1) };
        if (auto const It{ algo::find(InCurrentUsedBlends, Copy, &LDiskBlendTexture::Name) }; It != InCurrentUsedBlends.end())
        {
            return static_cast<LTextureIndex>(algo::distance(InCurrentUsedBlends.begin(), It));
        }

        auto const NoneIt{ algo::find(InCurrentUsedBlends, "None", &LDiskBlendTexture::Name) };
        check( NoneIt != InCurrentUsedBlends.end() )
        return static_cast<LTextureIndex>(algo::distance(InCurrentUsedBlends.begin(), NoneIt));
    }

    check( algo::count(this->Name, LDiskVoxelTexture::TexSectionDividerChar) == 2 )

    LString Copy { this->Name.substr(this->Name.find(LDiskVoxelTexture::TexSectionDividerChar) + 1, this->Name.rfind(LDiskVoxelTexture::TexSectionDividerChar)) };

    if (auto const MaybeIt{ algo::find(InCurrentUsedBlends, Copy, &LDiskBlendTexture::Name) }; MaybeIt != InCurrentUsedBlends.end())
    {
        return static_cast<LTextureIndex>(algo::distance(InCurrentUsedBlends.begin(), MaybeIt));
    }

    Copy = this->Name.substr(this->Name.rfind(LDiskVoxelTexture::TexSectionDividerChar) + 1);
    if (auto const MaybeIt{ algo::find(InCurrentUsedBlends, Copy, &LDiskBlendTexture::Name) }; MaybeIt != InCurrentUsedBlends.end())
    {
        return static_cast<LTextureIndex>(algo::distance(InCurrentUsedBlends.begin(), MaybeIt));
    }

    panicMsgf( "Encountered invalid texture name '{}'.", this->Name )
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
        Out.emplace_back(std::move(Name));
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
        Out.emplace_back(std::move(Name));
    }

    return Out;
}
