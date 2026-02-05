// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/StaticMesh.h"

namespace Jafg
{

struct LStaticMeshRef
{
    constexpr LStaticMeshRef() noexcept = default;
    explicit LStaticMeshRef(std::shared_ptr<LStaticMesh> InMesh) noexcept : Mesh{ std::move(InMesh) } {}
    ~LStaticMeshRef() noexcept = default;

    FORCEINLINE bool HasMesh() const noexcept { return this->Mesh.get() != nullptr; }
    FORCEINLINE CONSTEXPR_CHECK auto const& GetMesh() const noexcept { check( this->HasMesh() ) return *this->Mesh; }

    FORCEINLINE std::shared_ptr<LStaticMesh> GetNewHandle() const noexcept
    {
        check( this->HasMesh() )
        return this->Mesh;
    }

private:

    std::shared_ptr<LStaticMesh> Mesh;
};

} /* ~Namespace Jafg */
