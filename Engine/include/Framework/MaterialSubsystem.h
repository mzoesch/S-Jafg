// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/FrontendSubsystem.h"
#include "Rhi/Material.h"
#include "MaterialSubsystem.generated.h"

namespace Jafg
{

class JShaderSubsystem;
class JTextureSubsystem;
struct LTexture2;

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
    NODISCARD FORCEINLINE constexpr auto const& GetMaterialTemplates() const noexcept { return this->MaterialTemplates; }
    //# All materials allocated by this subsystem.
    NODISCARD FORCEINLINE constexpr auto const& GetMaterials() const noexcept { return this->Materials; }
    //#
    //# All instances that a client has set to share globally.
    //# By default instances are not shared. But they can be easily registered here to be shared across many
    //# objects and draw calls.
    //#
    NODISCARD FORCEINLINE constexpr auto const& GetSharedMaterialInstances() const noexcept { return this->MaterialInstances; }
    FORCEINLINE void RegisterSharedMaterialInstance(LStringView Identifier, LMaterialInstanceRef Instance) noexcept
    {
        if (this->MaterialInstances.contains(Identifier))
        {
            LOG_FATAL(LogMaterialSubsystem, "[{}]: Material instance with this identifier already exists.", Identifier)
        }
        this->MaterialInstances.emplace(Identifier, Instance);
    }
    NODISCARD std::optional<LMaterialInstanceRef> FindSharedMaterialInstance(LStringView Identifier) noexcept
    {
        if (auto It{this->MaterialInstances.find(Identifier)}; It != this->MaterialInstances.end())
        {
            return It->second;
        }
        return {};
    }
    NODISCARD LMaterialInstanceRef FindOrAllocateSharedMaterialInstance(LStringView Identifier, LStringView Material) noexcept
    {
        if (auto Instance{this->FindSharedMaterialInstance(Identifier)})
        {
            return *Instance;
        }
        auto Result{this->GetInstanceFromMaterialName(Material)};
        this->RegisterSharedMaterialInstance(Identifier, Result);
        return Result;
    }

    //# Get a material that is device ready.
    NODISCARD LMaterialRef GetMaterial(LStringView Name) noexcept;
    //#
    //# Get the instance of a material to specify unique data. Completely optional. You can use materials just
    //# as is without any instance.
    //#
    NODISCARD LMaterialInstanceRef GetInstance(LMaterialRef Material);
    NODISCARD LMaterialInstanceRef GetInstanceFromMaterialName(LStringView MaterialName) { return this->GetInstance(this->GetMaterial(MaterialName)); }

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

    JTextureSubsystem* TextureSubsystem{};
    JShaderSubsystem* ShaderSubsystem{};

    //#
    //# Once populated, materials will never change their address again. This is guaranteed by jafg.
    //#
    //# TODO: Can't we solve with std::hive?
    //#
    TArray<rhi::material_template> MaterialTemplates;
    algo::transparent_unordered_string_map<LMaterialRef> Materials;
    algo::transparent_unordered_string_map<LMaterialInstanceRef> MaterialInstances;
    NODISCARD rhi::material_template const& GetMaterialTemplate(LStringView Identifier) noexcept
    {
        auto It{algo::find(this->MaterialTemplates, Identifier, &rhi::material_template::identifier)};
        if (It == this->MaterialTemplates.end())
        {
            LOG_FATAL(LogMaterialSubsystem, "[{}]: No such material.", Identifier)
        }
        return *It;
    }

    algo::transparent_unordered_string_map<vk::raii::DescriptorSetLayout> SharedDescriptorSetLayouts;
};

} /* ~Namespace Jafg */
