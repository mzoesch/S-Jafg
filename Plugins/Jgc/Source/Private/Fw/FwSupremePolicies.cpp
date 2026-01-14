// Copyright mzoesch. All rights reserved.

#include "Fw/FwSupremePolicies.h"
#include "Framework/Actor.h"
#include "Components/StaticMeshComponent.h"

void Jgc::JFwSupremePolicies::OnWorldPreInit()
{
    Super::OnWorldPreInit();

    this->GetWorldChecked()->SetBackgroundColor(Lal::LLinearColor::DeepSkyBlue);

    this->RotatingActor = Jafg::SpawnDeferredActor<Jafg::AActor>(this->GetWorld());
    this->RotatingActor->EmplaceComponent<Jafg::LStaticMeshComponent>(Jafg::LStaticMeshComponent::LCreateInfo{
        .MeshPath = "Content/Models/viking_room.obj",
        .TexturePath = "Content/Textures/viking_room.png",
        });

    return;
}
