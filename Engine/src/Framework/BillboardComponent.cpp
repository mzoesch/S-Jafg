// Copyright mzoesch. All rights reserved.

#include "Framework/BillboardComponent.h"

void Jafg::ABillboardComponent::Render(LActorRenderInfo const& Info) const
{
    Super::Render(Info);

    check(this->Billboard.GetTexture())

    this->Billboard.Render(Info, {
        this->GetWorldTranslationSlow()
        });
}
