// Copyright mzoesch. All rights reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "Rhi/StaticMeshRef.h"
#include "Rhi/Image.h"
#include "StaticMeshComponent.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API JStaticMeshComponent : public JSceneComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JStaticMeshComponent)

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

     decltype(auto) Create(this auto&& Self, LCreateInfo const& Info)
     {
         Self.CreateImpl(Info);
         return std::forward<decltype(Self)>(Self);
     }

    virtual void Render(LRenderInfo const& Info) noexcept override;

private:

    void CreateImpl(LCreateInfo const& Info);

    LImage Image;
    LStaticMeshRef Mesh;
};

} /* ~Namespace Jafg */
