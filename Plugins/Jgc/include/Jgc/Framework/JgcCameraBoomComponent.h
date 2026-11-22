// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/SceneComponent.h"
#include "JgcCameraBoomComponent.generated.h"

namespace Jgc
{

//#
//# Holds the eyes below it at the end of an arm that points backwards from it, e.g., a third person view behind the
//# head of a character. Where the world is in the way, the arm is pulled in at once; afterward it extends smoothly.
//#
//# Only traces while one of its eyes is viewed.
//#
DECLARE_JAFG_CLASS()
class JGC_API AJgcCameraBoomComponent : public Jafg::ASceneComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(AJgcCameraBoomComponent)
    {
        this->bTick = true;
    }

public:

    virtual void ParentTick(f32 Dt) override;

    //# In meters.
    CLASS_FIELD(EditorEditable)
    f32 ArmLength{ 2.8f };
    //# How far the eyes keep away from what is in the way. In meters.
    CLASS_FIELD(EditorEditable)
    f32 ProbeMargin{ 0.25f };
    //# How fast the arm extends again after it was pulled in. In 1/s.
    CLASS_FIELD(EditorEditable)
    f32 ExtendSpeed{ 6.0f };
    //#
    //# Where the end of the arm is moved to, in the space of this component, at full length. E.g., over the right
    //# shoulder (right is -X). Shrinks with the arm, so that it does not reach into walls.
    //#
    LVec3F SocketOffset{ -0.4f, 0.15f, 0.0f };

private:

    //# Negative until the first tick.
    f32 CurrentLength{ -1.0f };
};

} /* ~Namespace Jgc */
