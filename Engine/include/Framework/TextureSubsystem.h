// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/FrontendSubsystem.h"
#include "Rhi/TextureView.h"
#include "Rhi/Texture2.h"
#include "Rhi/TextureCube.h"
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

    virtual void Initialize(LSubsystemCollection& Collection) override;

public:

    //# Removes all loaded textures that are not referenced anymore.
    void PurgeUnused() noexcept;

    //# All texture views known to this subsystem.
    NODISCARD FORCEINLINE constexpr auto const& GetTextureViews() const noexcept { return this->TextureViews; }
    NODISCARD LTextureView const& GetTextureView(LPluginInfo Namespace, LStringView Identifier) const noexcept;

    NODISCARD std::size_t GetLoadedTextureCount() const noexcept;
    NODISCARD FORCEINLINE constexpr auto const& GetTextures() const noexcept { return this->Textures; }

    //#
    //# Loads a texture file. Or reuses it, if already loaded.
    //# @param Path Relative to the content dir of the namespace.
    //#
    NODISCARD LTexture2Ref FromFile(LPluginInfo Namespace, LStringView Path
        , LTexture2::HostInfo HostCreateInfo
        , LTexture2::DeviceInfo DeviceCreateInfo
        , ETexture2State State = ETexture2StateBits::Device
        );
    //# Loads the texture of a texture view. Or reuses it, if already loaded.
    NODISCARD LTexture2Ref FromTextureView(LPluginInfo Namespace, LStringView Name
        , ETexture2State State = ETexture2StateBits::Device
        );

    //#
    //# Loads a cube map. Or reuses it, if already loaded.
    //# @param Path Relative to the content dir of the namespace.
    //#
    LTextureCube2Ref CubeFromFile(LPluginInfo Namespace, LStringView Path);

    NODISCARD FORCEINLINE std::size_t GetBindlessTextureArrayCapacity() const noexcept { return BindlessTextureArrayCapacity; }
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
    NODISCARD std::optional<rhi::resource_index_t> _Vk_AddTransientImageToGlobalBindlessArray(vk::ImageView const& ImageView);
    //# Public private function of jafg. Do not use.
    void _Vk_AllocateBindlessPool();
    NODISCARD FORCEINLINE auto const& Vk_GetBindlessTextureArrayDescriptorPool() const noexcept { return this->BindlessTextureArrayDescriptorPool; }
    NODISCARD FORCEINLINE auto const& Vk_GetBindlessTextureArrayDescriptorSet() const noexcept { return this->BindlessTextureArrayDescriptorSet; }

private:

    //# Noop, if the views of the namespace are already known (e.g., shared with another plugin or loaded again).
    void RetrieveTextureViewsFrom(LPluginInfo Namespace);
    //#
    //# Shared by #FromFile and #FromTextureView.
    //# @param MakePath Returns the absolute path to the texture file. Only called if not cached.
    //#
    template<typename TMakePath>
    NODISCARD LTexture2Ref FindOrLoad(LPluginInfo Namespace, LStringView Key, TMakePath&& MakePath
        , LTexture2::HostInfo HostCreateInfo
        , LTexture2::DeviceInfo DeviceCreateInfo
        , ETexture2State State
        );

    inline constexpr static auto BindlessTextureArrayCapacity{ 128uz };
    std::array<vk::raii::Sampler, UBO::Bindless::SamplerCount> Vk_DefaultSamplers JAFG_INIT_EIGHT(nullptr);
    vk::raii::DescriptorPool BindlessTextureArrayDescriptorPool{ nullptr };
    vk::raii::DescriptorSet BindlessTextureArrayDescriptorSet{ nullptr };
    algo::dynamic_bit_set FreeBindlessTextureArraySlots;

    //# Once populated, texture views will never change their address again. This is guaranteed by jafg.
    std::unordered_map<LPluginInfo, TArray<LTextureView>> TextureViews;

    TArray<LTexture2Ref> GuaranteedTextures;
    //# Per namespace: The identifier of the texture view or the file path (relative to the content dir) to the texture.
    std::unordered_map<LPluginInfo, algo::transparent_unordered_string_map<LTexture2Ref>> Textures;
    //# Per namespace: The file path (relative to the content dir) to the cube texture.
    std::unordered_map<LPluginInfo, algo::transparent_unordered_string_map<LTextureCube2Ref>> CubeTextures;
};

} /* ~Namespace Jafg */
