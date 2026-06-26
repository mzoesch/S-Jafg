// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/FrontendSubsystem.h"
#include "Rhi/TextureView.h"
#include "Rhi/Texture2.h"
#include "Rhi/Bindless.h"
#include "TextureSubsystem.generated.h"

namespace Jafg
{

struct LTextureView;

//# A texture that was loaded by the program and may be used across many different widgets.
DECLARE_JAFG_CLASS()
class ENGINE_API JTextureSubsystem : public JFrontendSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JTextureSubsystem)

public:

    virtual void Initialize(LSubsystemCollection& Collection) override;

    //# Removes all loaded textures that are not referenced anymore.
    void PurgeUnused() noexcept;

    LTextureView const& GetTextureView(LStringView Asset) const noexcept;
    void RefetchTextureViews();

    FORCEINLINE auto GetLoadedTextureCount() const noexcept { return this->Textures.size(); }
    FORCEINLINE auto const& GetTextures() const noexcept { return this->Textures; }

    LTexture2Ref FromFile(LPath const& Path
        , LTexture2::HostInfo HostCreateInfo
        , LTexture2::DeviceInfo DeviceCreateInfo
        , ETexture2State State = ETexture2StateBits::Device
        );

    LTexture2Ref FromAsset(LStringView Asset, ETexture2State State = ETexture2StateBits::Device);
    LTexture2Ref FromTextureView(LTextureView const& View, ETexture2State State = ETexture2StateBits::Device);

    NODISCARD FORCEINLINE u32 GetBindlessTextureArrayCapacity() const noexcept { return this->BindlessTextureArrayCapacity; }
    FORCEINLINE auto const& Vk_GetDefaultSamplers() const noexcept { return this->Vk_DefaultSamplers; }
    FORCEINLINE auto const& Vk_GetLinearSamplerRepeat() const noexcept { return this->Vk_DefaultSamplers[UBO::Bindless::LinearRepeatSamplerIdx]; }
    FORCEINLINE auto const& Vk_GetLinearSamplerMirroredRepeat() const noexcept { return this->Vk_DefaultSamplers[UBO::Bindless::LinearMirroredRepeatSamplerIdx]; }
    FORCEINLINE auto const& Vk_GetLinearSamplerClampToEdge() const noexcept { return this->Vk_DefaultSamplers[UBO::Bindless::LinearClampToEdgeSamplerIdx]; }
    FORCEINLINE auto const& Vk_GetLinearSamplerClampToBorder() const noexcept { return this->Vk_DefaultSamplers[UBO::Bindless::LinearClampToBorderSamplerIdx]; }
    // FORCEINLINE auto const& Vk_GetLinearSamplerMirrorClampToEdge() const noexcept { return this->Vk_DefaultSamplers[UBO::Bindless::LinearMirrorClampToEdgeSamplerIdx]; }
    FORCEINLINE auto const& Vk_GetNearestSamplerRepeat() const noexcept { return this->Vk_DefaultSamplers[UBO::Bindless::NearestRepeatSamplerIdx]; }
    FORCEINLINE auto const& Vk_GetNearestSamplerMirroredRepeat() const noexcept { return this->Vk_DefaultSamplers[UBO::Bindless::NearestMirroredRepeatSamplerIdx]; }
    FORCEINLINE auto const& Vk_GetNearestSamplerClampToEdge() const noexcept { return this->Vk_DefaultSamplers[UBO::Bindless::NearestClampToEdgeSamplerIdx]; }
    FORCEINLINE auto const& Vk_GetNearestSamplerClampToBorder() const noexcept { return this->Vk_DefaultSamplers[UBO::Bindless::NearestClampToBorderSamplerIdx]; }
    // FORCEINLINE auto const& Vk_GetNearestSamplerMirrorClampToEdge() const noexcept { return this->Vk_DefaultSamplers[UBO::Bindless::NearestMirrorClampToEdgeSamplerIdx]; }
    //# Public private function of jafg. Do not use.
    void _Vk_UpdateSamplers();
    void AddTextureToGlobalBindlessArray(LTexture2* Texture);
    //# Public private function of jafg. Do not use.
    std::optional<std::size_t> _Vk_AddTransientImageToGlobalBindlessArray(vk::ImageView const& ImageView);
    //# Public private function of jafg. Do not use.
    void _Vk_AllocateBindlessPool();
    NODISCARD FORCEINLINE auto const& Vk_GetBindlessTextureArrayDescriptorPool() const noexcept { return this->BindlessTextureArrayDescriptorPool; }
    NODISCARD FORCEINLINE auto const& Vk_GetBindlessTextureArrayDescriptorSet() const noexcept { return this->BindlessTextureArrayDescriptorSet; }

private:

    u32 BindlessTextureArrayCapacity{ 128u };
    std::array<vk::raii::Sampler, UBO::Bindless::SamplerCount> Vk_DefaultSamplers JAFG_INIT_EIGHT(nullptr);
    vk::raii::DescriptorPool BindlessTextureArrayDescriptorPool{ nullptr };
    vk::raii::DescriptorSet BindlessTextureArrayDescriptorSet{ nullptr };
    algo::dynamic_bit_set FreeBindlessTextureArraySlots;

    TArray<LTextureView> TextureViews;
    TArray<LTexture2Ref> GuaranteedTextures;
    std::unordered_map<LPath, std::shared_ptr<LTexture2>> Textures;
};

} /* ~Namespace Jafg */
