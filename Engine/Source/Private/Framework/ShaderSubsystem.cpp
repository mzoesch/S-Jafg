// Copyright mzoesch. All rights reserved.

#include "Framework/ShaderSubsystem.h"
#include "Platform/PlatformMisc.h"
#include "Stats/Stats.h"
#include "Serialization/Json.h"

namespace
{

LPath Slangc{LPath{Jafg::SprintF("Binaries/{}/Vendor/Slang/bin/slangc{}",
    Jafg::PlatformMisc::GetTargetPlatformCompound(),
#if PLATFORM_WINDOWS
    ".exe"
#else /* PLATFORM_WINDOWS */
    ""
#endif /* !PLATFORM_WINDOWS */
    )}.make_preferred()};


std::unordered_map<LString, Jafg::LDeviceVertexInputStateCreateProviderSig>& GetStaticVertexInputStateCreateProviders() noexcept
{
    static std::unordered_map<LString, Jafg::LDeviceVertexInputStateCreateProviderSig> Providers;
    return Providers;
}

std::unordered_map<LString, Jafg::LPushConstantProviderSig>& GetStaticPushConstantProviders() noexcept
{
    static std::unordered_map<LString, Jafg::LPushConstantProviderSig> Providers;
    return Providers;
}

void PopulateChildFromParent(Jafg::LFetchedShader* Child, Jafg::LFetchedShader const& Parent)
{
    check(Child)

    Child->IncludeDirectories.insert(Child->IncludeDirectories.end(), Parent.IncludeDirectories.begin(), Parent.IncludeDirectories.end());
    Child->Entrypoints.insert(Child->Entrypoints.end(), Parent.Entrypoints.begin(), Parent.Entrypoints.end());

    if (Child->Src.has_value() == false)
    {
        Child->Src = Parent.Src;
    }
    if (Child->SrcPrefix.has_value() == false)
    {
        Child->SrcPrefix = Parent.SrcPrefix;
    }
    if (Child->Dst.has_value() == false)
    {
        Child->Dst = Parent.Dst;
    }
    if (Child->DstPrefix.has_value() == false)
    {
        Child->DstPrefix = Parent.DstPrefix;
    }
    if (Child->VertexInput.has_value() == false)
    {
        Child->VertexInput = Parent.VertexInput;
    }
    Child->PushConstants.insert(Child->PushConstants.begin(), Parent.PushConstants.begin(), Parent.PushConstants.end());

    return;
}

} /* ~Namespace <Anonymous> */

void Jafg::Detail::AddVertexProviderImpl(LString Name, LDeviceVertexInputStateCreateProviderSig Sig) noexcept
{
    LOG_TRACE(LogRhi, "[{}]: Adding vertex input provider.", Name)
    auto& Providers{::GetStaticVertexInputStateCreateProviders()};
    check(Providers.contains(Name) == false)
    Providers[Name] = Sig;

    return;
}

void Jafg::Detail::AddPushConstantProviderImpl(LString Name, LPushConstantProviderSig Sig) noexcept
{
    LOG_TRACE(LogRhi, "[{}]: Adding push constant provider.", Name)
    auto& Providers{::GetStaticPushConstantProviders()};
    check(Providers.contains(Name) == false)
    Providers[Name] = Sig;

    return;
}

void Jafg::JShaderSubsystem::Initialize(LSubsystemCollection& Collection)
{
    STAT_CYCLE_FUNCTION()
    Super::Initialize(Collection);

    if (Finder::DoesFileExist(Slangc) == false)
    {
        LOG_FATAL(LogShaderSubsystem, "No such file: [{}].", Slangc)
    }

    this->RefetchShaders();
    this->RecompileChangedShaders();

    return;
}

void Jafg::JShaderSubsystem::RefetchShaders()
{
    STAT_CYCLE_FUNCTION()
    LOG_VERBOSE(LogShaderSubsystem, "Refetching shaders...")

    algo::orphan(&this->FetchedShaders);

    LString MissingKey; Json::EError Error;
    for (auto ShaderFiles{Finder::FindFilesRecursively(Finder::GetShadersDir(), true, ".*\\.shader.json")}; auto const& TextureViewFile : ShaderFiles)
    {
        LFetchedShader Shader{.Path = TextureViewFile,.Name=TextureViewFile.stem().stem().string()};
        json ShaderJson = json::parse(Finder::ReadFile(Shader.Path), nullptr, false);
        if (ShaderJson.is_discarded())
        {
            LOG_FATAL(LogShaderSubsystem, "[{}]: Shader is not valid json. Failed to load.", Shader.Path)
        }

        if (ShaderJson.contains("Inherits"))
        {
            if (ShaderJson["Inherits"].is_string())
            {
                Shader.Inherits.emplace_back(ShaderJson["Inherits"].get<LString>());
            }
            else if (ShaderJson["Inherits"].is_array())
            {
                for (auto const& Inherit : ShaderJson["Inherits"])
                {
                    if (Inherit.is_string() == false)
                    {
                        LOG_FATAL(LogShaderSubsystem, "[{}]: Inherits entry is not a string. Failed to load.", Shader.Path)
                    }
                    Shader.Inherits.emplace_back(Inherit.get<LString>());
                }
            }
            else
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: Inherits entry is neither a string nor an array. Failed to load.", Shader.Path)
            }
        }

        if (ShaderJson.contains("IncludeDirectories"))
        {
            if (ShaderJson["IncludeDirectories"].is_array() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: IncludeDirectories entry is not an array. Failed to load.", Shader.Path)
            }
            for (auto const& IncludeDirectory : ShaderJson["IncludeDirectories"])
            {
                if (IncludeDirectory.is_string() == false)
                {
                    LOG_FATAL(LogShaderSubsystem, "[{}]: IncludeDirectories entry is not a string. Failed to load.", Shader.Path)
                }
                Shader.IncludeDirectories.emplace_back(IncludeDirectory.get<LString>());
            }
        }

        if (ShaderJson.contains("Entrypoints"))
        {
            if (ShaderJson["Entrypoints"].is_array() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: Entrypoints entry is not an array. Failed to load.", Shader.Path)
            }
            for (auto const& EntrypointJson : ShaderJson["Entrypoints"])
            {
                if (EntrypointJson.is_object() == false)
                {
                    LOG_FATAL(LogShaderSubsystem, "[{}]: Entrypoints entry is not an object. Failed to load.", Shader.Path)
                }
                if (Json::DoesObjectContainTypeCheckedKeys(EntrypointJson, {{"Stage", Json::LKeyType::String}, {"Name", Json::LKeyType::String}}, &MissingKey, &Error) == false)
                {
                    Json::DefaultFail(Shader.Path, MissingKey, Error);
                }
                Shader.Entrypoints.emplace_back(Vk_StringToShaderStageFlagBits(EntrypointJson["Stage"].get<LString>()), EntrypointJson["Name"].get<LString>());
            }
        }

        if (ShaderJson.contains("Src"))
        {
            if (ShaderJson["Src"].is_string() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: Src entry is not a string. Failed to load.", Shader.Path)
            }
            Shader.Src = LPath{ShaderJson["Src"].get<LString>()}.make_preferred();
        }
        if (ShaderJson.contains("SrcPrefix"))
        {
            if (ShaderJson["SrcPrefix"].is_string() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: SrcPrefix entry is not a string. Failed to load.", Shader.Path)
            }
            Shader.SrcPrefix = LPath{ShaderJson["SrcPrefix"].get<LString>()}.make_preferred();
        }
        if (ShaderJson.contains("Dst"))
        {
            if (ShaderJson["Dst"].is_string() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: Dst entry is not a string. Failed to load.", Shader.Path)
            }
            Shader.Dst = LPath{ShaderJson["Dst"].get<LString>()}.make_preferred();
        }
        if (ShaderJson.contains("DstPrefix"))
        {
            if (ShaderJson["DstPrefix"].is_string() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: DstPrefix entry is not a string. Failed to load.", Shader.Path)
            }
            Shader.DstPrefix = LPath{ShaderJson["DstPrefix"].get<LString>()}.make_preferred();
        }

        if (ShaderJson.contains("VertexInput"))
        {
            if (ShaderJson["VertexInput"].is_string() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: VertexInput entry is not a string. Failed to load.", Shader.Path)
            }
            Shader.VertexInput = ShaderJson["VertexInput"].get<LString>();
        }

        if (ShaderJson.contains("PushConstants"))
        {
            if (ShaderJson["PushConstants"].is_array() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: PushConstants entry is not an array. Failed to load.", Shader.Path)
            }
            for (auto const& PushConstant : ShaderJson["PushConstants"])
            {
                if (PushConstant.is_string() == false)
                {
                    LOG_FATAL(LogShaderSubsystem, "[{}]: PushConstants entry is not a string. Failed to load.", Shader.Path)
                }
                Shader.PushConstants.emplace_back(PushConstant.get<LString>());
            }
        }

        this->FetchedShaders.emplace_back(Shader);
    }

    for (auto const& FetchedShader : this->FetchedShaders)
    {
        for (auto const& Parent : FetchedShader.Inherits)
        {
            if (this->HasFetchedShader(Parent) == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: Inherits from [{}] which is not found among fetched shaders. Failed to load.", FetchedShader.Path, Parent)
            }
        }
    }

    for (auto& FetchedShader : this->FetchedShaders)
    {
        for (auto const& Parent : FetchedShader.Inherits)
        {
            auto& ParentShader{this->GetFetchedShader(Parent)};
            if (ParentShader.Inherits.empty() == false)
            {
                LOG_FATAL(LogShaderSubsystem
                    , "[{}]: Inherits from [{}] which itself inherits from other shaders. Multiple levels of inheritance are currently not supported."
                    , FetchedShader.Path, Parent
                    )
            }
            ::PopulateChildFromParent(&FetchedShader, ParentShader);
        }
    }

    if constexpr (IS_COMPILED_LOG(LogShaderSubsystem, Trace))
    for (auto const& FetchedShader : this->FetchedShaders)
    {
        LOG_TRACE(LogShaderSubsystem, "[{}]: IncludeDirectories [{}], Entrypoints [{}], Src [{}], Dst [{}], VertexInput [{}], PushConstants [{}]."
            , FetchedShader.Path
            , algo::join(FetchedShader.IncludeDirectories)
            , algo::join(FetchedShader.Entrypoints, &LShaderEntrypoint::Name)
            , FetchedShader.Src.has_value() ? FetchedShader.GetSrc().string() : "N/A"
            , FetchedShader.Dst.has_value() ? FetchedShader.GetDst().string() : "N/A"
            , FetchedShader.VertexInput.has_value() ? *FetchedShader.VertexInput : "N/A"
            , algo::join(FetchedShader.PushConstants)
            )
    }

    LOG_VERBOSE(LogShaderSubsystem, "Refetched {} shaders.", this->FetchedShaders.size())
    return;
}

void Jafg::JShaderSubsystem::RecompileChangedShaders()
{
    LOG_VERBOSE(LogShaderSubsystem, "Recompiling changed shaders...")

    auto Recompiled{0uz};
    for (auto const& FetchedShader : this->FetchedShaders)
    {
        if (FetchedShader.Src.has_value() == false || FetchedShader.Dst.has_value() == false)
        {
            LOG_TRACE(LogShaderSubsystem, "[{}]: No source or destination specified. Skipping recompilation.", FetchedShader.Path)
            continue;
        }

        LPath Src{FetchedShader.GetSrc()};
        if (Finder::DoesFileExist(Src) == false)
        {
            LOG_FATAL(LogShaderSubsystem, "Shader [{}] source file [{}] does not exist. Failed to recompile.", FetchedShader.Path, Src)
        }

        LPath Dst{FetchedShader.GetDst()};
        if (auto DstParent{Dst.parent_path()}; Finder::DoesDirectoryExist(DstParent) == false)
        {
            Finder::CreateDirectories(DstParent);
        }

        if (Finder::DoesFileExist(Dst) == false)
        {
            LOG_VERBOSE(LogShaderSubsystem, "Shader [{}] does not exist. Compiling from [{}].", Dst, Src)
            TArray<LString> Entrypoints; Entrypoints.reserve(FetchedShader.Entrypoints.size());
            algo::for_each(FetchedShader.Entrypoints, [&Entrypoints](auto const& Entrypoint)
            {
                Entrypoints.emplace_back(Entrypoint.Name);
            });
            if (this->RecompileShader({
                .In = Src, .Out = Dst,
                .IncludeDirectories = FetchedShader.IncludeDirectories, .EntryPoints = Entrypoints,
                }) != 0)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: Failed to compile shader from [{}].", Dst, Src)
            }
            ++Recompiled;
            continue;
        }

        auto EntryLastWriteTime{std::filesystem::last_write_time(Src)};
        auto DstLastWriteTime{std::filesystem::last_write_time(Dst)};
        if (EntryLastWriteTime > DstLastWriteTime)
        {
            LOG_VERBOSE(LogShaderSubsystem, "[{}]: Shader is outdated. Compiling from [{}]; Because [{:%c} > {:%c}]."
                , Dst, Src, EntryLastWriteTime, DstLastWriteTime
                )
            TArray<LString> Entrypoints; Entrypoints.reserve(FetchedShader.Entrypoints.size());
            algo::for_each(FetchedShader.Entrypoints, [&Entrypoints](auto const& Entrypoint)
            {
                Entrypoints.emplace_back(Entrypoint.Name);
            });
            if (this->RecompileShader({
                .In = Src, .Out = Dst,
                .IncludeDirectories = FetchedShader.IncludeDirectories, .EntryPoints = Entrypoints,
                }) != 0)
            {
                LOG_FATAL(LogShaderSubsystem, "Failed to compile shader [{}] from [{}].", Dst, Src)
            }
            ++Recompiled;
            continue;
        }

        LOG_TRACE(LogShaderSubsystem, "Shader [{}] is up to date. No need to compile from [{}]; Because [{:%c} <= {:%c}]."
            , Dst, Src, EntryLastWriteTime, DstLastWriteTime
            )
         continue;
    }

    if (Recompiled > 0)
    {
        LOG_VERBOSE(LogShaderSubsystem, "Recompiled {} shaders.", Recompiled)
    }
    else
    {
        LOG_VERBOSE(LogShaderSubsystem, "All shaders up to date. No shaders recompiled.")
    }

    return;
}

i32 Jafg::JShaderSubsystem::RecompileShader(LShaderCompilationRequest const& Request)
{
    Slangc.make_preferred();

    std::ostringstream SS;

    SS << Slangc;

    /* Quoting the requested in/out paths will not work, therefore spaces are not permitted. */
    check(algo::contains(Request.In.native(), LITERAL_TEXT(' ')) == false && "Slangc does not permit spaces in requested in/out paths.")
    check(algo::contains(Request.Out.native(), LITERAL_TEXT(' ')) == false && "Slangc does not permit spaces in requested in/out paths.")
    SS << " " << Request.In.string();
    SS << " -o " << Request.Out.string();
    for (LString const& IncludeDirectory : Request.IncludeDirectories)
    {
        SS << " -I" << IncludeDirectory;
    }
    SS << " -target " << Request.Target;
    SS << " -profile " << Request.Profile;
    SS << " -emit-spirv-directly";

    if (Request.EntryPoints.empty() == false)
    {
        SS << " -fvk-use-entrypoint-name";
        for (auto const& EntryPoint : Request.EntryPoints)
        {
            SS << " -entry " << EntryPoint;
        }
    }

    auto SSStr{SS.str()};
    LOG_VERBOSE(LogSystem, "Executing: [{}].", SSStr)
    return std::system(SSStr.c_str());
}

vk::PipelineVertexInputStateCreateInfo Jafg::JShaderSubsystem::GetVertexInputStateCreateInfo(LString const& Name) const
{
    auto& Providers{::GetStaticVertexInputStateCreateProviders()};
    if (auto It{Providers.find(Name)}; It != Providers.end())
    {
        return It->second();
    }
    LOG_FATAL(LogShaderSubsystem, "[{}]: No such vertex provider.", Name)
}

Jafg::Detail::LPushConstantInfo Jafg::JShaderSubsystem::GetPushConstantInfo(LString const& Name) const
{
    auto& Providers{::GetStaticPushConstantProviders()};
    if (auto It{Providers.find(Name)}; It != Providers.end())
    {
        return It->second();
    }
    LOG_FATAL(LogShaderSubsystem, "[{}]: No such push constant provider.", Name)
}
