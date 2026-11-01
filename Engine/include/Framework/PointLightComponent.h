// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/SceneComponent.h"
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

#if JAFG_WITH_EDITOR
    virtual void OnAttach(AActor& Owner) override;
#endif /* JAFG_WITH_EDITOR */
};

} /* ~Namespace Jafg */
