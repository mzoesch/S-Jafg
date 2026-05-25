// Copyright mzoesch. All rights reserved.

#include "Nodes/TextIconized.h"
#include "Framework/Frontend.h"
#include "Rhi/NodeRenderInfo.h"

void Jafg::Detail::DrawIcon(LNodeRenderInfo const& Info, LVec2F TopLeft, LTexture2Ref const& Icon, LIconBrush const& Brush) noexcept
{
    check(Icon.get())

    if (Brush.Scale > 0.0f)
    {
        if (!Icon->IsBindless())
        {
            Info.Frontend.Vk_AddTextureToGlobalBindlessArray(&*Icon);
            check(Icon->IsBindless())
        }
        Info.AddInstance({
            .Rect = {maths::round(TopLeft), Icon->GetExtentAsVec2F() * static_cast<f32>(Brush.Scale)},
            .Tint = Brush.Tint,
            .TextureIndex = Icon->GetBindlessIndex(),
            });
    }

    return;
}

void Jafg::WTextIconizedLeft::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);
    if (this->Icon.get())
    {
        Detail::DrawIcon(Info, this->GetIconTopLeft(Info.Translation), this->Icon, this->IconBrush);
    }
    return;
}

void Jafg::WTextIconizedLeft::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();
    auto Size{this->GetIconSize(this->GetViewport())};
    this->SetDesiredSizeInSpt(this->GetDesiredSize_v2() + Size);
    this->TextDrawOffset = Size;
    return;
}

void Jafg::WTextIconizedRight::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);
    if (this->Icon.get())
    {
        Detail::DrawIcon(Info, this->GetIconTopLeft(Info.Translation), this->Icon, this->IconBrush);
    }
    return;
}

void Jafg::WTextIconizedRight::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();
    auto Size{this->GetIconSize(this->GetViewport())};
    this->SetDesiredSizeInSpt(this->GetDesiredSize_v2() + Size);
    this->TextPlayroomReduction = Size;
    return;
}
