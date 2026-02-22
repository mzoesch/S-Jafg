// Copyright mzoesch. All rights reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "Rhi/Image.h"
#include "Rhi/StaticMesh.h"
#include "StaticMeshComponent.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API AStaticMeshComponent : public ASceneComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(AStaticMeshComponent)

public:

    struct CreateInfo final
    {
        //# Whether to enable rendering for this component in future draw calls.
        bool bRender{ true };

        //# Required path to the mesh that should be rendered by this component.
        LPath MeshPath;
        //# The state that the mesh will have when loaded.
        EStaticMeshState MeshState{ EStaticMeshStateBits::Device };

        //# The path to the texture that should be rendered by this component.
        LPath TexturePath;
        LTexture2::LMetadata TextureMetadata{ .Format = vk::Format::eR8G8B8A8Srgb };
        ETextureLoadFlags TextureLoadFlags{ ETextureLoadFlagBits::Load | ETextureLoadFlagBits::Stage };
    };

    void Create(CreateInfo const& Info);

    virtual void Render(LRenderInfo const& Info) noexcept override;

private:

    LImage Image;
    LStaticMeshRef_v2 Mesh;
};

} /* ~Namespace Jafg */
