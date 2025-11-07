// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/EngineSubsystem.h"
#include "MyWorld/VoxelMask.h"
#include "VoxelSubsystem.generated.h"

namespace Jafg
{

class JMaterialSubsystem;

DECLARE_JAFG_CLASS()
class ENGINE_API JVoxelSubsystem final : public JEngineSubsystem
{
    GENERATED_CLASS_BODY()

    friend JMaterialSubsystem;

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JVoxelSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;

public:

    ENGINE_API voxel_t GetVoxelIndex(const LString& Name) const;
    ENGINE_API voxel_t GetVoxelIndex(const LString& Namespace, const LString& Name) const;
    FORCEINLINE voxel_t GetCheckedVoxelIndex(const LString& Name) const;
    FORCEINLINE voxel_t GetCheckedVoxelIndex(const LString& Namespace, const LString& Name) const;
    FORCEINLINE voxel_t GetPanickedVoxelIndex(const LString& Name) const;
    FORCEINLINE voxel_t GetPanickedVoxelIndex(const LString& Namespace, const LString& Name) const;

    FORCEINLINE       LVoxelMask* GetVoxelMask(const voxel_t Index)       { return &this->VoxelMasks[Index]; }
    FORCEINLINE const LVoxelMask& GetVoxelMask(const voxel_t Index) const { return  this->VoxelMasks[Index]; }

private:

    //#
    //# Ensure that the Omnia texture group is at the end of the texture group array to allow for bitwise and
    //# operations to work correctly.
    //# @see MyWorld/Meshing/*Mesher.cpp
    //#
    void SortAllVoxelMasksTextureGroups();

    void InitializeCompileTimeVoxels();
    i32 CompileTimeVoxels = INDEX_NONE;

    void InitializeOptionalVoxels();

    TArray<LVoxelMask> VoxelMasks;
};

FORCEINLINE voxel_t JVoxelSubsystem::GetCheckedVoxelIndex(const LString& Name) const
{
    const voxel_t Out = this->GetVoxelIndex(Name);
    check( Out != ECompileTimeVoxels::Null )
    return Out;
}

FORCEINLINE voxel_t JVoxelSubsystem::GetCheckedVoxelIndex(const LString& Namespace, const LString& Name) const
{
    const voxel_t Out = this->GetVoxelIndex(Namespace, Name);
    check( Out != ECompileTimeVoxels::Null )
    return Out;
}

FORCEINLINE voxel_t JVoxelSubsystem::GetPanickedVoxelIndex(const LString& Name) const
{
    const voxel_t Out = this->GetVoxelIndex(Name);

    if (Out == ECompileTimeVoxels::Null)
    {
        panicMsgf( "Voxel with name '{}' was not found.", Name )
    }

    return Out;
}

FORCEINLINE voxel_t JVoxelSubsystem::GetPanickedVoxelIndex(const LString& Namespace, const LString& Name) const
{
    const voxel_t Out = this->GetVoxelIndex(Namespace, Name);

    if (Out == ECompileTimeVoxels::Null)
    {
        panicMsgf( "Voxel with name '{}' in namespace '{}' was not found.", Name, Namespace )
    }

    return Out;
}

} /* ~Namespace Jafg */
