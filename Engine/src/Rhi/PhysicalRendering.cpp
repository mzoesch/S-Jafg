// Copyright mzoesch. All rights reserved.

#include "Rhi/PhysicalRendering.h"

namespace
{

Jafg::LPushConstantRegistrator<Jafg::PC::Pbr> _0{};
Jafg::LPushConstantRegistrator<Jafg::PC::MeshPbr> _1{};

} /* ~Namespace <Anonymous> */

void Jafg::LPbrMaterialParameters::ToDevice(SSBO::BindlessPbrMaterial& Data) const noexcept
{
    Data.BaseColor = this->BaseColor;
    Data.BaseWeight = this->BaseWeight;
    Data.BaseMetalness = this->BaseMetalness;
    Data.BaseDiffuseRoughness = this->BaseDiffuseRoughness;
    Data.SpecularWeight = this->SpecularWeight;
    Data.SpecularRoughness = this->SpecularRoughness;

    Data.SpecularColor = this->SpecularColor;
    Data.SpecularIor = this->SpecularIor;
    Data.SpecularRoughnessAnisotropy = this->SpecularRoughnessAnisotropy;
    Data.GeometryTangentRotation = this->GeometryTangentRotation;
    Data.OcclusionFactor = this->OcclusionFactor;
    Data.GeometryOpacity = this->GeometryOpacity;

    Data.TransmissionColor = this->TransmissionColor;
    Data.TransmissionWeight = this->TransmissionWeight;
    Data.TransmissionScatter = this->TransmissionScatter;
    Data.TransmissionDepth = this->TransmissionDepth;
    Data.TransmissionScatterAnisotropy = this->TransmissionScatterAnisotropy;
    Data.TransmissionDispersionScale = this->TransmissionDispersionScale;
    Data.TransmissionDispersionAbbeNumber = this->TransmissionDispersionAbbeNumber;
    Data.SubsurfaceWeight = this->SubsurfaceWeight;

    Data.SubsurfaceColor = this->SubsurfaceColor;
    Data.SubsurfaceRadius = this->SubsurfaceRadius;
    Data.SubsurfaceRadiusScale = this->SubsurfaceRadiusScale;
    Data.SubsurfaceScatterAnisotropy = this->SubsurfaceScatterAnisotropy;

    Data.CoatColor = this->CoatColor;
    Data.CoatWeight = this->CoatWeight;
    Data.CoatRoughness = this->CoatRoughness;
    Data.CoatRoughnessAnisotropy = this->CoatRoughnessAnisotropy;
    Data.CoatIor = this->CoatIor;
    Data.CoatDarkening = this->CoatDarkening;

    Data.FuzzColor = this->FuzzColor;
    Data.FuzzWeight = this->FuzzWeight;
    Data.EmissionColor = this->EmissionColor;
    Data.EmissionLuminance = this->EmissionLuminance;

    Data.FuzzRoughness = this->FuzzRoughness;
    Data.ThinFilmWeight = this->ThinFilmWeight;
    Data.ThinFilmThickness = this->ThinFilmThickness;
    Data.ThinFilmIor = this->ThinFilmIor;

    Data.GeometryCoatTangentRotation = this->GeometryCoatTangentRotation;
    Data.AlphaCutoff = this->AlphaCutoff;
    Data.NormalScale = this->NormalScale;
    Data.CoatNormalScale = this->CoatNormalScale;

    Data.UVScale = this->UVScale;
    Data.UVOffset = this->UVOffset;

    Data.Sampler = this->Sampler;
    Data.Flags = this->Flags;
    Data.GeometryThinWalled = this->GeometryThinWalled ? 1u : 0u;
}

Jafg::LPbrMaterialParameters Jafg::LPbrMaterialParameters::FromDevice(SSBO::BindlessPbrMaterial const& Data) noexcept
{
    LPbrMaterialParameters Result;

    Result.BaseColor = Data.BaseColor;
    Result.BaseWeight = Data.BaseWeight;
    Result.BaseMetalness = Data.BaseMetalness;
    Result.BaseDiffuseRoughness = Data.BaseDiffuseRoughness;
    Result.SpecularWeight = Data.SpecularWeight;
    Result.SpecularRoughness = Data.SpecularRoughness;

    Result.SpecularColor = Data.SpecularColor;
    Result.SpecularIor = Data.SpecularIor;
    Result.SpecularRoughnessAnisotropy = Data.SpecularRoughnessAnisotropy;
    Result.GeometryTangentRotation = Data.GeometryTangentRotation;
    Result.OcclusionFactor = Data.OcclusionFactor;
    Result.GeometryOpacity = Data.GeometryOpacity;

    Result.TransmissionColor = Data.TransmissionColor;
    Result.TransmissionWeight = Data.TransmissionWeight;
    Result.TransmissionScatter = Data.TransmissionScatter;
    Result.TransmissionDepth = Data.TransmissionDepth;
    Result.TransmissionScatterAnisotropy = Data.TransmissionScatterAnisotropy;
    Result.TransmissionDispersionScale = Data.TransmissionDispersionScale;
    Result.TransmissionDispersionAbbeNumber = Data.TransmissionDispersionAbbeNumber;
    Result.SubsurfaceWeight = Data.SubsurfaceWeight;

    Result.SubsurfaceColor = Data.SubsurfaceColor;
    Result.SubsurfaceRadius = Data.SubsurfaceRadius;
    Result.SubsurfaceRadiusScale = Data.SubsurfaceRadiusScale;
    Result.SubsurfaceScatterAnisotropy = Data.SubsurfaceScatterAnisotropy;

    Result.CoatColor = Data.CoatColor;
    Result.CoatWeight = Data.CoatWeight;
    Result.CoatRoughness = Data.CoatRoughness;
    Result.CoatRoughnessAnisotropy = Data.CoatRoughnessAnisotropy;
    Result.CoatIor = Data.CoatIor;
    Result.CoatDarkening = Data.CoatDarkening;

    Result.FuzzColor = Data.FuzzColor;
    Result.FuzzWeight = Data.FuzzWeight;
    Result.EmissionColor = Data.EmissionColor;
    Result.EmissionLuminance = Data.EmissionLuminance;

    Result.FuzzRoughness = Data.FuzzRoughness;
    Result.ThinFilmWeight = Data.ThinFilmWeight;
    Result.ThinFilmThickness = Data.ThinFilmThickness;
    Result.ThinFilmIor = Data.ThinFilmIor;

    Result.GeometryCoatTangentRotation = Data.GeometryCoatTangentRotation;
    Result.AlphaCutoff = Data.AlphaCutoff;
    Result.NormalScale = Data.NormalScale;
    Result.CoatNormalScale = Data.CoatNormalScale;

    Result.UVScale = Data.UVScale;
    Result.UVOffset = Data.UVOffset;

    Result.Sampler = Data.Sampler;
    Result.Flags = Data.Flags;
    Result.GeometryThinWalled = Data.GeometryThinWalled != 0u;

    return Result;
}

bool Jafg::LPbrMaterialParameters::IsTranslucent() const noexcept
{
    /* Masked materials are either fully there or not at all. */
    if (!!(this->Flags & EPbrMaterialBits::AlphaMask))
    {
        return this->TransmissionWeight > 0.0f && this->BaseMetalness < 1.0f;
    }
    return this->GeometryOpacity < 1.0f
        || !!(this->Flags & EPbrMaterialBits::AlphaBlend)
        || (this->TransmissionWeight > 0.0f && this->BaseMetalness < 1.0f);
}

Jafg::LPbrDrawPlan Jafg::LPbrMaterialInstance::GetDrawPlan() const noexcept
{
    check(this->Instance)

    /* Other underlying materials have no variants. */
    if (!this->Translucent)
    {
        return {.Front = this->Instance.get()};
    }
    check(this->DoubleSided && this->TranslucentBackFaces)

    LPbrMaterialParameters const Parameters{LPbrMaterialParameters::FromDevice(this->Data)};
    bool const bDoubleSided{Parameters.IsDoubleSided()};

    if (!Parameters.IsTranslucent())
    {
        return {.Front = bDoubleSided ? this->DoubleSided.get() : this->Instance.get()};
    }

    return {
        /* The back faces of a closed mesh are behind its front faces. Drawing them first keeps the order. */
        .Back = bDoubleSided ? this->TranslucentBackFaces.get() : nullptr,
        .Front = this->Translucent.get(),
        .bTranslucent = true,
        /* Thin walls have no volume. The light leaves them right where it entered. */
        .bThickness = Parameters.TransmissionWeight > 0.0f && !Parameters.GeometryThinWalled,
        };
}
