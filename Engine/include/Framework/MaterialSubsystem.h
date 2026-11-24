// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/FrontendSubsystem.h"
#include "Rhi/Material.h"
#include "Rhi/PhysicalRendering.h"
#include "MaterialSubsystem.generated.h"

namespace Jafg
{

class JShaderSubsystem;
class JTextureSubsystem;
struct LTexture2;

//#
//# The material system of Jafg.
//#
//# In a content directory you may declare a material with .*\.material\.json having the following structure:
//# {
//#   "inherits": ("material"|["material1","material2",...]),
//#   "shader": "shader",
//#   "pipelineInputAssemblyState": ...
//#   "pipelineRasterizationState": ...
//#   // "pipelineMultisampleState": ... (no longer supported.)
//#   "pipelineDepthStencilState": ...
//#   "pipelineColorBlendState": ...
//#   "properties": {
//#     "Key1": "Value1",
//#     "Key2": "Value2",
//#     ...,
//#   }
//# }
//# Each state must exactly represent the Vulkan spec.
//#
//# Everything is optional. States, properties, etc. in the derived material are always merged with the parent material
//# keeping the child's value in case of a conflict. Inheritance is done in order of the array. The first parent
//# has the highest priority.
//#
//# Materials that do not fulfill the requirements are kept as base materials to be inherited from. They are
//# not instanceable.
//#
//# Additionally there are some syntax sugars:
//# - Create pbr materials with either the above or by using .*pbr.json
//#
DECLARE_JAFG_CLASS()
class ENGINE_API JMaterialSubsystem final : public JFrontendSubsystem
{
    GENERATED_CLASS_BODY()

    friend struct LResolveInheritance;

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JMaterialSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override
    {
        this->PurgeUnused();
        Super::TearDown();
    }

public:

    //# Called automatically by Jafg. Usually you do not want to call this.
    void PurgeUnused();

    //# All materials templates known to this subsystem.
    NODISCARD FORCEINLINE constexpr auto const& GetTemplates() const noexcept { return this->Templates; }
    //# All pbr material templates known to this subsystem.
    NODISCARD FORCEINLINE constexpr auto const& GetPbrTemplates() const noexcept { return this->PbrTemplates; }
    //# All materials allocated by this subsystem.
    NODISCARD FORCEINLINE constexpr auto const& GetMaterials() const noexcept { return this->Materials; }

    //#
    //# All instances that a client has set to share globally.
    //# By default instances are not shared. But they can be easily registered here to be shared across many
    //# objects and draw calls.
    //#
    NODISCARD FORCEINLINE constexpr auto const& GetSharedInstances() const noexcept { return this->Instances; }
    void RegisterSharedInstance(LPluginInfo Namespace, LStringView Name, LMaterialInstanceRef Instance) noexcept;
    NODISCARD std::optional<LMaterialInstanceRef> FindSharedInstance(LPluginInfo Namespace, LStringView Name) const noexcept;
    NODISCARD LMaterialInstanceRef FindOrAllocateSharedInstance(LPluginInfo Namespace, LStringView Name) noexcept;

    //# Get a material that is device ready.
    NODISCARD LMaterialRef GetMaterial(LPluginInfo Namespace, LStringView Name) noexcept;
    //#
    //# Get the instance of a material to specify unique data. Completely optional. You can use materials just
    //# as is without any instance.
    //#
    NODISCARD LMaterialInstanceRef GetInstance(LMaterialRef Material);
    NODISCARD LMaterialInstanceRef GetInstanceFromName(LPluginInfo Namespace, LStringView Name);

    //# Get a bindless instance to a pbr material.
    NODISCARD LPbrMaterialInstanceRef GetPbrInstance(LPluginInfo Namespace, LStringView Name);

    //# The namespace of a pbr template of this subsystem. Null if it is not one of them.
    NODISCARD LPluginInfo FindPbrTemplateNamespace(LPbrMaterialTemplate const& Template) const noexcept;
    //#
    //# The texture that an instance of the template uses for the type with the current texture quality preference.
    //# Relative to the content dir of the namespace. Empty if there is none.
    //# @param Namespace The namespace of the template.
    //#
    NODISCARD LString ResolvePbrTemplateTexture(LPluginInfo Namespace, LPbrMaterialTemplate const& Template, LPbrMaterialTemplate::ETextureType Type) const;
    //#
    //# Loads a texture (or reuses it if already loaded) in the format that pbr materials expect for the type and makes
    //# it bindless.
    //# @param Path Relative to the content dir of the namespace.
    //#
    NODISCARD LTexture2Ref LoadPbrTexture(LPluginInfo Namespace, LStringView Path, LPbrMaterialTemplate::ETextureType Type);
    //# The texture that is bound to the type of the instance. Null if none.
    NODISCARD LTexture2 const* FindPbrTexture(LPbrMaterialInstance const& Instance, LPbrMaterialTemplate::ETextureType Type) const noexcept;
    //#
    //# Binds the texture to the type of the instance. Null unbinds. Everything that is drawn with the instance sees it
    //# from the next frame on.
    //# @param Texture Has to be bindless. @see #LoadPbrTexture
    //#
    void SetPbrTexture(LPbrMaterialInstance& Instance, LPbrMaterialTemplate::ETextureType Type, LTexture2Ref Texture);
    // NODISCARD std::pair<LPbrMaterialInstanceRef, LPbrMaterialTemplate const*> GetOnTheFlyPbrInstance(LPbrMaterialTemplate Template);

    //#
    //# Usually layouts are unique per pipeline. But some layouts are used across many shaders. You can find the here
    //# or register you own.
    //#
    NODISCARD auto const& GetSharedDescriptorSetLayouts() const noexcept { return this->SharedDescriptorSetLayouts; }
    template<rhi::detail::object T>
    NODISCARD vk::raii::DescriptorSetLayout const& GetSharedDescriptorSetLayout() const noexcept { return this->SharedDescriptorSetLayouts.at(T::name()); }
    vk::DescriptorSetLayout EmplaceSharedDescriptorSetLayout(LString Identifier, vk::raii::DescriptorSetLayout Layout) noexcept
    {
        auto [It, Emplaced]{this->SharedDescriptorSetLayouts.emplace(std::move(Identifier), std::move(Layout))};
        check(Emplaced)
        return *It->second;
    }

    void AddInstanceToGlobalBindlessArray(LPbrMaterialInstance* Instance);
    NODISCARD std::optional<rhi::resource_index_t> _Vk_AddInstanceToGlobalBindlessArray(LPbrMaterialInstance* Instance);
    void RemoveInstanceFromGlobalBindlessArray(LPbrMaterialInstance* Instance);

    NODISCARD FORCEINLINE TArray<LPbrMaterialInstanceRef> const& GetBindlessPbrInstances() const noexcept { return this->PbrInstances; }

    ENGINE_API void UpdateMaterialsFor(std::size_t Sync);
    NODISCARD FORCEINLINE constexpr std::size_t GetBindlessPbrInstanceLimit() const noexcept { return this->PbrInstanceLimit; }
    NODISCARD FORCEINLINE constexpr rhi::mapped_device_buffer const& GetBindlessPbrInstanceBuffer(std::size_t Frame) const noexcept { return this->PbrInstanceBuffers[Frame]; }
    NODISCARD FORCEINLINE constexpr rhi::frame_array<rhi::mapped_device_buffer> const& GetBindlessPbrInstanceBuffers() const noexcept { return this->PbrInstanceBuffers; }
    NODISCARD FORCEINLINE constexpr vk::raii::DescriptorSet const& GetBindlessPbrInstanceDescriptorSet(std::size_t Frame) const noexcept { return this->PbrInstanceDescriptorSets[Frame]; }
    NODISCARD FORCEINLINE constexpr rhi::frame_array<vk::raii::DescriptorSet> const& GetBindlessPbrInstanceDescriptorSets() const noexcept { return this->PbrInstanceDescriptorSets; }

private:

    //# Noop, if the materials of the namespace are already known (e.g., shared with another plugin or loaded again).
    void RetrieveMaterialsFrom(LPluginInfo Namespace);

    JTextureSubsystem* TextureSubsystem{};
    JShaderSubsystem* ShaderSubsystem{};

    //# Once populated, templates will never change their address again. This is guaranteed by jafg.
    std::unordered_map<LPluginInfo, TArray<rhi::material_template>> Templates;
    std::unordered_map<LPluginInfo, TArray<LPbrMaterialTemplate>> PbrTemplates;

    //# Per namespace: The name of the material to the material.
    std::unordered_map<LPluginInfo, algo::transparent_unordered_string_map<LMaterialRef>> Materials;
    //# Per namespace: The name of the shared instance to the instance.
    std::unordered_map<LPluginInfo, algo::transparent_unordered_string_map<LMaterialInstanceRef>> Instances;
    NODISCARD rhi::material_template const& GetMaterialTemplate(LPluginInfo Namespace, LStringView Identifier) const noexcept;

    algo::transparent_unordered_string_map<vk::raii::DescriptorSetLayout> SharedDescriptorSetLayouts;

    void PrepareBindless();
    TArray<LPbrMaterialInstanceRef> PbrInstances;
    algo::dynamic_bit_set FreeBindlessPbrMaterialArraySlots;

    std::size_t PbrInstanceLimit{ 256uz }; // TODO: Make dynamic.
    rhi::frame_array<rhi::mapped_device_buffer> PbrInstanceBuffers;
    rhi::frame_array<vk::raii::DescriptorSet> PbrInstanceDescriptorSets JAFG_VK_FRAME_ARRAY_INIT(nullptr);
};

} /* ~Namespace Jafg */
