// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/SceneComponent.h"
#include "EyeComponent.generated.h"

namespace Jafg
{

//#
//# A point of view in the world, e.g., the head of a character or a camera behind a ship. It looks along its
//# forward vector. A controller sees through the eye of the avatar that it possesses (see #AAvatarComponent::GetEye).
//#
//# Eyes are ordinary scene components: Attach them wherever the view should be. A third person camera, for example,
//# is an eye below the eye of the head, moved backwards.
//#
DECLARE_JAFG_CLASS()
class ENGINE_API AEyeComponent : public ASceneComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(AEyeComponent)

public:

    //# The perspective of this eye in world space.
    NODISCARD LWorldEye GetEye() const noexcept;

    //# Vertical field of view in radians.
    CLASS_FIELD(EditorEditable)
    f32 VertFov{ maths::pi_f / 3.0f };
    CLASS_FIELD(EditorEditable)
    f32 NearFrustum{ 0.1f };
    CLASS_FIELD(EditorEditable)
    f32 FarFrustum{ 1.0f };

    //# The first eye in the hierarchy below and including #Root. Depth first.
    NODISCARD static AEyeComponent* FindFirstEyeIn(ASceneComponent const& Root) noexcept;
    //# Calls #Fn for every eye in the hierarchy below and including #Root. Depth first.
    template<typename TFn> requires std::is_invocable_v<TFn, AEyeComponent&>
    static void ForEachEyeIn(ASceneComponent const& Root, TFn&& Fn)
    {
        if (auto* Eye{const_cast<ASceneComponent&>(Root).As<AEyeComponent>()})
        {
            Fn(*Eye);
        }
        for (auto const& Child: Root.GetChildren())
        {
            ForEachEyeIn(*Child, Fn);
        }
    }
};

} /* ~Namespace Jafg */
