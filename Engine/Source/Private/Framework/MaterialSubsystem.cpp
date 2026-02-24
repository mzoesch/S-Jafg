// Copyright mzoesch. All rights reserved.

#include "Framework/MaterialSubsystem.h"
#include "Framework/TextureViewSubsystem.h"
#include "Framework/ShaderSubsystem.h"
#include "Stats/Stats.h"
#include "Serialization/Json.h"

void Jafg::JMaterialSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    Collection.InitializeDependency<JTextureViewSubsystem>(this);
    Collection.InitializeDependency<JShaderSubsystem>(this);
    this->ReloadMaterials();

    return;
}

void Jafg::JMaterialSubsystem::ReloadMaterials()
{
    STAT_CYCLE_FUNCTION()
    LOG_VERBOSE(LogMaterialSubsystem, "Reloading materials.")

    for (auto MaterialFiles{Finder::FindFilesRecursively("Content/Materials", true, ".*\\.mat")}; auto const& MaterialFile : MaterialFiles)
    {
        LMaterial Material{.Path = MaterialFile};
        json MaterialJson = json::parse(Finder::ReadFile(Material.Path), nullptr, false);
        if (MaterialJson.is_discarded())
        {
            LOG_FATAL(LogMaterialSubsystem, "Material [{}] is not valid json. Failed to load.", Material.Path)
        }

        if (LString MissingKey; Json::DoesObjectContainKeys(MaterialJson, {"Shader", "TextureViews"}, &MissingKey) == false)
        {
            if (MissingKey.empty())
            {
                LOG_FATAL(LogMaterialSubsystem, "Material [{}] is not a json object. Failed to load.", Material.Path)
            }
            LOG_FATAL(LogMaterialSubsystem, "Material [{}] does not contain key [{}]. Failed to load.", Material.Path, MissingKey)
        }

        if (MaterialJson["Shader"].is_string() == false)
        {
            LOG_FATAL(LogMaterialSubsystem, "Material [{}]: Shader key is not a string. Failed to load.", Material.Path)
        }
        Material.Shader = NativeSprintF(LITERAL_TEXT("{}.spv"), (Finder::GetSpirShadersDir() / MaterialJson["Shader"].get<LString>()).native());
        if (Finder::DoesFileExist(Material.Shader) == false)
        {
            LOG_FATAL(LogMaterialSubsystem, "[{}]: No such shader [{}]."
                , Material.Path, Material.Shader
                )
        }

        if (MaterialJson["TextureViews"].is_array() == false)
        {
            LOG_FATAL(LogMaterialSubsystem, "Material [{}]: TextureViews key is not an array. Failed to load.", Material.Path)
        }
        json TextureViewArray = MaterialJson["TextureViews"];
        for (auto const& TextureViewJson : TextureViewArray)
        {
            if (LString MissingKey; Json::DoesObjectContainKeys(TextureViewJson, {"Identifier"}, &MissingKey) == false)
            {
                if (MissingKey.empty())
                {
                    LOG_FATAL(LogMaterialSubsystem, "Material [{}]: TextureView entry is not a json object. Failed to load.", Material.Path)
                }
                LOG_FATAL(LogMaterialSubsystem, "Material [{}]: TextureView entry does not contain key [{}]. Failed to load.", Material.Path, MissingKey)
            }

            if (TextureViewJson["Identifier"].is_string() == false)
            {
                LOG_FATAL(LogMaterialSubsystem, "Material [{}]: TextureView entry Identifier key is not a string. Failed to load.", Material.Path)
            }

            Material.TextureViews.emplace_back(TextureViewJson["Identifier"].get<LString>());
        }

        this->Materials.emplace_back(std::move(Material));
    }

    LOG_VERBOSE(LogMaterialSubsystem, "Finished loading [{}] materials.", this->Materials.size())

    return;
}
