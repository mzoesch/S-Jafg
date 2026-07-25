// Copyright mzoesch. All rights reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "Rhi/Texture2.h"
#include "Rhi/StaticMesh.h"
#include "Rhi/Material.h"
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

    void SetMesh(LPath const& Mesh, EStaticMeshState MeshState = EStaticMeshStateBits::Device);
    void SetMaterialInstance(LMaterialInstanceRef Instance) noexcept;

    virtual void Render(LActorRenderInfo const& Info) const override;

private:

    LMaterialInstanceRef MaterialInstance;
    CLASS_FIELD(EditorVisible)
    LStaticMeshRef Mesh;

    JShaderSubsystem* ShaderSubsystem{};
};

} /* ~Namespace Jafg */
