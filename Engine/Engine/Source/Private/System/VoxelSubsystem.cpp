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

Jafg::voxel_t Jafg::JVoxelSubsystem::GetVoxelIndex(const LSimpleString& Name) const
{
    for (voxel_t_it i = 0; i < this->VoxelMasks.GetSize(); ++i)
    {
        if (this->VoxelMasks[i].GetName() == Name)
        {
            return static_cast<voxel_t>(i);
        }

        continue;
    }

    return ECompileTimeVoxels::Null;
}

Jafg::voxel_t Jafg::JVoxelSubsystem::GetVoxelIndex(const LSimpleString& Namespace, const LSimpleString& Name) const
{
    for (voxel_t_it i = 0; i < this->VoxelMasks.GetSize(); ++i)
    {
        if (this->VoxelMasks[i].GetNamespace() == Namespace && this->VoxelMasks[i].GetName() == Name)
        {
            return static_cast<voxel_t>(i);
        }

        continue;
    }

    return ECompileTimeVoxels::Null;
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
