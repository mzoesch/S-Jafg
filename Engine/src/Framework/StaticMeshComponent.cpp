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

void Jafg::AStaticMeshComponent::SetMesh(LPath const& Mesh, EStaticMeshState MeshState)
{
    this->Mesh = this->GetEngine().GetSubsystemChecked<JMeshSubsystem>()->FromFile(Mesh, MeshState);
    this->SetAabb(this->Mesh->GetAabb());
}

void Jafg::AStaticMeshComponent::SetMaterialInstance(LMaterialInstanceRef Instance) noexcept
{
    this->MaterialInstance = std::move(Instance);
    check(!this->MaterialInstance || !!this->MaterialInstance->Material.get())
}

namespace
{

NODISCARD Jafg::LMaterialInstance const& GetPreferredMaterialInstance(Jafg::LActorRenderInfo const& Info, Jafg::LMaterialInstance const* Owned) noexcept
{
    Jafg::LMaterialInstance const* InstancePtr{};
    if (Info.PreferredMaterial)
    {
        InstancePtr = &*Info.PreferredMaterial;
    }
    else if (Info.UserPreferences.EditorMeshMaterialPreference)
    {
        InstancePtr = &**Info.UserPreferences.EditorMeshMaterialPreference;
    }
    else if (Owned)
    {
        InstancePtr = Owned;
    }
    else
    {
        LOG_FATAL(LogRhi, " No material given.")
    }
    check(InstancePtr)

    return *InstancePtr;
}

} /* ~Namespace <Anonymous> */

void Jafg::AStaticMeshComponent::Render(LActorRenderInfo const& Info) const
{
    checkCode
    (
        if (!this->Mesh.get())
        {
            LOG_FATAL(LogRhi, "No mesh set for this static mesh component. Failed to render.")
        }
    )
    this->Mesh->Render(Info, this->GetTransform(), GetPreferredMaterialInstance(Info, this->MaterialInstance.get()));
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
    this->Mesh->Render(Info, this->GetTransform(), GetPreferredMaterialInstance(Info, this->MaterialInstance.get()));
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

    this->Mesh->Render(Info, this->GetTransform(), *this->MaterialInstance);
}

void Jafg::LStaticMeshRenderable::SetMesh(LPath const& Mesh, EStaticMeshState MeshState)
{
    check(GEngine)
    this->Mesh = GEngine->GetSubsystemChecked<JMeshSubsystem>()->FromFile(Mesh, MeshState);
}
