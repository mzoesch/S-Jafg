// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "System/VoxelSubsystem.h"

void Jafg::JVoxelSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    this->InitializeCompileTimeVoxels();
    jassert( this->CompileTimeVoxels == 2 )

    this->InitializeOptionalVoxels();

    LOG_INFO(LogVoxelSubsystem, "Voxel Subsystem initialized with [{}/{}] voxels.", this->CompileTimeVoxels, this->VoxelMasks.GetSize())

    return;
}

void Jafg::JVoxelSubsystem::TearDown()
{
    Super::TearDown();
}

Jafg::voxel_t Jafg::JVoxelSubsystem::GetVoxelIndex(const LString& Name) const
{
    for (TArray<LVoxelMask>::SizeType Idx { 0 }; Idx < this->VoxelMasks.GetSize(); ++Idx)
    {
        if (this->VoxelMasks[Idx].GetName() == Name)
        {
            return static_cast<voxel_t>(Idx);
        }

        continue;
    }

    return ECompileTimeVoxels::Null;
}

Jafg::voxel_t Jafg::JVoxelSubsystem::GetVoxelIndex(const LString& Namespace, const LString& Name) const
{
    for (TArray<LVoxelMask>::SizeType Idx { 0 }; Idx < this->VoxelMasks.GetSize(); ++Idx)
    {
        if (this->VoxelMasks[Idx].GetNamespace() == Namespace && this->VoxelMasks[Idx].GetName() == Name)
        {
            return static_cast<voxel_t>(Idx);
        }

        continue;
    }

    return ECompileTimeVoxels::Null;
}

void Jafg::JVoxelSubsystem::SortAllVoxelMasksTextureGroups()
{
    for (LVoxelMask& Mask : this->VoxelMasks)
    {
        for (TArray<LVoxelMask::LTextureGroup>::SizeType Idx { 0 }; Idx < Mask.TextureGroups.GetSize(); ++Idx)
        {
            if (Mask.TextureGroups[Idx].Normal != ENormalLookup::Omnia)
            {
                continue;
            }

            if (Idx == Mask.TextureGroups.GetSize() - 1)
            {
                /* Already at the end. Nothing to do. */
                continue;
            }

            Mask.TextureGroups.SwapIndices(Idx, Mask.TextureGroups.GetSize() - 1);

            continue;
        }

        continue;
    }

    return;
}

void Jafg::JVoxelSubsystem::InitializeCompileTimeVoxels()
{
    this->VoxelMasks.Add(LVoxelMask::Null);
    this->VoxelMasks.Add(LVoxelMask::Air);

    this->CompileTimeVoxels = this->VoxelMasks.GetSize();

    return;
}

void Jafg::JVoxelSubsystem::InitializeOptionalVoxels()
{
    LVoxelMask DebugVoxel1("Jafg", "Debug1");
    LVoxelMask DebugVoxel2("Jafg", "Debug2");
    LVoxelMask DirtVoxel("Jafg", "Dirt");
    LVoxelMask GrassVoxel("Jafg", "Grass");
    LVoxelMask StoneVoxel("Jafg", "Stone");

    this->VoxelMasks.Add(std::move(DebugVoxel1));
    this->VoxelMasks.Add(std::move(DebugVoxel2));
    this->VoxelMasks.Add(std::move(DirtVoxel));
    this->VoxelMasks.Add(std::move(GrassVoxel));
    this->VoxelMasks.Add(std::move(StoneVoxel));

    return;
}
