// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/ApplicationInstanceSubsystem.h"
#include "MyWorld/VoxelMask.h"
#include "VoxelSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class JVoxelSubsystem final : public JApplicationInstanceSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JVoxelSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override;

public:

    voxel_t GetVoxelIndex(const LSimpleString& Name) const;
    voxel_t GetVoxelIndex(const LSimpleString& Namespace, const LSimpleString& Name) const;
    FORCEINLINE voxel_t GetCheckedVoxelIndex(const LSimpleString& Name) const;
    FORCEINLINE voxel_t GetCheckedVoxelIndex(const LSimpleString& Namespace, const LSimpleString& Name) const;
    FORCEINLINE voxel_t GetPanickedVoxelIndex(const LSimpleString& Name) const;
    FORCEINLINE voxel_t GetPanickedVoxelIndex(const LSimpleString& Namespace, const LSimpleString& Name) const;

    FORCEINLINE       LVoxelMask* GetVoxelMask(const voxel_t Index)       { return &this->VoxelMasks[Index]; }
    FORCEINLINE const LVoxelMask& GetVoxelMask(const voxel_t Index) const { return  this->VoxelMasks[Index]; }

private:

    void InitializeCompileTimeVoxels();
    int32 CompileTimeVoxels = INDEX_NONE;

    void InitializeOptionalVoxels();

    TdhArray<LVoxelMask> VoxelMasks;
};

voxel_t JVoxelSubsystem::GetCheckedVoxelIndex(const LSimpleString& Name) const
{
    const voxel_t Out = this->GetVoxelIndex(Name);
    check( Out != ECompileTimeVoxels::Null )
    return Out;
}

voxel_t JVoxelSubsystem::GetCheckedVoxelIndex(const LSimpleString& Namespace, const LSimpleString& Name) const
{
    const voxel_t Out = this->GetVoxelIndex(Namespace, Name);
    check( Out != ECompileTimeVoxels::Null )
    return Out;
}

voxel_t JVoxelSubsystem::GetPanickedVoxelIndex(const LSimpleString& Name) const
{
    const voxel_t Out = this->GetVoxelIndex(Name);

    if (Out == ECompileTimeVoxels::Null)
    {
        panicMsgf( "Voxel with name '{}' was not found.", Name )
    }

    return Out;
}

voxel_t JVoxelSubsystem::GetPanickedVoxelIndex(const LSimpleString& Namespace, const LSimpleString& Name) const
{
    const voxel_t Out = this->GetVoxelIndex(Namespace, Name);

    if (Out == ECompileTimeVoxels::Null)
    {
        panicMsgf( "Voxel with name '{}' in namespace '{}' was not found.", Name, Namespace )
    }

    return Out;
}

} /* ~Namespace Jafg */
