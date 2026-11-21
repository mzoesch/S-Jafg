// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/SceneComponent.h"
#include "Rhi/Texture2.h"
#include "Rhi/StaticMesh.h"
#include "Rhi/Material.h"
#include "Rhi/PhysicalRendering.h"
#include "StaticMeshComponent.generated.h"

namespace Jafg
{

class JShaderSubsystem;

DECLARE_JAFG_CLASS()
class ENGINE_API AStaticMeshComponent : public ASceneComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(AStaticMeshComponent)
    {
        this->SetShouldRender(true);
    }

public:

    virtual void OnAttach(AActor& InOwner) override;

    //# The only mesh of the file. @see #JMeshSubsystem::LoadMesh
    void SetMesh(LPluginInfo Namespace, LStringView Mesh, EStaticMeshState MeshState = EStaticMeshStateBits::Device, std::optional<LStaticMeshCreateInfo> Info = {});
    //# Use an already allocated mesh (e.g., of a #LMeshScene). Its state is not changed.
    void SetMesh(LStaticMeshRef InMesh) noexcept;
    NODISCARD FORCEINLINE constexpr bool IsMeshValid() const noexcept { return !!this->Mesh; }
    NODISCARD FORCEINLINE constexpr LStaticMeshRef GetMeshRef() const noexcept { return this->Mesh; }
    NODISCARD FORCEINLINE constexpr LStaticMesh& GetMutableMesh() noexcept { check(this->IsMeshValid()) return *this->Mesh; }
    NODISCARD FORCEINLINE constexpr LStaticMesh const& GetMesh() const noexcept { check(this->IsMeshValid()) return *this->Mesh; }
    void SetMaterialInstance(LMaterialInstanceRef Instance) noexcept;
    void SetPbrMaterialInstance(LPbrMaterialInstanceRef Instance) noexcept;

    //# Renders the mesh and then the child components.
    virtual void Render(LActorRenderInfo const& Info) const override;

    //#
    //# Traces the triangles of the mesh if it has trace geometry (editor builds, @see #LStaticMesh::HasTraceGeometry).
    //# Else only the aabb.
    //#
    NODISCARD virtual std::optional<LWorldReal> TraceNonPhysical(LWorldMagRay3 const& Ray) const override;

private:

    CLASS_FIELD(Identity)
    LMaterialInstanceRef MaterialInstance;
    CLASS_FIELD(Identity)
    LPbrMaterialInstanceRef PbrMaterialInstance;
    CLASS_FIELD(EditorVisible)
    LStaticMeshRef Mesh;

    JShaderSubsystem* ShaderSubsystem{};
};

} /* ~Namespace Jafg */
