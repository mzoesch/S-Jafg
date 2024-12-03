// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "System/MaterialSubsystem.h"
#include "Engine/Framework/ApplicationInstance.h"
#include "System/Finder.h"
#include "System/TextureSubsystem.h"
#include "System/VoxelSubsystem.h"
#include "System/EnginePath.h"
#include "User/UserPreferences.h"

void Jafg::JMaterialSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Collection.InitializeDependency<JVoxelSubsystem>();
    Collection.InitializeDependency<JTextureSubsystem>();
    Super::Initialize(Collection);

    this->LoadAllTextures();

    return;
}

void Jafg::JMaterialSubsystem::TearDown()
{
    Super::TearDown();

    return;
}

void Jafg::JMaterialSubsystem::ReloadAllTextures()
{
    this->ClearAllTextures();
    this->LoadAllTextures();

    return;
}

void Jafg::JMaterialSubsystem::ClearAllTextures()
{
    this->BlendOpaqueAtlas.Free();
    this->BlendersAtlas.Free();

    return;
}

void Jafg::JMaterialSubsystem::LoadAllTextures()
{
    JVoxelSubsystem* VoxelSubsystem = this->GetApplicationInstance()->GetCheckedSubsystem<JVoxelSubsystem>();
    JTextureSubsystem* TextureSubsystem = this->GetApplicationInstance()->GetCheckedSubsystem<JTextureSubsystem>();

#pragma region Blending
    TdhArray<LDiskBlendTexture> BlendedTextureNames = TextureSubsystem->FindMeaningBlendTextureNames();
    {
        TdhArray<LTexture2> LoadedTextures;

        for (const LDiskBlendTexture& DiskTexture : BlendedTextureNames)
        {
            LEnginePath Path = LEnginePath(EEnginePaths::Blends, DiskTexture.Name.ToPtr());
            Path.AddExtension(".png");

            LTexture2 Texture;
            if (Texture.LoadFromDisk(Path, *GetDefault<JUserPreferences>()) == false)
            {
                panicMsgf(
                    "Failed to load texture: {}. Faulty path: {}.",
                    DiskTexture.Name, Path.GetRelativeUnresolvedPath().GetPath()
                )
                continue;
            }

            if (Texture.GetFirstMipMap().Size.X != Texture.GetFirstMipMap().Size.Y)
            {
                panicMsgf(
                    "Texture is not square: {}. Size: {}x{}.",
                    DiskTexture.Name, Texture.GetFirstMipMap().Size.X, Texture.GetFirstMipMap().Size.Y
                )
                continue;
            }

            if (Maths::IsPowerOfTwo(Texture.GetFirstMipMap().Size.X) == false)
            {
                panicMsgf(
                    "Texture is not power of two: {}. Size: {}x{}.",
                    DiskTexture.Name, Texture.GetFirstMipMap().Size.X, Texture.GetFirstMipMap().Size.Y
                )
                continue;
            }

            LoadedTextures.Add(std::move(Texture));

            continue;
        }

        this->BlendersAtlas.Make(LoadedTextures);
    }
#pragma endregion Blending

#pragma region Actual textures
    {
        TdhArray<LTexture2> LoadedTextures;
        TdhArray<LDiskVoxelTexture> TextureNames = TextureSubsystem->FindMeaningFullVoxelTextureNames();

        for (const LDiskVoxelTexture& DiskTexture : TextureNames)
        {
            LSimpleString VoxelName  = DiskTexture.GetVoxelName();
            const voxel_t VoxelIndex = VoxelSubsystem->GetPanickedVoxelIndex(VoxelName);
            LVoxelMask*   VoxelMask  = VoxelSubsystem->GetVoxelMask(VoxelIndex);
            checkSlow( VoxelMask->GetName() == DiskTexture.GetVoxelName() )

            VoxelMask->TextureGroups.Emplace(
                DiskTexture.GetNormalLookUpBasedOfFileName(),
                static_cast<LTextureIndex>(LoadedTextures.GetSize()),
                DiskTexture.GetBlendLookUpBasedOfFileName(BlendedTextureNames)
            );

            LEnginePath Path = LEnginePath(EEnginePaths::Voxels, DiskTexture.Name.ToPtr());
            Path.AddExtension(".png");

            LTexture2 Texture;
            if (Texture.LoadFromDisk(Path, *GetDefault<JUserPreferences>()) == false)
            {
                panicMsgf(
                    "Failed to load texture: {}. Faulty path: {}.",
                    DiskTexture.Name, Path.GetRelativeUnresolvedPath().GetPath()
                )
                continue;
            }

            if (Texture.GetFirstMipMap().Size.X != Texture.GetFirstMipMap().Size.Y)
            {
                panicMsgf(
                    "Texture is not square: {}. Size: {}x{}.",
                    DiskTexture.Name, Texture.GetFirstMipMap().Size.X, Texture.GetFirstMipMap().Size.Y
                )
                continue;
            }

            if (Maths::IsPowerOfTwo(Texture.GetFirstMipMap().Size.X) == false)
            {
                panicMsgf(
                    "Texture is not power of two: {}. Size: {}x{}.",
                    DiskTexture.Name, Texture.GetFirstMipMap().Size.X, Texture.GetFirstMipMap().Size.Y
                )
                continue;
            }

            LoadedTextures.Add(std::move(Texture));

            continue;
        }

        VoxelSubsystem->SortAllVoxelMasksTextureGroups();
        this->BlendOpaqueAtlas.Make(LoadedTextures);
    }
#pragma endregion Actual textures

    return;
}
