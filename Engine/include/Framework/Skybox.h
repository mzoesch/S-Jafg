// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/WorldObject.h"
#include "Rhi/TextureCube.h"
#include "Rhi/Material.h"
#include "Skybox.generated.h"

namespace Jafg
{

struct LActorRenderInfo;

DECLARE_JAFG_CLASS()
class ENGINE_API ASkybox : public AWorldObject
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(ASkybox)

public:

    virtual void BeginLife() override;

    void RenderSkybox(LActorRenderInfo const& Info);
    //# @return The replaced texture.
    std::shared_ptr<LTextureCube2> SetSkyboxTexture(std::shared_ptr<LTextureCube2> Texture);
    NODISCARD constexpr std::shared_ptr<LTextureCube2> const& GetSkyboxTexture() const noexcept { return this->Texture; }

private:

    rhi::device_buffer Vertices;
    rhi::device_buffer Indices;
    std::shared_ptr<LTextureCube2> Texture;
    LMaterialInstanceRef MaterialInstance;
};

} /* ~Namespace Jafg */
