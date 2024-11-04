// Copyright mzoesch. All rights reserved.

#include "Widgets/Spacer.h"

void Jafg::WSpacer::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();
    this->SetDesiredSize(this->Size);
    return;
}
