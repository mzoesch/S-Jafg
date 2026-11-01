// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/FrontendSubsystem.h"
#include "Rhi/Material.h"
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

    NODISCARD LString GetKey(LStringView Namespace, LStringView Instance) const noexcept
    {
        return algo::sprintf("{}/{}", Namespace, Instance);
    }

    //# All materials templates known to this subsystem.
    NODISCARD FORCEINLINE constexpr auto const& GetTemplates() const noexcept { return this->Templates; }
    //# All materials allocated by this subsystem.
    NODISCARD FORCEINLINE constexpr auto const& GetMaterials() const noexcept { return this->Materials; }

    //#
    //# All instances that a client has set to share globally.
    //# By default instances are not shared. But they can be easily registered here to be shared across many
    //# objects and draw calls.
    //#
    NODISCARD FORCEINLINE constexpr auto const& GetSharedInstances() const noexcept { return this->Instances; }
    FORCEINLINE void RegisterSharedInstance(LStringView Key, LMaterialInstanceRef Instance) noexcept
    {
        if (this->Instances.contains(Key))
        {
            LOG_FATAL(LogMaterialSubsystem, "[{}]: Material instance with this key already exists.", Key)
        }
        this->Instances.emplace(std::move(Key), std::move(Instance));
    }
    FORCEINLINE void RegisterSharedInstance(LStringView Namespace, LStringView Name, LMaterialInstanceRef Instance) noexcept
    {
        this->RegisterSharedInstance(this->GetKey(Namespace, Name), std::move(Instance));
    }
    NODISCARD std::optional<LMaterialInstanceRef> FindSharedInstance(LStringView Key) noexcept
    {
        if (auto It{this->Instances.find(Key)}; It != this->Instances.end())
        {
            return It->second;
        }
        return {};
    }
    NODISCARD std::optional<LMaterialInstanceRef> FindSharedInstance(LStringView Namespace, LStringView Name) noexcept
    {
        return this->FindSharedInstance(this->GetKey(Namespace, Name));
    }
    NODISCARD LMaterialInstanceRef FindOrAllocateSharedInstance(LStringView Namespace, LStringView Name) noexcept
    {
        LString Key{this->GetKey(Namespace, Name)};
        if (auto Instance{this->FindSharedInstance(Key)})
        {
            return *Instance;
        }
        auto Result{this->GetInstanceFromName(Namespace, Name)};
        this->RegisterSharedInstance(Key, Result);
        return Result;
    }

    //# Get a material that is device ready.
    NODISCARD LMaterialRef GetMaterial(LStringView Namespace, LStringView Name) noexcept;
    //#
    //# Get the instance of a material to specify unique data. Completely optional. You can use materials just
    //# as is without any instance.
    //#
    NODISCARD LMaterialInstanceRef GetInstance(LStringView Namespace, LStringView Name, LMaterialRef Material);
    NODISCARD LMaterialInstanceRef GetInstanceFromName(LStringView Namespace, LStringView Name);

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

private:

    void RetrieveMaterialsFrom(LPath const& Path);

    JTextureSubsystem* TextureSubsystem{};
    JShaderSubsystem* ShaderSubsystem{};

    //# Once populated, materials will never change their address again. This is guaranteed by jafg.
    algo::transparent_unordered_string_map<TArray<rhi::material_template>> Templates;
    algo::transparent_unordered_string_map<TArray<LPbrMaterial>> PbrMaterials;

    algo::transparent_unordered_string_map<LMaterialRef> Materials;
    algo::transparent_unordered_string_map<LMaterialInstanceRef> Instances;
    NODISCARD rhi::material_template const& GetMaterialTemplate(LStringView Namespace, LStringView Identifier) noexcept
    {
        LString Key{this->GetKey(Namespace, Identifier)};
        if (!this->Templates.contains(Namespace))
        {
            LOG_FATAL(LogMaterialSubsystem, "[{}]: No such namespace.", Namespace)
        }
        auto& SearchSpace{this->Templates.at(Namespace)};
        auto It{algo::find(SearchSpace, Identifier, &rhi::material_template::identifier)};
        if (It == SearchSpace.end())
        {
            LOG_FATAL(LogMaterialSubsystem, "[{}]: No such material.", Key)
        }
        return *It;
    }

    algo::transparent_unordered_string_map<vk::raii::DescriptorSetLayout> SharedDescriptorSetLayouts;
};

} /* ~Namespace Jafg */
