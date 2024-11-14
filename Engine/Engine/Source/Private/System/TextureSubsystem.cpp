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
        Copy.InlineCut(Copy.FindFirst(LDiskVoxelTexture::TexSectionDividerChar));

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
    Copy.InlineCut(Copy.FindLast(LDiskVoxelTexture::TexSectionDividerChar));

    if (ENormalLookup::IsValid(Copy))
    {
        return ENormalLookup::FromString(Copy);
    }

    panicMsgf( "Encountered invalid texture name '{}'.", this->Name )

    return ENormalLookup::None;
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
