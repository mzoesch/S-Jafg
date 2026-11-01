// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/SceneComponent.h"
#include "Rhi/Billboard.h"
#include "BillboardComponent.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API ABillboardComponent : public ASceneComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(ABillboardComponent)
    {
        this->SetShouldRender(true);
        this->Billboard.SetSize(LVec2F{0.3f});
    }

public:

    virtual void Render(LActorRenderInfo const& Info) const override;

    FORCEINLINE void SetBillboard(LBillboard const& Billboard) noexcept
    {
        this->Billboard = Billboard;
    }
    NODISCARD FORCEINLINE constexpr LBillboard const& GetBillboard() const noexcept
    {
        return this->Billboard;
    }

    FORCEINLINE void SetTexture(LTexture2Ref Texture) noexcept
    {
        this->Billboard.SetTexture(std::move(Texture));
    }
    NODISCARD FORCEINLINE constexpr LTexture2Ref const& GetTexture() const noexcept
    {
        return this->Billboard.GetTexture();
    }

private:

    LBillboard Billboard;
};

} /* ~Namespace Jafg */
