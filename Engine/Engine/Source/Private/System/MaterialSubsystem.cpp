// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "System/MaterialSubsystem.h"
#include "Engine/Framework/ApplicationInstance.h"
#include "System/Finder.h"
#include "System/TextureSubsystem.h"
#include "System/VoxelSubsystem.h"
#include "System/EnginePath.h"
#include "User/UserPreferences.h"
#include "Rhi/RendererInformation.h"

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
    this->CurrentTextureWidth = 0;
    this->Atlas.Free();

    return;
}

void Jafg::JMaterialSubsystem::LoadAllTextures()
{
    TdhArray<LTexture2> LoadedTextures;

    JVoxelSubsystem* VoxelSubsystem = this->GetApplicationInstance()->GetCheckedSubsystem<JVoxelSubsystem>();
    const JTextureSubsystem* TextureSubsystem = this->GetApplicationInstance()->GetCheckedSubsystem<JTextureSubsystem>();

    TdhArray<LDiskVoxelTexture> TextureNames = TextureSubsystem->FindMeaningFullVoxelTextureNames();

    for (const LDiskVoxelTexture& DiskTexture : TextureNames)
    {
        LSimpleString VoxelName  = DiskTexture.GetVoxelName();
        const voxel_t VoxelIndex = VoxelSubsystem->GetPanickedVoxelIndex(VoxelName);
        LVoxelMask*   VoxelMask  = VoxelSubsystem->GetVoxelMask(VoxelIndex);
        checkSlow( VoxelMask->GetName() == DiskTexture.GetVoxelName() )

        VoxelMask->TextureGroups.Emplace(
            DiskTexture.GetNormalLookUpBasedOfFileName(),
            static_cast<LTextureIndex>(LoadedTextures.GetSize())
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

    this->CreateAtlas(LoadedTextures);

    return;
}

void Jafg::JMaterialSubsystem::CreateAtlas(const TdhArray<LTexture2>& Textures)
{
    const int32 TextureSizeLimit = RendererInformation::GetLimitTextureDimension();

    int32 MaxTextureSize = INDEX_NONE;
    for (const LTexture2& Texture : Textures)
    {
        MaxTextureSize = Maths::Max(MaxTextureSize, static_cast<int32>(Texture.GetFirstMipMap().Size.X));
    }
    if (MaxTextureSize > TextureSizeLimit)
    {
        panicMsgf(
            "Texture size limit exceeded. Texture size limit: {}. Largest texture size: {}.",
            TextureSizeLimit, MaxTextureSize
        )
        return;
    }
    this->CurrentTextureWidth = MaxTextureSize;

    const int32 AtlasTextureDimensionCount = static_cast<int32>(Maths::Ceil<float>(Maths::Sqrt(static_cast<float>(Textures.GetSize()))));
    jassert(AtlasTextureDimensionCount > 0)

    this->Atlas.CreateEmpty(
        static_cast<uint32>(this->CurrentTextureWidth * AtlasTextureDimensionCount),
        static_cast<uint32>(this->CurrentTextureWidth * AtlasTextureDimensionCount),
        ERawImageFormat::BGRA8
    );

    for (LTextureIndex TextureIndex = 0; TextureIndex < static_cast<LTextureIndex>(Textures.GetSize()); ++TextureIndex)
    {
        const LTexture2& Texture = Textures[static_cast<int32>(TextureIndex)];
        const LPoint TexturePoint = this->CalculateSpecificTexturePointOnAtlas(TextureIndex);

        this->Atlas.CopyTexture(Texture, TexturePoint, true);

        continue;
    }

    return;
}
