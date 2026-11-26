// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/CameraBoomComponent.h"
#include "Framework/EyeComponent.h"
#include "Framework/TraceUtility.h"
#include "Framework/Actor.h"
#include "Engine/World.h"

void Jgc::ACameraBoomComponent::ParentTick(f32 Dt)
{
    Super::ParentTick(Dt);

    bool bViewed{};
    for (auto const& Child: this->GetChildren())
    {
        Jafg::AEyeComponent::ForEachEyeIn(*Child, [&bViewed](Jafg::AEyeComponent const& Eye)
        {
            bViewed = bViewed || Eye.IsViewed();
        });
    }

    f32 Target{maths::max(this->ArmLength, 0.0f)};
    if (bViewed && Target > 0.0f)
    {
        /* From the pivot to where the eyes want to be. */
        LWorldTrans const World{this->GetWorldTransformSlow()};
        LVec3F const Local{this->SocketOffset + maths::backward_vector<LVec3F> * Target};
        LWorldVec3 const Begin{World.t};
        LWorldVec3 const End{World.t + LWorldVec3{World.r * (LVec3F{World.s} * Local)}};

        TArray<Jafg::LHitResult> Hits;
        Jafg::LCollisionQueryParams Params{.bSingleHit = true, .bSortByDistance = true};
        Params.IgnoredActors.emplace_back(&this->GetOwningActor());
        if (this->GetWorld().LineTraceByChannel(&Hits, Begin, End, this->Channels, Params) && !Hits.empty())
        {
            f32 const Full{static_cast<f32>(maths::distance(Begin, End))};
            f32 const Hit{static_cast<f32>(maths::distance(Begin, Hits.front().GlobalWorldLocation))};
            if (Full > 0.0f)
            {
                Target *= maths::clamp((Hit - this->ProbeMargin) / Full, 0.0f, 1.0f);
            }
        }
    }

    if (this->CurrentLength < 0.0f || Target < this->CurrentLength)
    {
        /* Never through walls. */
        this->CurrentLength = Target;
    }
    else if (Dt > 0.0f)
    {
        this->CurrentLength += (Target - this->CurrentLength) * (1.0f - std::exp(-this->ExtendSpeed * Dt));
    }

    f32 const Fraction{this->ArmLength > 0.0f ? this->CurrentLength / this->ArmLength : 0.0f};
    LWorldVec3 const Translation{this->SocketOffset * Fraction + maths::backward_vector<LVec3F> * this->CurrentLength};
    for (auto const& Child: this->GetChildren())
    {
        if (Child->GetLocalTranslation() != Translation)
        {
            Child->SetLocalTranslationByTeleport(Translation);
        }
    }
}
