// Copyright mzoesch. All rights reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "Rhi/StaticMesh.h"
#include "Rhi/Image.h"

namespace Jafg
{

class ENGINE_API LStaticMeshComponent : public LSceneComponent
{
public:

    struct LCreateInfo
    {
        bool bRender{ true };

        LPath MeshPath;
        LStaticMesh::ELoadBehavior MeshLoadBehavior{ LStaticMesh::ELoadBehavior::LoadToDevice };
        LStaticMesh::EUploadHostMemoryBehavior MeshHostMemoryBehavior{ LStaticMesh::EUploadHostMemoryBehavior::Free };

        LPath TexturePath;
        LTexture2::LMetadata TextureMetadata{ .Format = vk::Format::eR8G8B8A8Srgb };
        ETextureLoadFlags TextureLoadFlags{ ETextureLoadFlagBits::Load | ETextureLoadFlagBits::Stage };
    };

    explicit LStaticMeshComponent(LCreateInfo const& Info);

    virtual void Render(LRenderInfo const& Info, LEye const& Eye) noexcept override;

private:

    LImage Image;
    LStaticMesh Mesh;
};

} /* ~Namespace Jafg */
