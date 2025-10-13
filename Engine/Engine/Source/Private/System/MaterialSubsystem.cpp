// Copyright mzoesch. All rights reserved.

#include "Engine/Engine.h"
#include "System/MaterialSubsystem.h"
#include "System/VoxelTextureSubsystem.h"
#include "System/VoxelSubsystem.h"
#include "System/EnginePath.h"
#include "User/UserPreferences.h"

void Jafg::JMaterialSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Collection.InitializeDependency<JVoxelSubsystem>();
    Collection.InitializeDependency<JVoxelTextureSubsystem>();
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
    JVoxelSubsystem* VoxelSubsystem = this->GetEngine()->GetSubsystemChecked<JVoxelSubsystem>();
    JVoxelTextureSubsystem* TextureSubsystem = this->GetEngine()->GetSubsystemChecked<JVoxelTextureSubsystem>();

#pragma region "Blending"
    TArray<LDiskBlendTexture> BlendedTextureNames = TextureSubsystem->FindMeaningBlendTextureNames();
    {
        TArray<LTexture2> LoadedTextures;

        for (const LDiskBlendTexture& DiskTexture : BlendedTextureNames)
        {
            LEnginePath Path = LEnginePath(EEnginePaths::Blends, DiskTexture.Name);
            Path.concat(".png");

            LTexture2 Texture;
            if (Texture.LoadFromDisk(Path, ERawImageFormat::BGRA8) == false)
            {
                panicMsgf
                (
                    "Failed to load texture: {}. Faulty path: {}.",
                    DiskTexture.Name, Path
                )
                continue;
            }

            if (Texture.GetFirstMipMap().GetWidth() != Texture.GetFirstMipMap().GetHeight())
            {
                panicMsgf
                (
                    "Texture is not square: {}. Size: {}x{}.",
                    DiskTexture.Name, Texture.GetFirstMipMap().GetWidth(), Texture.GetFirstMipMap().GetHeight()
                )
                continue;
            }

            if (Maths::IsPowerOfTwo(Texture.GetFirstMipMap().GetWidth()) == false)
            {
                panicMsgf
                (
                    "Texture is not power of two: {}. Size: {}x{}.",
                    DiskTexture.Name, Texture.GetFirstMipMap().GetWidth(), Texture.GetFirstMipMap().GetHeight()
                )
                continue;
            }

            LoadedTextures.emplace_back(std::move(Texture));

            continue;
        }

        this->BlendersAtlas.Make(LoadedTextures);
    }
#pragma endregion "Blending"

#pragma region "Actual textures"
    {
        TArray<LTexture2> LoadedTextures;
        TArray<LDiskVoxelTexture> TextureNames = TextureSubsystem->FindMeaningFullVoxelTextureNames();

        for (const LDiskVoxelTexture& DiskTexture : TextureNames)
        {
            LString VoxelName  = DiskTexture.GetVoxelName();
            const voxel_t VoxelIndex = VoxelSubsystem->GetPanickedVoxelIndex(VoxelName);
            LVoxelMask*   VoxelMask  = VoxelSubsystem->GetVoxelMask(VoxelIndex);
            checkSlow( VoxelMask->GetName() == DiskTexture.GetVoxelName() )

            VoxelMask->TextureGroups.emplace_back(
                DiskTexture.GetNormalLookUpBasedOfFileName(),
                static_cast<LTextureIndex>(LoadedTextures.size()),
                DiskTexture.GetBlendLookUpBasedOfFileName(BlendedTextureNames)
                );

            LEnginePath Path = LEnginePath(EEnginePaths::Voxels, DiskTexture.Name);
            Path.concat(".png");

            LTexture2 Texture;
            if (Texture.LoadFromDisk(Path, ERawImageFormat::BGRA8) == false)
            {
                panicMsgf
                (
                    "Failed to load texture: {}. Faulty path: {}.",
                    DiskTexture.Name, Path
                )
                continue;
            }

            if (Texture.GetFirstMipMap().GetWidth() != Texture.GetFirstMipMap().GetHeight())
            {
                panicMsgf
                (
                    "Texture is not square: {}. Size: {}x{}.",
                    DiskTexture.Name, Texture.GetFirstMipMap().GetWidth(), Texture.GetFirstMipMap().GetHeight()
                )
                continue;
            }

            if (Maths::IsPowerOfTwo(Texture.GetFirstMipMap().GetWidth()) == false)
            {
                panicMsgf
                (
                    "Texture is not power of two: {}. Size: {}x{}.",
                    DiskTexture.Name, Texture.GetFirstMipMap().GetWidth(), Texture.GetFirstMipMap().GetHeight()
                )
                continue;
            }

            LoadedTextures.emplace_back(std::move(Texture));

            continue;
        }

        VoxelSubsystem->SortAllVoxelMasksTextureGroups();
        this->BlendOpaqueAtlas.Make(LoadedTextures);
    }
#pragma endregion "Actual textures"

    return;
}
