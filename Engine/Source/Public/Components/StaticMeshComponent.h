// Copyright mzoesch. All rights reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "Rhi/Texture2.h"
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
        LTexture2::HostInfo TextureHostCreateInfo{ .Format = vk::Format::eR8G8B8A8Srgb };
        LTexture2::DeviceInfo TextureDeviceCreateInfo{};
        ETexture2State TextureState{ ETexture2StateBits::Device };
    };

    void Create(CreateInfo const& Info);

    virtual void Render(LRenderInfo const& Info) noexcept override;

private:

    LTexture2Ref Texture;
    LStaticMeshRef Mesh;
};

} /* ~Namespace Jafg */
