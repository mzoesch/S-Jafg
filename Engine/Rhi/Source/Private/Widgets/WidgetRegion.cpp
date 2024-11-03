// Copyright mzoesch. All rights reserved.

#include "Widgets/WidgetRegion.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

void Jafg::WWidgetRegion::Draw(LViewport* Context) const
{
    if (this->HasBrush() == false)
    {
        if (this->ShaderContext)
        {
            this->ShaderContext.Reset();
        }

        Super::Draw(Context);

        return;
    }

    if (this->ShaderContext == false)
    {
        LOG_TRACE(LogTemporal, "Creating new shader context for WWidgetRegion.")
        this->ShaderContext.MakeMeaningful();
        this->CreateNewShaderContext();
    }

    this->ShaderContext->Draw(*Context, this->GetDesiredSize(), this->GetRelativeTopLeftFromMostOuter(this));

    Super::Draw(Context);

    return;
}

void Jafg::WWidgetRegion::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    LVector2 DesiredSize = LVector2::Zero();
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        DesiredSize.X = Maths::Max(DesiredSize.X, ChildSlot->Content->GetDesiredSize().X);
        DesiredSize.Y = Maths::Max(DesiredSize.Y, ChildSlot->Content->GetDesiredSize().Y);

        continue;
    }

    this->SetDesiredSize(DesiredSize);

    return;
}

void Jafg::WWidgetRegion::CreateNewShaderContext() const
{
    this->ShaderContext->Make();
}
