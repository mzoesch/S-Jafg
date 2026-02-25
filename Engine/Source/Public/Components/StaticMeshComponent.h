// Copyright mzoesch. All rights reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "Rhi/Texture2.h"
#include "Rhi/StaticMesh.h"
#include "Rhi/Material.h"
#include "StaticMeshComponent.generated.h"

namespace Jafg
{

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

    void SetMesh(LPath const& Mesh, EStaticMeshState MeshState = EStaticMeshStateBits::Device);
    void SetMaterialInstance(LMaterialInstanceRef MaterialInstance) noexcept;

    virtual void Render(LRenderInfo const& Info) noexcept override;

private:

    LMaterialInstanceRef MaterialInstance;
    LTexture2Ref Texture;
    LStaticMeshRef Mesh;
};

} /* ~Namespace Jafg */
