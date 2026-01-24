// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/StaticMeshRef.h"
#include "EnginePath.h"
#include "Engine/CxxClass.h"
#include "MeshSubsystem.generated.h"

namespace Jafg
{

//#
//# A mesh that was loaded by the program and may be used across many different widgets.
//# @remark This is not intended for textures that are very specific.
//#
DECLARE_JAFG_CLASS(ECxxClassFlags::Singleton)
class JMeshSubsystem : public JCxxClass
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JMeshSubsystem)

public:

    //# Removes all loaded models that are not referenced anymore.
    void PurgeUnused() noexcept;

    LStaticMeshRef GetMesh(LString const& Ident) const
    {
        return LStaticMeshRef{this->GetMesh(nullptr, Ident, {}, {})};
    }
    LStaticMeshRef GetMesh(
          LPath const& Path
        , LStaticMesh::ELoadBehavior LoadBehavior = LStaticMesh::ELoadBehavior::LoadToDevice
        , LStaticMesh::EUploadHostMemoryBehavior HostMemoryBehavior = LStaticMesh::EUploadHostMemoryBehavior::Free
        ) const
    {
        return LStaticMeshRef{this->GetMesh(&Path, Path.generic_string(), LoadBehavior, HostMemoryBehavior)};
    }
    LStaticMeshRef GetMesh(
          LEnginePath const& Path,
          LStaticMesh::ELoadBehavior LoadBehavior = LStaticMesh::ELoadBehavior::LoadToDevice
        , LStaticMesh::EUploadHostMemoryBehavior HostMemoryBehavior = LStaticMesh::EUploadHostMemoryBehavior::Free
        ) const
    {
        return this->GetMesh(Path.ResolvePath(), LoadBehavior, HostMemoryBehavior);
    }

    void AddMesh(LStaticMeshRef const& Mesh, LString const& Ident) const noexcept
    {
        check( Mesh.HasMesh() )
        check( this->Meshes.contains(Ident) == false )
        this->Meshes.emplace(Ident, Mesh.GetNewHandle());
        return;
    }

    FORCEINLINE auto GetLoadedMeshCount() const noexcept { return this->Meshes.size(); }
    FORCEINLINE auto const& GetMeshes() const noexcept { return this->Meshes; }

private:

    std::shared_ptr<LStaticMesh> GetMesh(
          LPath const* Path
        , LString const& Ident
        , LStaticMesh::ELoadBehavior LoadBehavior
        , LStaticMesh::EUploadHostMemoryBehavior HostMemoryBehavior
        ) const;

    mutable std::unordered_map<LString, std::shared_ptr<LStaticMesh>> Meshes;
};

} /* ~Namespace Jafg */
