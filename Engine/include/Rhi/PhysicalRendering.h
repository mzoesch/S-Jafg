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

enum struct EPbrMaterialBits: u32 /* Stable across shader barrier. */
{
    Identity = 0x0 << 0,
    //# Discard below alpha cutoff.
    AlphaMask = 0x1 << 0,
    //# Flip normal on back faces.
    //# @note Your underlying material has to be able to handle this.
    //#       This is not part of the default Jafg pbr pipeline.
    DoubleSided = 0x1 << 1,
    //# Use direct x normal map format. Jafg uses by default the OpenGl standard.
    //# Y-Coords of normals will be flipped.
    DirectXNormals = 0x1 << 2,
};
SERDE_ENUM_STRUCT_FLAGS(EPbrMaterialBits, EPbrMaterialFlags)
SERDE_JSON_ENUM(EPbrMaterialBits, Identity, AlphaMask, DoubleSided, DirectXNormals)

//#
//# Pbr materials are just syntax sugar over the base pbr material and are optional but highly recommended not only
//# for convenience but also for performance and integration with existing Jafg subsystems and user preferences.
//#
struct LPbrMaterialTemplate final
{
    LString Identifier;

    //# The actual underlying material to use. Defaults to (Jafg, Materials/Pbr) if empty.
    std::pair<LString,LString> Material;
    //# TODO: Prob. some enum with translucent, transparent, etc. shaders.

    //# Factors are multiplied with the sampled texture values if bound
    //# else the factors are used directly as a base.
    LLinearColor AlbedoFactor{ LinearColors::White };
    //# Range [0,1] with 0 = completely occluded and 1 = lit.
    f32          OcclusionFactor{ 1.0f };
    //# Range [0,1] with 0 = smooth and 1 = rough.
    f32          RoughnessFactor{ 0.5f };
    //# Range [0,1] with 0 = dielectric and 1 = metallic.
    f32          MetallicFactor{};
    //# The color of the emission.
    LVec3F       EmissiveFactor{ LinearColors::Black.ToDeviceReadyVector3() };

    //# The alpha cutoff value for alpha masking. Range [0,1] with 0 = fully transparent and 1 = fully opaque.
    f32 AlphaCutoff{ 0.5f };
    //# The normal scale for normal mapping. Range [0,inf] with 0 = no normal mapping and 1 = full normal mapping.
    f32 NormalScale{ 1.0 };

    //# Projections into the TextureBindings.
    enum struct ETextureType: u32
    {
        Albedo,
        Normal,
        Arm,
        Emissive,
    };

    //# All textures are optional.
    struct TextureBindings final
    {
        //# The base color map.
        LPath Albedo;
        //# The normal map.
        //# We use the OpenGl standard. Use #EPbrMaterialBits::DirectXNormals for DX standard.
        LPath Normal;
        //# The arm map.
        //# - R channel: AO
        //# - G channel: Roughness
        //# - B channel: Metallic
        //# - A channel: Unused
        LPath Arm;
        //# The emissive map.
        LPath Emissive;
    };
    //#
    //# Texture bindings for user quality preferences.
    //#
    //# By default everything falls through first and then elevates up if nothing was found.
    //#
    //# So lets say there is:
    //# - ETextureQualityBits::Potato       w/ Albedo
    //# - ETextureQualityBits::Cinematic    w/ Normal
    //# If we are loading ETextureQualityBits::Standard:
    //# - Albedo:
    //#     - 1. Search for Albedo in Standard
    //#     - 2. Search for Albedo in Mobile, ...
    //#     - 4. Search for Albedo in Potato
    //#         -> Found (this texture will be used for albedo w/ ETextureQualityBits::Standard)
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
    //# The sampler to use.
    UBO::Bindless::Sampler Sampler{ UBO::Bindless::Sampler::LinearRepeat };

    EPbrMaterialFlags Flags{ EPbrMaterialBits::Identity };

    //# The UV scale and offset for the material. Defaults to (1,1) and (0,0).
    LVec2F UVScale{ maths::one_vector<LVec2F> };
    LVec2F UVOffset{ maths::zero_vector<LVec2F> };
};
SERDE_JSON_TYPE_ONLY_DESERIALIZE(LPbrMaterialTemplate::TextureBindings, Albedo, Normal, Arm, Emissive)

namespace SSBO
{

// TODO: Should this be a ubo with variable descriptor count instead?
//# @see Content/Shaders/include/Jafg/Pbr.slang
struct BindlessPbrMaterial: rhi::ssbo_template<BindlessPbrMaterial, vk::ShaderStageFlagBits::eFragment>
{
    NODISCARD static constexpr LStringView name() noexcept { return "Jafg::SSBO::PbrMaterials"; }

    static constexpr u32 MaterialBinding{ 0 };

    LVec4F AlbedoFactor;
    f32    OcclusionFactor;
    f32    RoughnessFactor;
    f32    MetallicFactor;
    u32    _pad0;
    LVec3F EmissiveFactor;

    f32 AlphaCutoff;
    f32 NormalScale;

    rhi::resource_index_t  Albedo;
    rhi::resource_index_t  Normal;
    rhi::resource_index_t  Arm;
    rhi::resource_index_t  Emissive;
    UBO::Bindless::Sampler Sampler;
    EPbrMaterialFlags      Flags;

    u32 _pad1;

    LVec2F UVScale;
    LVec2F UVOffset;

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
static_assert(sizeof(BindlessPbrMaterial) == 96, "Is the shader also modified?");
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

} /* ~Namespace PC */

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

    LMaterialInstanceRef Instance;
    LPbrMaterialTemplate const& Template;

    NODISCARD FORCEINLINE constexpr SSBO::BindlessPbrMaterial const& GetData() const noexcept { return this->Data; }
    NODISCARD FORCEINLINE constexpr SSBO::BindlessPbrMaterial& MakeDirty() noexcept
    {
        this->Dirty |= m_MakeDirty;
        return this->Data;
    }

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
};

} /* ~Namespace Jafg */
