// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/SceneComponent.h"
#include "Rhi/Light.h"
#include "PointLightComponent.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API APointLightComponent : public ASceneComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(APointLightComponent)
    {
#if JAFG_WITH_EDITOR
        this->SetShouldRender(true);
#endif /* JAFG_WITH_EDITOR */
    }

public:

    virtual void OnAttach(AActor& Owner) override;
    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

    ENGINE_API bool IsRegistered() const noexcept;

protected:

    virtual void OnTransformChanged(ETransformChangeFlags Change) override
    {
        auto Transform{this->GetWorldTransformSlow()};
        this->Light.Translation = Transform.t;
        this->Light.Rotation = Transform.r;
    }

private:

    LLight Light;
};

} /* ~Namespace Jafg */
