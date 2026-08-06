// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/StaticMesh.h"
#include "Rhi/Material.h"

namespace Jafg
{

struct LStaticMeshRenderable
{
    explicit LStaticMeshRenderable() noexcept = default;

    void Render(LActorRenderInfo const& Info) const;
    void ForceRenderWithOwnedMaterial(LActorRenderInfo const& Info) const;

    ENGINE_API void SetMesh(LPath const& Mesh, EStaticMeshState MeshState = EStaticMeshStateBits::Device);
    void SetMesh(LStaticMeshRef Mesh) noexcept
    {
        this->Mesh = std::move(Mesh);
    }
    NODISCARD FORCEINLINE constexpr LStaticMeshRef const& GetMesh() const noexcept { return this->Mesh; }

    void SetMaterialInstance(LMaterialInstanceRef Instance) noexcept
    {
        this->MaterialInstance = std::move(Instance);
        check(!this->MaterialInstance || !!this->MaterialInstance->Material.get())
    }
    NODISCARD FORCEINLINE constexpr LMaterialInstanceRef const& GetMaterialInstance() const noexcept { return this->MaterialInstance; }

    void SetTransform(LWorldTrans const& Transform) noexcept { this->Trans = Transform; }

    void AddTranslation(LWorldVec3 const& Location) noexcept { this->Trans.t += Location; }
    void AddRotator(LWorldQuat const& Rotator) noexcept { this->Trans.r = Rotator * this->Trans.r; }
    void AddScale(LWorldVec3 const& Scale) noexcept { this->Trans.s += Scale; }

    void SetTranslation(LWorldVec3 const& Location) noexcept { this->Trans.t = Location; }
    void SetRotator(LWorldQuat const& Rotator) noexcept { this->Trans.r = Rotator; }
    void SetScale(LWorldVec3 const& Scale) noexcept { this->Trans.s = Scale; }

    NODISCARD FORCEINLINE constexpr LWorldTrans const& GetTransform() const noexcept { return this->Trans; }
    NODISCARD FORCEINLINE constexpr LWorldVec3 const& GetTranslation() const noexcept { return this->Trans.t; }
    NODISCARD FORCEINLINE constexpr LWorldQuat const& GetRotator() const noexcept { return this->Trans.r; }
    NODISCARD FORCEINLINE constexpr LWorldVec3 const& GetScale() const noexcept { return this->Trans.s; }

    NODISCARD FORCEINLINE constexpr LWorldAabb3 const& GetAabb() const noexcept
    {
        if (this->Mesh.get())
        {
            return this->Mesh->GetAabb();
        }
        static constexpr LWorldAabb3 Dummy{ maths::identity<LWorldAabb3> };
        return Dummy;
    }
    NODISCARD FORCEINLINE constexpr LWorldAabb3 GetTransformedAabb() const noexcept
    {
        if (this->Mesh.get())
        {
            return this->Mesh->GetAabb().apply(this->Trans);
        }
        return maths::identity<LWorldAabb3>;
    }

private:

    LStaticMeshRef Mesh;
    LMaterialInstanceRef MaterialInstance;
    LWorldTrans Trans{ maths::identity<LWorldTrans> };
};

} /* ~Namespace Jafg */
