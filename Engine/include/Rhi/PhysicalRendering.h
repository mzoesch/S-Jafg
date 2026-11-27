// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/Bindless.h"
#include "Rhi/Material.h"
#include "Rhi/Objects.h"
#include "Rhi/Light.h"
#include "Framework/ActorComponentForward.h"

namespace Jafg
{

struct LPbrMaterialTemplate;
struct LPbrMaterialInstance;
typedef rhi::shared_ref<LPbrMaterialInstance> LPbrMaterialInstanceRef;

namespace SSBO
{
struct BindlessPbrMaterial;
} /* ~Namespace SSBO */

template<> ENGINE_API Detail::LNodeFactoryBase GetEditorNode<LPbrMaterialInstanceRef>(TEditorNodeCreateInfo<LPbrMaterialInstanceRef> const& Info) noexcept;

enum struct EPbrMaterialBits: u32 /* Stable across shader barrier. */
{
    Identity = 0x0 << 0,
    //# Discard where the coverage (geometry_opacity times the alpha of the base color texture) is below the alpha
    //# cutoff. Everything else is fully opaque.
    AlphaMask = 0x1 << 0,
    //# Draw both faces. Back faces are shaded with flipped normals. Implied by geometry_thin_walled.
    DoubleSided = 0x1 << 1,
    //# Use direct x normal map format. Jafg uses by default the OpenGl standard.
    //# Y-Coords of normals will be flipped.
    DirectXNormals = 0x1 << 2,
    //# Blend by the coverage (geometry_opacity times the alpha of the base color texture), even if geometry_opacity
    //# is one. Without it, the alpha of the base color texture is ignored (unless masked).
    AlphaBlend = 0x1 << 3,
};
SERDE_ENUM_STRUCT_FLAGS(EPbrMaterialBits, EPbrMaterialFlags)
SERDE_JSON_ENUM(EPbrMaterialBits, Identity, AlphaMask, DoubleSided, DirectXNormals, AlphaBlend)

//# Everything above this is not a finite value (e.g., the upper bound of an index of refraction).
inline constexpr f32 PbrUnbounded{ std::numeric_limits<f32>::infinity() };

//#
//# Describes a parameter of a pbr material: How it is named, what it does and which values it takes.
//#
//# - Range: The values that are valid (per channel for colors). Everything outside is clamped when it is loaded or
//#          edited. An open lower bound (e.g., of an index of refraction) excludes the bound itself.
//# - Norm:  The values that are typical. Editors span their sliders across them, but still accept the whole range.
//#
//# @see https://academysoftwarefoundation.github.io/OpenPBR/parametrization.md.html
//#
template<typename T>
struct TPbrParameter final
{
    typedef T value_type;

    //# The key in *.pbr.json files. The OpenPBR identifier of the parameter, if it is part of OpenPBR.
    LStringView Key;
    T Default;

    f32 Min{};
    f32 Max{};
    bool bOpenMin{};
    f32 NormMin{};
    f32 NormMax{};

    LStringView Description;
    //# Unit of the values, if any.
    LStringView Unit;
    //# Whether this is part of OpenPBR. Else, it is an extension of Jafg.
    bool bOpenPbr{ true };

    //# The smallest valid value.
    NODISCARD f32 GetMin() const noexcept
    {
        return this->bOpenMin ? std::nextafter(this->Min, PbrUnbounded) : this->Min;
    }

    //# @return The value clamped into the range (per channel).
    NODISCARD T Clamp(T Value) const noexcept
    {
        if constexpr (std::is_same_v<T, bool>)
        {
            return Value;
        }
        else if constexpr (std::is_arithmetic_v<T>)
        {
            return std::clamp(Value, static_cast<T>(this->GetMin()), static_cast<T>(this->Max));
        }
        else
        {
            for (auto Idx{0}; Idx < T::length(); ++Idx)
            {
                Value[Idx] = std::clamp(Value[Idx], this->GetMin(), this->Max);
            }
            return Value;
        }
    }

    //# E.g., "Range: [0,1]. Norm: [0,1]. Default: 0.5." in a human-readable form.
    NODISCARD LString DescribeValues() const
    {
        auto Number{[](f32 Value) -> LString
        {
            if (Value == PbrUnbounded)
            {
                return "inf";
            }
            if (Value == -PbrUnbounded)
            {
                return "-inf";
            }
            return algo::sprintf("{}", Value);
        }};

        LString Default;
        if constexpr (std::is_same_v<T, bool>)
        {
            return algo::sprintf("Default: {}.", this->Default ? "true" : "false");
        }
        else if constexpr (std::is_arithmetic_v<T>)
        {
            Default = Number(this->Default);
        }
        else
        {
            Default = "(";
            for (auto Idx{0}; Idx < T::length(); ++Idx)
            {
                Default += (Idx == 0 ? "" : ", ") + Number(this->Default[Idx]);
            }
            Default += ")";
        }

        LString Result{algo::sprintf("Range: {}{},{}]. Norm: [{},{}]. Default: {}."
            , this->bOpenMin || this->Min == -PbrUnbounded ? "(" : "[", Number(this->Min), Number(this->Max)
            , Number(this->NormMin), Number(this->NormMax), Default)};
        if (this->Max == PbrUnbounded)
        {
            /* Unbounded intervals are open. */
            Result.replace(Result.find("inf]"), 4, "inf)");
        }
        if (!this->Unit.empty())
        {
            Result += algo::sprintf(" Unit: {}.", this->Unit);
        }
        return Result;
    }
};

//#
//# All parameters of a pbr material that can be edited on an instance: The OpenPBR surface parameters and the
//# extensions of Jafg (marked as such). Everything but the textures and the underlying material.
//#
//# Jafg uses for its pbr materials the open pbr standard (v1.1). Lengths are in centimeters (thin_film_thickness in
//# micrometers) as specified, colors are linear Rec. 709. Luminances are in the units of the lights (nits). That is,
//# a white diffuse surface lit head-on by a directional light of pi lux has a luminance of one nit, which is the
//# white of the display at the default exposure.
//#
//# Factors of parameters that can also be textured are multiplied with the sampled texture values if bound, else the
//# factors are used directly.
//#
//# @see https://academysoftwarefoundation.github.io/OpenPBR
//#
struct LPbrMaterialParameters
{
    //////////////////////////////////////////////////////////////////////////
    // Base.
    //////////////////////////////////////////////////////////////////////////

    static constexpr TPbrParameter<f32> BaseWeightInfo{
        .Key = "base_weight", .Default = 1.0f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Multiplier on the base color of both, the dielectric and the metal.",
        };
    f32 BaseWeight{ BaseWeightInfo.Default };

    static constexpr TPbrParameter<LVec3F> BaseColorInfo{
        .Key = "base_color", .Default = {0.8f, 0.8f, 0.8f}, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Albedo of the diffuse base, or the reflectance at normal incidence of the metal. Multiplied "
                       "with the rgb of the base color texture.",
        };
    LVec3F BaseColor{ BaseColorInfo.Default };

    static constexpr TPbrParameter<f32> BaseMetalnessInfo{
        .Key = "base_metalness", .Default = 0.0f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Mix between the dielectric (0) and the metal (1). Multiplied with the blue channel of the arm "
                       "texture.",
        };
    f32 BaseMetalness{ BaseMetalnessInfo.Default };

    static constexpr TPbrParameter<f32> BaseDiffuseRoughnessInfo{
        .Key = "base_diffuse_roughness", .Default = 0.0f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Roughness of the diffuse base with 0 = Lambertian. Flattens the diffuse response like of dusty "
                       "or powdery surfaces (energy-preserving Oren-Nayar).",
        };
    f32 BaseDiffuseRoughness{ BaseDiffuseRoughnessInfo.Default };

    //////////////////////////////////////////////////////////////////////////
    // Specular.
    //////////////////////////////////////////////////////////////////////////

    static constexpr TPbrParameter<f32> SpecularWeightInfo{
        .Key = "specular_weight", .Default = 1.0f, .Min = 0.0f, .Max = PbrUnbounded, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Multiplier on the reflectance of the specular layer at normal incidence. 0 = no specular at "
                       "all. Above 1 the dielectric reflects more, up to everything. Multiplied with the alpha of the "
                       "specular texture.",
        };
    f32 SpecularWeight{ SpecularWeightInfo.Default };

    static constexpr TPbrParameter<LVec3F> SpecularColorInfo{
        .Key = "specular_color", .Default = {1.0f, 1.0f, 1.0f}, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Dielectrics: Tint of the reflection (not physical). Metals: Tint of the reflection near grazing "
                       "angles (F82), e.g. the reddish edges of copper. Multiplied with the rgb of the specular texture.",
        };
    LVec3F SpecularColor{ SpecularColorInfo.Default };

    static constexpr TPbrParameter<f32> SpecularRoughnessInfo{
        .Key = "specular_roughness", .Default = 0.3f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Perceptual roughness of the specular layer (and of the transmission) with 0 = mirror-like. "
                       "Multiplied with the green channel of the arm texture.",
        };
    f32 SpecularRoughness{ SpecularRoughnessInfo.Default };

    static constexpr TPbrParameter<f32> SpecularRoughnessAnisotropyInfo{
        .Key = "specular_roughness_anisotropy", .Default = 0.0f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Stretches the highlights along the tangent (geometry_tangent) with 0 = isotropic, like of "
                       "brushed metal. Multiplied with the blue channel of the anisotropy texture.",
        };
    f32 SpecularRoughnessAnisotropy{ SpecularRoughnessAnisotropyInfo.Default };

    static constexpr TPbrParameter<f32> SpecularIorInfo{
        .Key = "specular_ior", .Default = 1.5f, .Min = 0.0f, .Max = PbrUnbounded, .bOpenMin = true, .NormMin = 1.0f, .NormMax = 3.0f,
        .Description = "Index of refraction of the dielectric. 1.5 reflects 4% at normal incidence.",
        };
    f32 SpecularIor{ SpecularIorInfo.Default };

    //////////////////////////////////////////////////////////////////////////
    // Transmission.
    //////////////////////////////////////////////////////////////////////////

    static constexpr TPbrParameter<f32> TransmissionWeightInfo{
        .Key = "transmission_weight", .Default = 0.0f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Mix between the opaque dielectric base (0) and a transparent one (1) that refracts the light "
                       "into its volume, like glass or liquids. Translucent materials are drawn after everything "
                       "opaque and see only that through themselves.",
        };
    f32 TransmissionWeight{ TransmissionWeightInfo.Default };

    static constexpr TPbrParameter<LVec3F> TransmissionColorInfo{
        .Key = "transmission_color", .Default = {1.0f, 1.0f, 1.0f}, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "The color that white light has after traveling transmission_depth through the volume. Without "
                       "a depth, a tint of the refracted light. Thin walled: The transmittance of the sheet.",
        };
    LVec3F TransmissionColor{ TransmissionColorInfo.Default };

    static constexpr TPbrParameter<f32> TransmissionDepthInfo{
        .Key = "transmission_depth", .Default = 0.0f, .Min = 0.0f, .Max = PbrUnbounded, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "The distance at which white light has the transmission_color. 0 = no volume, the color is a "
                       "tint only. The thickness of objects is measured from their back faces on screen.",
        .Unit = "cm",
        };
    f32 TransmissionDepth{ TransmissionDepthInfo.Default };

    static constexpr TPbrParameter<LVec3F> TransmissionScatterInfo{
        .Key = "transmission_scatter", .Default = {0.0f, 0.0f, 0.0f}, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Scattering of the volume relative to its extinction, which turns it cloudy or milky. Needs a "
                       "transmission_depth.",
        };
    LVec3F TransmissionScatter{ TransmissionScatterInfo.Default };

    static constexpr TPbrParameter<f32> TransmissionScatterAnisotropyInfo{
        .Key = "transmission_scatter_anisotropy", .Default = 0.0f, .Min = -1.0f, .Max = 1.0f, .NormMin = -1.0f, .NormMax = 1.0f,
        .Description = "Direction of the scattering in the volume: -1 = backwards, 0 = isotropic, 1 = forwards.",
        };
    f32 TransmissionScatterAnisotropy{ TransmissionScatterAnisotropyInfo.Default };

    static constexpr TPbrParameter<f32> TransmissionDispersionScaleInfo{
        .Key = "transmission_dispersion_scale", .Default = 0.0f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Strength of the dispersion (rainbows of refracted light). 0 = none, 1 = the dispersion of the "
                       "abbe number.",
        };
    f32 TransmissionDispersionScale{ TransmissionDispersionScaleInfo.Default };

    static constexpr TPbrParameter<f32> TransmissionDispersionAbbeNumberInfo{
        .Key = "transmission_dispersion_abbe_number", .Default = 20.0f, .Min = 0.0f, .Max = PbrUnbounded, .bOpenMin = true, .NormMin = 9.0f, .NormMax = 91.0f,
        .Description = "Abbe number of the dielectric. The lower, the stronger the dispersion. Glass 20 to 91, water and "
                       "diamond about 55.",
        };
    f32 TransmissionDispersionAbbeNumber{ TransmissionDispersionAbbeNumberInfo.Default };

    //////////////////////////////////////////////////////////////////////////
    // Subsurface.
    //////////////////////////////////////////////////////////////////////////

    static constexpr TPbrParameter<f32> SubsurfaceWeightInfo{
        .Key = "subsurface_weight", .Default = 0.0f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Mix between the diffuse base (0) and the subsurface (1), whose light scatters below the surface "
                       "(skin, wax, marble). Approximated: The light wraps around the terminator instead of diffusing "
                       "across the surface. Multiplied with the alpha of the subsurface texture.",
        };
    f32 SubsurfaceWeight{ SubsurfaceWeightInfo.Default };

    static constexpr TPbrParameter<LVec3F> SubsurfaceColorInfo{
        .Key = "subsurface_color", .Default = {0.8f, 0.8f, 0.8f}, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Albedo of the subsurface. Multiplied with the rgb of the subsurface texture.",
        };
    LVec3F SubsurfaceColor{ SubsurfaceColorInfo.Default };

    static constexpr TPbrParameter<f32> SubsurfaceRadiusInfo{
        .Key = "subsurface_radius", .Default = 1.0f, .Min = 0.0f, .Max = PbrUnbounded, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Mean free path of the light below the surface. Relative to the curvature of the surface, it "
                       "decides how far the light wraps around the terminator. Ignored if thin walled.",
        .Unit = "cm",
        };
    f32 SubsurfaceRadius{ SubsurfaceRadiusInfo.Default };

    static constexpr TPbrParameter<LVec3F> SubsurfaceRadiusScaleInfo{
        .Key = "subsurface_radius_scale", .Default = {1.0f, 0.5f, 0.25f}, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Multiplied per color channel with the subsurface_radius. Red scatters the farthest in most "
                       "materials (e.g. skin). Ignored if thin walled.",
        };
    LVec3F SubsurfaceRadiusScale{ SubsurfaceRadiusScaleInfo.Default };

    static constexpr TPbrParameter<f32> SubsurfaceScatterAnisotropyInfo{
        .Key = "subsurface_scatter_anisotropy", .Default = 0.0f, .Min = -1.0f, .Max = 1.0f, .NormMin = -1.0f, .NormMax = 1.0f,
        .Description = "Direction of the scattering below the surface: -1 = backwards, 0 = isotropic, 1 = forwards. "
                       "Forward scattering carries the light deeper. Thin walled: Splits the light between the "
                       "reflection (backwards) and the transmission through the sheet (forwards).",
        };
    f32 SubsurfaceScatterAnisotropy{ SubsurfaceScatterAnisotropyInfo.Default };

    //////////////////////////////////////////////////////////////////////////
    // Coat.
    //////////////////////////////////////////////////////////////////////////

    static constexpr TPbrParameter<f32> CoatWeightInfo{
        .Key = "coat_weight", .Default = 0.0f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Coverage of a clear coat (varnish, lacquer) on top of the base. It has its own reflection and "
                       "absorbs, darkens and roughens what is below.",
        };
    f32 CoatWeight{ CoatWeightInfo.Default };

    static constexpr TPbrParameter<LVec3F> CoatColorInfo{
        .Key = "coat_color", .Default = {1.0f, 1.0f, 1.0f}, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Transmittance of the coat (through it and back at normal incidence). Tints what is below, not "
                       "the reflection of the coat itself.",
        };
    LVec3F CoatColor{ CoatColorInfo.Default };

    static constexpr TPbrParameter<f32> CoatRoughnessInfo{
        .Key = "coat_roughness", .Default = 0.0f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Perceptual roughness of the coat.",
        };
    f32 CoatRoughness{ CoatRoughnessInfo.Default };

    static constexpr TPbrParameter<f32> CoatRoughnessAnisotropyInfo{
        .Key = "coat_roughness_anisotropy", .Default = 0.0f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Stretches the highlights of the coat along its tangent (geometry_coat_tangent) with 0 = "
                       "isotropic.",
        };
    f32 CoatRoughnessAnisotropy{ CoatRoughnessAnisotropyInfo.Default };

    static constexpr TPbrParameter<f32> CoatIorInfo{
        .Key = "coat_ior", .Default = 1.6f, .Min = 0.0f, .Max = PbrUnbounded, .bOpenMin = true, .NormMin = 1.0f, .NormMax = 3.0f,
        .Description = "Index of refraction of the coat. Also changes the reflection of the base below it.",
        };
    f32 CoatIor{ CoatIorInfo.Default };

    static constexpr TPbrParameter<f32> CoatDarkeningInfo{
        .Key = "coat_darkening", .Default = 1.0f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "How much the light that bounces inside of the coat darkens and saturates the base. 1 = "
                       "physical, 0 = none.",
        };
    f32 CoatDarkening{ CoatDarkeningInfo.Default };

    //////////////////////////////////////////////////////////////////////////
    // Fuzz.
    //////////////////////////////////////////////////////////////////////////

    static constexpr TPbrParameter<f32> FuzzWeightInfo{
        .Key = "fuzz_weight", .Default = 0.0f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Coverage of a layer of fine fibers or dust on top of everything (velvet, peach fuzz, dust).",
        };
    f32 FuzzWeight{ FuzzWeightInfo.Default };

    static constexpr TPbrParameter<LVec3F> FuzzColorInfo{
        .Key = "fuzz_color", .Default = {1.0f, 1.0f, 1.0f}, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Color of the fuzz.",
        };
    LVec3F FuzzColor{ FuzzColorInfo.Default };

    static constexpr TPbrParameter<f32> FuzzRoughnessInfo{
        .Key = "fuzz_roughness", .Default = 0.5f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Low: Fibers that light up at grazing angles (sheen). High: Round grains that scatter in every "
                       "direction (dust).",
        };
    f32 FuzzRoughness{ FuzzRoughnessInfo.Default };

    //////////////////////////////////////////////////////////////////////////
    // Emission.
    //////////////////////////////////////////////////////////////////////////

    static constexpr TPbrParameter<f32> EmissionLuminanceInfo{
        .Key = "emission_luminance", .Default = 0.0f, .Min = 0.0f, .Max = PbrUnbounded, .NormMin = 0.0f, .NormMax = 1000.0f,
        .Description = "Luminance of the emission for a white emission_color. In the units of the lights: 1 nit is the "
                       "white of the display at the default exposure. Below the coat and the fuzz, which absorb it.",
        .Unit = "nits",
        };
    f32 EmissionLuminance{ EmissionLuminanceInfo.Default };

    static constexpr TPbrParameter<LVec3F> EmissionColorInfo{
        .Key = "emission_color", .Default = {1.0f, 1.0f, 1.0f}, .Min = 0.0f, .Max = PbrUnbounded, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Color of the emission. Multiplied with the rgb of the emission texture.",
        };
    LVec3F EmissionColor{ EmissionColorInfo.Default };

    //////////////////////////////////////////////////////////////////////////
    // Thin film.
    //////////////////////////////////////////////////////////////////////////

    static constexpr TPbrParameter<f32> ThinFilmWeightInfo{
        .Key = "thin_film_weight", .Default = 0.0f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Coverage of a thin film (soap, oil, oxide layers) on the base, whose interference colors the "
                       "reflection. Multiplied with the red channel of the thin film texture.",
        };
    f32 ThinFilmWeight{ ThinFilmWeightInfo.Default };

    static constexpr TPbrParameter<f32> ThinFilmThicknessInfo{
        .Key = "thin_film_thickness", .Default = 0.5f, .Min = 0.0f, .Max = PbrUnbounded, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Thickness of the thin film. Interference colors need about 0.1 to 1. Multiplied with the green "
                       "channel of the thin film texture.",
        .Unit = "um",
        };
    f32 ThinFilmThickness{ ThinFilmThicknessInfo.Default };

    static constexpr TPbrParameter<f32> ThinFilmIorInfo{
        .Key = "thin_film_ior", .Default = 1.4f, .Min = 0.0f, .Max = PbrUnbounded, .bOpenMin = true, .NormMin = 1.0f, .NormMax = 3.0f,
        .Description = "Index of refraction of the thin film.",
        };
    f32 ThinFilmIor{ ThinFilmIorInfo.Default };

    //////////////////////////////////////////////////////////////////////////
    // Geometry.
    //////////////////////////////////////////////////////////////////////////

    static constexpr TPbrParameter<f32> GeometryOpacityInfo{
        .Key = "geometry_opacity", .Default = 1.0f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Coverage of the surface with 0 = not there. Multiplied with the alpha of the base color texture "
                       "if alpha masked or blended. Below 1, the material is drawn translucent.",
        };
    f32 GeometryOpacity{ GeometryOpacityInfo.Default };

    static constexpr TPbrParameter<bool> GeometryThinWalledInfo{
        .Key = "geometry_thin_walled", .Default = false,
        .Description = "The surface is an infinitely thin, double sided sheet instead of the boundary of a volume (e.g., "
                       "leaves, paper, soap bubbles, window panes). Transmission does not refract and the subsurface "
                       "lets light through.",
        };
    bool GeometryThinWalled{ GeometryThinWalledInfo.Default };

    //# geometry_normal is the normal texture (scaled by #NormalScale), geometry_coat_normal the coat normal texture.
    static constexpr TPbrParameter<f32> NormalScaleInfo{
        .Key = "normal_scale", .Default = 1.0f, .Min = 0.0f, .Max = PbrUnbounded, .NormMin = 0.0f, .NormMax = 2.0f,
        .Description = "Strength of the normal texture (geometry_normal) with 0 = no normal mapping.",
        .bOpenPbr = false,
        };
    f32 NormalScale{ NormalScaleInfo.Default };

    static constexpr TPbrParameter<f32> CoatNormalScaleInfo{
        .Key = "coat_normal_scale", .Default = 1.0f, .Min = 0.0f, .Max = PbrUnbounded, .NormMin = 0.0f, .NormMax = 2.0f,
        .Description = "Strength of the coat normal texture (geometry_coat_normal) with 0 = no normal mapping. Without "
                       "a texture, the coat is smooth even above a normal mapped base.",
        .bOpenPbr = false,
        };
    f32 CoatNormalScale{ CoatNormalScaleInfo.Default };

    //# geometry_tangent is the tangent of the mesh, turned by the direction of the anisotropy texture and this.
    static constexpr TPbrParameter<f32> GeometryTangentRotationInfo{
        .Key = "geometry_tangent_rotation", .Default = 0.0f, .Min = -PbrUnbounded, .Max = PbrUnbounded, .NormMin = -180.0f, .NormMax = 180.0f,
        .Description = "Turns the tangent of the anisotropy (geometry_tangent) around the normal, from the tangent of "
                       "the mesh towards its bitangent.",
        .Unit = "degrees",
        .bOpenPbr = false,
        };
    f32 GeometryTangentRotation{ GeometryTangentRotationInfo.Default };

    static constexpr TPbrParameter<f32> GeometryCoatTangentRotationInfo{
        .Key = "geometry_coat_tangent_rotation", .Default = 0.0f, .Min = -PbrUnbounded, .Max = PbrUnbounded, .NormMin = -180.0f, .NormMax = 180.0f,
        .Description = "Turns the tangent of the anisotropy of the coat (geometry_coat_tangent) around the normal, from "
                       "the tangent of the mesh towards its bitangent.",
        .Unit = "degrees",
        .bOpenPbr = false,
        };
    f32 GeometryCoatTangentRotation{ GeometryCoatTangentRotationInfo.Default };

    //////////////////////////////////////////////////////////////////////////
    // Extensions of Jafg.
    //////////////////////////////////////////////////////////////////////////

    static constexpr TPbrParameter<f32> OcclusionFactorInfo{
        .Key = "occlusion_factor", .Default = 1.0f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Ambient occlusion with 0 = completely occluded and 1 = lit. Only affects the ambient light. "
                       "Multiplied with the red channel of the arm texture.",
        .bOpenPbr = false,
        };
    f32 OcclusionFactor{ OcclusionFactorInfo.Default };

    static constexpr TPbrParameter<f32> AlphaCutoffInfo{
        .Key = "alpha_cutoff", .Default = 0.5f, .Min = 0.0f, .Max = 1.0f, .NormMin = 0.0f, .NormMax = 1.0f,
        .Description = "Alpha masked materials discard where their coverage is below this.",
        .bOpenPbr = false,
        };
    f32 AlphaCutoff{ AlphaCutoffInfo.Default };

    static constexpr TPbrParameter<LVec2F> UVScaleInfo{
        .Key = "uv_scale", .Default = {1.0f, 1.0f}, .Min = -PbrUnbounded, .Max = PbrUnbounded, .NormMin = 0.0f, .NormMax = 8.0f,
        .Description = "Multiplied with the texture coordinates of the mesh.",
        .bOpenPbr = false,
        };
    LVec2F UVScale{ UVScaleInfo.Default };

    static constexpr TPbrParameter<LVec2F> UVOffsetInfo{
        .Key = "uv_offset", .Default = {0.0f, 0.0f}, .Min = -PbrUnbounded, .Max = PbrUnbounded, .NormMin = -1.0f, .NormMax = 1.0f,
        .Description = "Added to the scaled texture coordinates of the mesh.",
        .bOpenPbr = false,
        };
    LVec2F UVOffset{ UVOffsetInfo.Default };

    //# The sampler of all textures. Key: "sampler".
    UBO::Bindless::Sampler Sampler{ UBO::Bindless::Sampler::LinearRepeat };

    //# Key: "flags", e.g., "AlphaMask|DoubleSided".
    EPbrMaterialFlags Flags{ EPbrMaterialBits::Identity };

    NODISCARD bool operator==(LPbrMaterialParameters const&) const = default;

    //#
    //# Calls the visitor with (Info, Field) for every parameter that has a #TPbrParameter. Every info is a
    //# TPbrParameter<T> const& and every field a T& (T const& for a const self).
    //#
    template<typename TSelf, typename TVisitor>
    static constexpr void ForEachParameter(TSelf& Self, TVisitor&& Visitor)
    {
        Visitor(BaseWeightInfo, Self.BaseWeight);
        Visitor(BaseColorInfo, Self.BaseColor);
        Visitor(BaseMetalnessInfo, Self.BaseMetalness);
        Visitor(BaseDiffuseRoughnessInfo, Self.BaseDiffuseRoughness);
        Visitor(SpecularWeightInfo, Self.SpecularWeight);
        Visitor(SpecularColorInfo, Self.SpecularColor);
        Visitor(SpecularRoughnessInfo, Self.SpecularRoughness);
        Visitor(SpecularRoughnessAnisotropyInfo, Self.SpecularRoughnessAnisotropy);
        Visitor(SpecularIorInfo, Self.SpecularIor);
        Visitor(TransmissionWeightInfo, Self.TransmissionWeight);
        Visitor(TransmissionColorInfo, Self.TransmissionColor);
        Visitor(TransmissionDepthInfo, Self.TransmissionDepth);
        Visitor(TransmissionScatterInfo, Self.TransmissionScatter);
        Visitor(TransmissionScatterAnisotropyInfo, Self.TransmissionScatterAnisotropy);
        Visitor(TransmissionDispersionScaleInfo, Self.TransmissionDispersionScale);
        Visitor(TransmissionDispersionAbbeNumberInfo, Self.TransmissionDispersionAbbeNumber);
        Visitor(SubsurfaceWeightInfo, Self.SubsurfaceWeight);
        Visitor(SubsurfaceColorInfo, Self.SubsurfaceColor);
        Visitor(SubsurfaceRadiusInfo, Self.SubsurfaceRadius);
        Visitor(SubsurfaceRadiusScaleInfo, Self.SubsurfaceRadiusScale);
        Visitor(SubsurfaceScatterAnisotropyInfo, Self.SubsurfaceScatterAnisotropy);
        Visitor(CoatWeightInfo, Self.CoatWeight);
        Visitor(CoatColorInfo, Self.CoatColor);
        Visitor(CoatRoughnessInfo, Self.CoatRoughness);
        Visitor(CoatRoughnessAnisotropyInfo, Self.CoatRoughnessAnisotropy);
        Visitor(CoatIorInfo, Self.CoatIor);
        Visitor(CoatDarkeningInfo, Self.CoatDarkening);
        Visitor(FuzzWeightInfo, Self.FuzzWeight);
        Visitor(FuzzColorInfo, Self.FuzzColor);
        Visitor(FuzzRoughnessInfo, Self.FuzzRoughness);
        Visitor(EmissionLuminanceInfo, Self.EmissionLuminance);
        Visitor(EmissionColorInfo, Self.EmissionColor);
        Visitor(ThinFilmWeightInfo, Self.ThinFilmWeight);
        Visitor(ThinFilmThicknessInfo, Self.ThinFilmThickness);
        Visitor(ThinFilmIorInfo, Self.ThinFilmIor);
        Visitor(GeometryOpacityInfo, Self.GeometryOpacity);
        Visitor(GeometryThinWalledInfo, Self.GeometryThinWalled);
        Visitor(NormalScaleInfo, Self.NormalScale);
        Visitor(CoatNormalScaleInfo, Self.CoatNormalScale);
        Visitor(GeometryTangentRotationInfo, Self.GeometryTangentRotation);
        Visitor(GeometryCoatTangentRotationInfo, Self.GeometryCoatTangentRotation);
        Visitor(OcclusionFactorInfo, Self.OcclusionFactor);
        Visitor(AlphaCutoffInfo, Self.AlphaCutoff);
        Visitor(UVScaleInfo, Self.UVScale);
        Visitor(UVOffsetInfo, Self.UVOffset);
    }

    //# Writes everything but the textures into the device data.
    ENGINE_API void ToDevice(SSBO::BindlessPbrMaterial& Data) const noexcept;
    NODISCARD ENGINE_API static LPbrMaterialParameters FromDevice(SSBO::BindlessPbrMaterial const& Data) noexcept;

    //#
    //# Whether drawables with these parameters are drawn translucent (after everything opaque, sorted back to front):
    //# If they let light through (transmission) or are not fully there (opacity, blended alpha).
    //#
    NODISCARD ENGINE_API bool IsTranslucent() const noexcept;
    //# Whether both faces are drawn.
    NODISCARD FORCEINLINE bool IsDoubleSided() const noexcept { return this->GeometryThinWalled || !!(this->Flags & EPbrMaterialBits::DoubleSided); }
};

//#
//# Pbr materials are just syntax sugar over the base pbr material and are optional but highly recommended not only
//# for convenience but also for performance and integration with existing Jafg subsystems and user preferences.
//#
//# The parameters (@see LPbrMaterialParameters) are read from *.pbr.json files by their keys, e.g.:
//#     { "base_color": [0.9, 0.1, 0.1], "specular_roughness": 0.2, "coat_weight": 1.0 }
//#
struct LPbrMaterialTemplate final : LPbrMaterialParameters
{
    LString Identifier;

    //#
    //# The actual underlying material to use as (namespace, name). Defaults to (Jafg, Materials/Pbr) if empty.
    //# If only the name is set, the namespace of this pbr material is used. The namespace is the identifier of a
    //# loaded plugin or "Jafg". @see LEngine::FindPluginInfo
    //# Only the default material is drawn double sided or translucent. Others are drawn as they are.
    //#
    std::pair<LString,LString> Material;

    //# Projections into the TextureBindings.
    enum struct ETextureType: u32
    {
        BaseColor,
        Normal,
        Arm,
        Emission,
        Specular,
        Anisotropy,
        ThinFilm,
        Subsurface,
        CoatNormal,
    };
    static constexpr std::array TextureTypes{
        ETextureType::BaseColor, ETextureType::Normal, ETextureType::Arm, ETextureType::Emission,
        ETextureType::Specular, ETextureType::Anisotropy, ETextureType::ThinFilm, ETextureType::Subsurface,
        ETextureType::CoatNormal,
        };
    //# Whether the color channels of the texture are stored in sRGB (else they are linear data). Alpha is always linear.
    NODISCARD static constexpr bool IsSrgbTexture(ETextureType Type) noexcept
    {
        return Type == ETextureType::BaseColor || Type == ETextureType::Emission
            || Type == ETextureType::Specular || Type == ETextureType::Subsurface;
    }

    //# All textures are optional.
    //# There are two ways to specify a path:
    //# 1. Relative to the current file: "../OtherTexture/Albedo_2k.png"
    //# 2. Relative to the containing content dir: "@Textures/OtherTexture/Albedo_2k.png"
    //#
    //# The keys in *.pbr.json files are the snake cased names of the members (e.g., "base_color", "coat_normal").
    //#
    struct TextureBindings final
    {
        //# rgb: base_color. a: geometry_opacity (if alpha masked or blended). Without any textures, a texture next to
        //# the template with its name is used (e.g., "Foo.png" for "Foo.pbr.json").
        LPath BaseColor;
        //# geometry_normal in tangent space.
        //# We use the OpenGl standard. Use #EPbrMaterialBits::DirectXNormals for DX standard.
        LPath Normal;
        //# r: Ambient occlusion. g: specular_roughness. b: base_metalness. a: Unused.
        LPath Arm;
        //# rgb: emission_color.
        LPath Emission;
        //# rgb: specular_color. a: specular_weight.
        LPath Specular;
        //# rg: Direction of the anisotropy in tangent space, mapped from [-1,1] to [0,1] (geometry_tangent).
        //# b: specular_roughness_anisotropy. Like KHR_materials_anisotropy.
        LPath Anisotropy;
        //# r: thin_film_weight. g: thin_film_thickness.
        LPath ThinFilm;
        //# rgb: subsurface_color. a: subsurface_weight.
        LPath Subsurface;
        //# geometry_coat_normal in tangent space. Same standard as #Normal.
        LPath CoatNormal;

        NODISCARD constexpr LPath& Get(ETextureType Type) noexcept
        {
            switch (Type)
            {
            case ETextureType::BaseColor:  return this->BaseColor;
            case ETextureType::Normal:     return this->Normal;
            case ETextureType::Arm:        return this->Arm;
            case ETextureType::Emission:   return this->Emission;
            case ETextureType::Specular:   return this->Specular;
            case ETextureType::Anisotropy: return this->Anisotropy;
            case ETextureType::ThinFilm:   return this->ThinFilm;
            case ETextureType::Subsurface: return this->Subsurface;
            case ETextureType::CoatNormal: return this->CoatNormal;
            }
            std::unreachable();
        }
        NODISCARD constexpr LPath const& Get(ETextureType Type) const noexcept
        {
            return const_cast<TextureBindings*>(this)->Get(Type);
        }
    };
    //#
    //# Texture bindings for user quality preferences.
    //#
    //# By default everything falls through first and then elevates up if nothing was found.
    //#
    //# So lets say there is:
    //# - ETextureQualityBits::Potato       w/ BaseColor
    //# - ETextureQualityBits::Cinematic    w/ Normal
    //# If we are loading ETextureQualityBits::Standard:
    //# - BaseColor:
    //#     - 1. Search for BaseColor in Standard
    //#     - 2. Search for BaseColor in Mobile, ...
    //#     - 4. Search for BaseColor in Potato
    //#         -> Found (this texture will be used for the base color w/ ETextureQualityBits::Standard)
    //# - Normal:
    //#     - 1. Search for Normal in Standard
    //#     - 2. Search for Normal in Mobile, ... (nothing found until (inclusive) Potato)
    //#     - 5. Search for Normal in Ultra
    //#     - 6. Search for Normal in Cinematic
    //#         -> Found (this texture will be used for normal w/ ETextureQualityBits::Standard)
    //#
    TArray<std::pair<ETextureQualityFlags, TextureBindings>> Textures;
    //# Returns the texture for the given quality or an empty path if not found.
    NODISCARD ENGINE_API LPath const& GetTexture(ETextureType Type, ETextureQualityBits Quality) const noexcept;
    NODISCARD ENGINE_API bool IsEmptyAtTexture(ETextureType Type) const noexcept;
};
SERDE_JSON_TYPE_ONLY_DESERIALIZE_BODY(LPbrMaterialTemplate::TextureBindings)
{
    serde::parse_optional(j, "base_color", field.BaseColor);
    serde::parse_optional(j, "normal", field.Normal);
    serde::parse_optional(j, "arm", field.Arm);
    serde::parse_optional(j, "emission", field.Emission);
    serde::parse_optional(j, "specular", field.Specular);
    serde::parse_optional(j, "anisotropy", field.Anisotropy);
    serde::parse_optional(j, "thin_film", field.ThinFilm);
    serde::parse_optional(j, "subsurface", field.Subsurface);
    serde::parse_optional(j, "coat_normal", field.CoatNormal);
}

namespace SSBO
{

// TODO: Should this be a ubo with variable descriptor count instead?
//# @see Content/Shaders/include/Jafg/PbrMaterial.slang and LPbrMaterialParameters for the meaning of the members.
struct BindlessPbrMaterial: rhi::ssbo_template<BindlessPbrMaterial, vk::ShaderStageFlagBits::eFragment>
{
    NODISCARD static constexpr LStringView name() noexcept { return "Jafg::SSBO::PbrMaterials"; }

    static constexpr u32 MaterialBinding{ 0 };

    LVec3F BaseColor;
    f32    BaseWeight;
    f32    BaseMetalness;
    f32    BaseDiffuseRoughness;
    f32    SpecularWeight;
    f32    SpecularRoughness;

    LVec3F SpecularColor;
    f32    SpecularIor;
    f32    SpecularRoughnessAnisotropy;
    f32    GeometryTangentRotation;
    f32    OcclusionFactor;
    f32    GeometryOpacity;

    LVec3F TransmissionColor;
    f32    TransmissionWeight;
    LVec3F TransmissionScatter;
    f32    TransmissionDepth;
    f32    TransmissionScatterAnisotropy;
    f32    TransmissionDispersionScale;
    f32    TransmissionDispersionAbbeNumber;
    f32    SubsurfaceWeight;

    LVec3F SubsurfaceColor;
    f32    SubsurfaceRadius;
    LVec3F SubsurfaceRadiusScale;
    f32    SubsurfaceScatterAnisotropy;

    LVec3F CoatColor;
    f32    CoatWeight;
    f32    CoatRoughness;
    f32    CoatRoughnessAnisotropy;
    f32    CoatIor;
    f32    CoatDarkening;

    LVec3F FuzzColor;
    f32    FuzzWeight;
    LVec3F EmissionColor;
    f32    EmissionLuminance;

    f32 FuzzRoughness;
    f32 ThinFilmWeight;
    f32 ThinFilmThickness;
    f32 ThinFilmIor;

    f32 GeometryCoatTangentRotation;
    f32 AlphaCutoff;
    f32 NormalScale;
    f32 CoatNormalScale;

    LVec2F UVScale;
    LVec2F UVOffset;

    //# Bindless indices of the textures or #rhi::invalid_resource_index. @see LPbrMaterialTemplate::TextureBindings
    rhi::resource_index_t BaseColorTexture;
    rhi::resource_index_t NormalTexture;
    rhi::resource_index_t ArmTexture;
    rhi::resource_index_t EmissionTexture;
    rhi::resource_index_t SpecularTexture;
    rhi::resource_index_t AnisotropyTexture;
    rhi::resource_index_t ThinFilmTexture;
    rhi::resource_index_t SubsurfaceTexture;
    rhi::resource_index_t CoatNormalTexture;

    UBO::Bindless::Sampler Sampler;
    EPbrMaterialFlags      Flags;
    u32                    GeometryThinWalled;

    NODISCARD constexpr rhi::resource_index_t& GetTexture(LPbrMaterialTemplate::ETextureType Type) noexcept
    {
        switch (Type)
        {
        case LPbrMaterialTemplate::ETextureType::BaseColor:  return this->BaseColorTexture;
        case LPbrMaterialTemplate::ETextureType::Normal:     return this->NormalTexture;
        case LPbrMaterialTemplate::ETextureType::Arm:        return this->ArmTexture;
        case LPbrMaterialTemplate::ETextureType::Emission:   return this->EmissionTexture;
        case LPbrMaterialTemplate::ETextureType::Specular:   return this->SpecularTexture;
        case LPbrMaterialTemplate::ETextureType::Anisotropy: return this->AnisotropyTexture;
        case LPbrMaterialTemplate::ETextureType::ThinFilm:   return this->ThinFilmTexture;
        case LPbrMaterialTemplate::ETextureType::Subsurface: return this->SubsurfaceTexture;
        case LPbrMaterialTemplate::ETextureType::CoatNormal: return this->CoatNormalTexture;
        }
        std::unreachable();
    }
    NODISCARD constexpr rhi::resource_index_t GetTexture(LPbrMaterialTemplate::ETextureType Type) const noexcept
    {
        return const_cast<BindlessPbrMaterial*>(this)->GetTexture(Type);
    }

    static std::array<vk::DescriptorSetLayoutBinding, 1> bindings() noexcept
    {
        return std::array{
            vk::DescriptorSetLayoutBinding{
                .binding = MaterialBinding,
                .descriptorType = vk::DescriptorType::eStorageBuffer,
                .descriptorCount = 1,
                .stageFlags = shader_stage_flags(),
                .pImmutableSamplers = nullptr
                },
            };
    }
};
static_assert(sizeof(BindlessPbrMaterial) % 16 == 0); // Required.
static_assert(sizeof(BindlessPbrMaterial) == 304, "Is the shader also modified?");
static_assert(offsetof(BindlessPbrMaterial, SpecularColor) == 32, "Is the shader also modified?");
static_assert(offsetof(BindlessPbrMaterial, TransmissionColor) == 64, "Is the shader also modified?");
static_assert(offsetof(BindlessPbrMaterial, TransmissionScatter) == 80, "Is the shader also modified?");
static_assert(offsetof(BindlessPbrMaterial, SubsurfaceColor) == 112, "Is the shader also modified?");
static_assert(offsetof(BindlessPbrMaterial, SubsurfaceRadiusScale) == 128, "Is the shader also modified?");
static_assert(offsetof(BindlessPbrMaterial, CoatColor) == 144, "Is the shader also modified?");
static_assert(offsetof(BindlessPbrMaterial, FuzzColor) == 176, "Is the shader also modified?");
static_assert(offsetof(BindlessPbrMaterial, EmissionColor) == 192, "Is the shader also modified?");
static_assert(offsetof(BindlessPbrMaterial, UVScale) == 240, "Is the shader also modified?");
static_assert(offsetof(BindlessPbrMaterial, BaseColorTexture) == 256, "Is the shader also modified?");
static_assert(offsetof(BindlessPbrMaterial, Sampler) == 292, "Is the shader also modified?");
static_assert(rhi::ssbo<BindlessPbrMaterial>);

enum struct ELightFlagBits: u32 /* stable across slang. */
{
    Identity = 0x0 << 0,
};
ENUM_STRUCT_FLAGS(ELightFlagBits, ELightFlags)

//# @see Content/Shaders/include/Jafg/Pbr.slang
struct BindlessPbrLight: rhi::ssbo_template<BindlessPbrLight, vk::ShaderStageFlagBits::eFragment>
{
    NODISCARD static constexpr LStringView name() noexcept { return "Jafg::SSBO::PbrLights"; }

    static constexpr u32 LightBinding{ 0 };

    LVec3F Translation;
    f32 Range;

    LVec3F Direction;
    ELight Type;

    LVec3F Tint;
    f32 Intensity;

    LVec3F Right;
    f32 Width;

    f32 Height;
    f32 SpotScale;
    f32 SpotOffset;

    ELightFlags Flags;

    f32 SourceRadius;
    f32 SpecularScale;
    //# Index of the first shadow view (see SSBO::PbrShadowView) or #rhi::invalid_resource_index if unshadowed.
    rhi::resource_index_t Shadow{ rhi::invalid_resource_index };
    //# Number of shadow views starting at #Shadow. Their meaning depends on the type, see SSBO::PbrShadowView.
    u32 ShadowCount{};

    //# The shadow views of the lights share the set of the lights. @see Rhi/Shadow.h
    static constexpr u32 ShadowViewBinding{ 1 };

    static std::array<vk::DescriptorBindingFlags, 2> const& descriptor_binding_flags() noexcept
    {
        static std::array Flags{vk::DescriptorBindingFlags{}, vk::DescriptorBindingFlags{},};
        return Flags;
    }
    static std::array<vk::DescriptorSetLayoutBinding, 2> bindings() noexcept
    {
        return std::array{
            vk::DescriptorSetLayoutBinding{
                .binding = LightBinding,
                .descriptorType = vk::DescriptorType::eStorageBuffer,
                .descriptorCount = 1,
                .stageFlags = shader_stage_flags(),
                .pImmutableSamplers = nullptr
                },
            vk::DescriptorSetLayoutBinding{
                .binding = ShadowViewBinding,
                .descriptorType = vk::DescriptorType::eStorageBuffer,
                .descriptorCount = 1,
                .stageFlags = shader_stage_flags(),
                .pImmutableSamplers = nullptr
                },
            };
    }
};
static_assert(sizeof(BindlessPbrLight) % 16 == 0); // Required.
static_assert(sizeof(BindlessPbrLight) == 96, "Is the shader also modified?");
static_assert(rhi::ssbo<BindlessPbrLight>);

} /* ~Namespace SSBO */

namespace PC
{

//# @see Content/Shaders/include/Jafg/Pbr.slang
struct Pbr final: rhi::pc_template<Pbr, vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment>
{
    LMat4F Model;
    rhi::resource_index_t Material;

    static void PushForActor(LActorRenderInfo const& Info, LMaterial const& Material, LPushConstantProvider::LActorInfo const& ActorInfo) noexcept
    {
        Pbr{
            .Model = maths::model(ActorInfo.Transform),
            .Material = ActorInfo.PbrMaterial,
            }.upload(Info.CommandBuffer, Material.Pipeline);
    }
};
static_assert(rhi::pc<Pbr>);

//#
//# The matrices of a mesh and the pbr material it is drawn with otherwise, if any. For passes that draw everything with
//# one material but still respect the coverage of the pbr materials, e.g., shadows.
//# @see Content/Shaders/include/Jafg/MeshPbr.slang
//#
struct MeshPbr final: rhi::pc_template<MeshPbr, vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment>
{
    LMat4F Model;
    LMat4F View;
    LMat4F Proj;
    //# Or #rhi::invalid_resource_index.
    rhi::resource_index_t Material;

    static void PushForActor(LActorRenderInfo const& Info, LMaterial const& Material, LPushConstantProvider::LActorInfo const& ActorInfo) noexcept
    {
        MeshPbr{
            .Model = maths::model(ActorInfo.Transform),
            .View = Info.WorldData.View,
            .Proj = Info.WorldData.Proj,
            .Material = ActorInfo.PbrMaterial,
            }.upload(Info.CommandBuffer, Material.Pipeline);
    }
};
static_assert(rhi::pc<MeshPbr>);

} /* ~Namespace PC */

//# How to draw with a pbr material instance. @see LPbrMaterialInstance::GetDrawPlan
struct LPbrDrawPlan final
{
    //# Drawn first, if any. The back faces of double sided translucent materials.
    LMaterialInstance const* Back{};
    LMaterialInstance const* Front{};
    //# Drawn after everything opaque, sorted back to front.
    bool bTranslucent{};
    //# Whether the draw needs the thickness of the drawable, which comes from its back faces on screen.
    bool bThickness{};
};

//# The instance of a pbr material loaded from a template.
struct LPbrMaterialInstance final
{
    friend JMaterialSubsystem;

    FORCEINLINE explicit LPbrMaterialInstance(LMaterialInstanceRef Instance, LPbrMaterialTemplate const& Template) noexcept
        : Instance{std::move(Instance)}, Template{Template}
    {
        check(this->Instance)
    }
    FORCEINLINE ~LPbrMaterialInstance() noexcept { this->FreeFromBindlessArray(nullptr); }

    //# The underlying material. Opaque and single sided if it is the default one.
    LMaterialInstanceRef Instance;
    LPbrMaterialTemplate const& Template;

    NODISCARD FORCEINLINE constexpr SSBO::BindlessPbrMaterial const& GetData() const noexcept { return this->Data; }
    NODISCARD FORCEINLINE constexpr SSBO::BindlessPbrMaterial& MakeDirty() noexcept
    {
        this->Dirty |= m_MakeDirty;
        return this->Data;
    }

    //#
    //# Which materials to draw with for the current data. Follows edits of the data right away. Underlying materials
    //# other than the default one are always drawn as they are.
    //#
    NODISCARD ENGINE_API LPbrDrawPlan GetDrawPlan() const noexcept;

    //#
    //# For pbr materials only. Else can be ignored.
    //# TODO: This is not clean. Can we change this?
    //#
    NODISCARD FORCEINLINE constexpr bool IsBindless() const noexcept { return this->Index != rhi::invalid_resource_index; }
    NODISCARD FORCEINLINE constexpr rhi::resource_index_t GetBindlessIndex() const noexcept
    {
        check(this->Index != rhi::invalid_resource_index)
        return this->Index;
    }

    //# Cached subsystem or nullptr if not cached.
    ENGINE_API void FreeFromBindlessArray(JMaterialSubsystem* CachedSubsystem);

private:

    //# Data is not initialized and does not reflect the data on the device.
    SSBO::BindlessPbrMaterial Data;

    static_assert(rhi::max_frames_in_flight <= 8);
    static constexpr u8 m_MakeDirty{ static_cast<u8>((1uz << rhi::max_frames_in_flight) - 1uz) };
    //# Bitmask of dirty frames. Each bit represents a frame in flight.
    u8 Dirty{ m_MakeDirty };

    //# The bindless index that this instance is bound to.
    rhi::resource_index_t Index{ rhi::invalid_resource_index };
    //# Owned textures for this instance. Do not modify. Internal to Jafg.
    TArray<LTexture2Ref> LoadedTextures;

    //# The variants of the default underlying material. Null for every other underlying material.
    LMaterialInstanceRef DoubleSided;
    LMaterialInstanceRef Translucent;
    LMaterialInstanceRef TranslucentBackFaces;
};

} /* ~Namespace Jafg */
