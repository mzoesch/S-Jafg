// Copyright mzoesch. All rights reserved.

#include "Framework/StaticMeshComponent.h"
#include "User/UserPreferences.h"
#include "Framework/ShaderSubsystem.h"
#include "Framework/MeshSubsystem.h"
#include "Framework/Frontend.h"
#include "Engine/Engine.h"
#include "Rhi/PhysicalRendering.h"
#include "Framework/Actor.h"
#include "Rhi/StaticMeshRenderable.h"

void Jafg::AStaticMeshComponent::OnAttach(AActor& InOwner)
{
    Super::OnAttach(InOwner);
    this->ShaderSubsystem = this->GetMutableLocalEgo().GetFrontend().GetSubsystemChecked<JShaderSubsystem>();
}

void Jafg::AStaticMeshComponent::SetMesh(LPluginInfo Namespace, LStringView Mesh, EStaticMeshState MeshState /* = EStaticMeshStateBits::Device */, std::optional<LStaticMeshCreateInfo> Info /* = {} */)
{
    this->Mesh = this->GetEngine().GetSubsystemChecked<JMeshSubsystem>()->LoadMesh(Namespace, Mesh, MeshState, std::move(Info));
    this->Aabb = this->Mesh->GetAabb();
}

void Jafg::AStaticMeshComponent::SetMesh(LStaticMeshRef InMesh) noexcept
{
    this->Mesh = std::move(InMesh);
    this->Aabb = this->Mesh ? this->Mesh->GetAabb() : maths::identity<LWorldAabb3>;
}

void Jafg::AStaticMeshComponent::SetMaterialInstance(LMaterialInstanceRef Instance) noexcept
{
    this->MaterialInstance = std::move(Instance);
    check(!this->MaterialInstance || !!this->MaterialInstance->Material.get())
    this->PbrMaterialInstance = nullptr;
}

void Jafg::AStaticMeshComponent::SetPbrMaterialInstance(LPbrMaterialInstanceRef Instance) noexcept
{
    this->PbrMaterialInstance = std::move(Instance);
    check(!this->PbrMaterialInstance || !!this->PbrMaterialInstance->Instance.get())
    if (this->PbrMaterialInstance)
    {
        this->MaterialInstance = this->PbrMaterialInstance->Instance;
        check(!this->MaterialInstance || !!this->MaterialInstance->Material.get())
    }
}

void Jafg::AStaticMeshComponent::Render(LActorRenderInfo const& Info) const
{
    checkCode
    (
        if (!this->Mesh.get())
        {
            LOG_FATAL(LogRhi, "No mesh set for this static mesh component. Failed to render.")
        }
    )

    //# Todo: Instead of this. We want to pass the parent transform to the children. This way we avoid the recursive calls up the chain here -- to recalculate many times the same thing.
    //#       E.g. Render(LActorRenderInfo const&, LWorldTrans* World); Then check
    //#             if (World) World apply with local trans else use local trans.
    auto& Preferred{GetPreferredMaterialInstance(Info, this->MaterialInstance.get())};
    LWorldTrans const Transform{this->GetWorldTransformSlow()};
    if (this->PbrMaterialInstance.get() && this->MaterialInstance.get() == &Preferred)
    {
        check(this->PbrMaterialInstance->Instance.get() == this->MaterialInstance.get())
        DrawWithPbrMaterial(Info, this->PbrMaterialInstance, GetWorldCenter(this->Mesh->GetAabb(), Transform)
            , [Mesh=this->Mesh, Transform, Pbr=this->PbrMaterialInstance.get()](LActorRenderInfo const& Info, LMaterialInstance const& Instance)
            {
                Mesh->Render(Info, Transform, Instance, Pbr->GetBindlessIndex());
            });
    }
    else
    {
        /* The preferred material (e.g., of the shadows) might still want to know the pbr material. */
        this->Mesh->Render(Info, Transform, Preferred
            , this->PbrMaterialInstance ? this->PbrMaterialInstance->GetBindlessIndex() : rhi::invalid_resource_index);
    }

    /* E.g., meshes of a mesh scene that are attached to this mesh. */
    Super::Render(Info);
}

std::optional<LWorldReal> Jafg::AStaticMeshComponent::TraceNonPhysical(LWorldMagRay3 const& Ray) const
{
    /* Cheap rejection first. */
    auto const AabbHit{Super::TraceNonPhysical(Ray)};
    if (!AabbHit || !this->Mesh || !this->Mesh->HasTraceGeometry())
    {
        return AabbHit;
    }

    LWorldTrans const World{this->GetWorldTransformSlow()};
    LWorldVec3 const Scale{World.s};
    if (Scale.x == 0 || Scale.y == 0 || Scale.z == 0)
    {
        return {};
    }

    /*
     * Into mesh space. The direction is deliberately not normalized there: The parameter along the ray stays the
     * same in both spaces.
     */
    LWorldQuat const InverseRotation{maths::inverse(World.r)};
    LVec3F const Origin{(InverseRotation * (Ray.origin - World.t)) / Scale};
    LVec3F const Direction{(InverseRotation * Ray.direction) / Scale};

    if (auto const T{this->Mesh->TraceTriangles(Origin, Direction, static_cast<f32>(Ray.magnitude))})
    {
        return static_cast<LWorldReal>(*T);
    }
    return {};
}

void Jafg::LStaticMeshRenderable::Render(LActorRenderInfo const& Info) const
{
    checkCode
    (
        if (!this->Mesh.get())
        {
            LOG_FATAL(LogRhi, "No mesh set for this static mesh component. Failed to render.")
        }
    )
    this->Mesh->Render(Info, this->GetTransform(), &GetPreferredMaterialInstance(Info, this->MaterialInstance.get()));
}

void Jafg::LStaticMeshRenderable::ForceRenderWithOwnedMaterial(LActorRenderInfo const& Info) const
{
    checkCode
    (
        if (!this->Mesh.get())
        {
            LOG_FATAL(LogRhi, "No mesh set for this static mesh component. Failed to render.")
        }
    )

    check(this->MaterialInstance.get())

    this->Mesh->Render(Info, this->GetTransform(), &*this->MaterialInstance);
}

void Jafg::LStaticMeshRenderable::SetMesh(LPluginInfo Namespace, LStringView Mesh, EStaticMeshState MeshState)
{
    check(GEngine)
    this->Mesh = GEngine->GetSubsystemChecked<JMeshSubsystem>()->LoadMesh(Namespace, Mesh, MeshState);
}
