// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/CxxClass.h"
#include "Rhi/StaticMesh.h"
#include "MeshSubsystem.generated.h"

namespace Jafg
{

//# A mesh that was loaded by the program and may be used across many different objects.
DECLARE_JAFG_CLASS(ECxxClassFlags::Singleton)
class JMeshSubsystem final : public JCxxClass
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JMeshSubsystem)

public:

    //# Removes all loaded meshes that are not referenced anymore.
    void PurgeUnused() noexcept;

    FORCEINLINE constexpr auto GetLoadedMeshCount() const noexcept { return this->Meshes.size(); }
    FORCEINLINE constexpr auto const& GetMeshes() const noexcept { return this->Meshes; }

    //# Allocate the requested resource from a file.
    ENGINE_API LStaticMeshRef_v2 FromFile(LPath const& Path, EStaticMeshState State = EStaticMeshStateBits::Device) const;

private:

    mutable std::unordered_map<LPath, std::shared_ptr<LStaticMesh>> Meshes;
};

} /* ~Namespace Jafg */
