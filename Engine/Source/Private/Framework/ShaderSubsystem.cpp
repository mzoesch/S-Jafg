// Copyright mzoesch. All rights reserved.

#include "Framework/ShaderSubsystem.h"
#include "Platform/PlatformMisc.h"
#include "Stats/Stats.h"
#include "Serialization/Json.h"
#include "Rhi/PushConstants.h"
#include "Rhi/FromString.h"

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

std::unordered_map<LString, Jafg::Detail::LPushConstantSigs>& GetStaticPushConstantProviders() noexcept
{
    static std::unordered_map<LString, Jafg::Detail::LPushConstantSigs> Providers;
    return Providers;
}

void PopulateChildFromParent(Jafg::LFetchedShader* Child, Jafg::LFetchedShader const& Parent)
{
    check(Child)

    Child->IncludeDirectories.insert(Child->IncludeDirectories.end(), Parent.IncludeDirectories.begin(), Parent.IncludeDirectories.end());
    Child->Entrypoints.insert(Child->Entrypoints.end(), Parent.Entrypoints.begin(), Parent.Entrypoints.end());
    Child->CompileTimeDefinitions.insert(Child->CompileTimeDefinitions.end(), Parent.CompileTimeDefinitions.begin(), Parent.CompileTimeDefinitions.end());

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

    if (Parent.VertexInput.has_value())
    {
        if (Child->VertexInput.has_value())
        {
            LOG_FATAL(LogShaderSubsystem, "[{}]: Both child and parent specify vertex input. This is not allowed.", Child->Path)
        }
        Child->VertexInput = Parent.VertexInput;
    }

    if (Parent.PushConstants.empty() == false)
    {
        Child->PushConstants.insert_range(Child->PushConstants.begin(), Parent.PushConstants);
    }
    if (Parent.Layouts.empty() == false)
    {
        Child->Layouts.insert_range(Child->Layouts.begin(), Parent.Layouts);
    }

    return;
}

struct FindSccsData
{
    TArray<std::unique_ptr<Jafg::LFetchedShader>> const& Shaders;

    u64 Index;
    TArray<LString> Stack;
    std::unordered_set<LString> OnStack;
    std::unordered_map<LString, u64> Indices;
    std::unordered_map<LString, u64> LowLink;

    TArray<TArray<LString>> Sccs;
};

void FindSccsConnect(FindSccsData& Data, Jafg::LFetchedShader const& Shader)
{
    Data.Indices[Shader.Name] = Data.Index;
    Data.LowLink[Shader.Name] = Data.Index;
    ++Data.Index;
    Data.Stack.emplace_back(Shader.Name);
    Data.OnStack.insert(Shader.Name);

    for (auto const& Parent : Shader.Inherits)
    {
        if (Data.Indices.contains(Parent) == false)
        {
            auto It{algo::find(Data.Shaders, Parent, &Jafg::LFetchedShader::Name)};
            if (It == Data.Shaders.end())
            {
                LOG_FATAL(LogShaderSubsystem, "No such shader [{}].", Parent)
            }
            FindSccsConnect(Data, **It);
            Data.LowLink[Shader.Name] = std::min(Data.LowLink[Shader.Name], Data.LowLink[Parent]);
        }
        else if (Data.OnStack.contains(Parent))
        {
            if (auto It{algo::find(Data.Shaders, Parent, &Jafg::LFetchedShader::Name)}; It == Data.Shaders.end())
            {
                LOG_FATAL(LogShaderSubsystem, "No such shader [{}].", Parent)
            }
            Data.LowLink[Shader.Name] = std::min(Data.LowLink[Shader.Name], Data.Indices[Parent]);
        }
    }

    if (Data.LowLink[Shader.Name] == Data.Indices[Shader.Name])
    {
        TArray<LString> Scc;
        while (true)
        {
            auto Top{Data.Stack.back()};
            Data.Stack.pop_back();
            Data.OnStack.erase(Top);
            Scc.emplace_back(Top);
            if (Top == Shader.Name)
            {
                break;
            }
        }
        Data.Sccs.emplace_back(std::move(Scc));
    }

    return;
}

TArray<TArray<LString>> FindSccs(TArray<std::unique_ptr<Jafg::LFetchedShader>> const& Shaders)
{
    FindSccsData Data{.Shaders=Shaders};
    for (auto const& Shader : Shaders)
    {
        if (Data.Indices.contains(Shader->Name) == false)
        {
            FindSccsConnect(Data, *Shader);
        }
    }
    return Data.Sccs;
}

void TopDownPopulation(TArray<Jafg::LFetchedShader*>& Shaders)
{
    auto IsLeaf{[&Shaders](Jafg::LFetchedShader const& Shader) -> bool
    {
        for (auto* PotentialChild : Shaders)
        {
            if (algo::contains(PotentialChild->Inherits, Shader.Name))
            {
                return false;
            }
        }
        return true;
    }};

    auto GetShader{[&Shaders](LString const& Name) -> Jafg::LFetchedShader&
    {
        auto It{algo::find(Shaders, Name, &Jafg::LFetchedShader::Name)};
        if (It == Shaders.end())
        {
            LOG_FATAL(LogShaderSubsystem, "No such shader [{}].", Name)
        }
        return **It;
    }};

    TArray<Jafg::LFetchedShader*> Siblings;
    TArray<Jafg::LFetchedShader*> Unrelated;

    for (auto* Shader : Shaders)
    {
        if (IsLeaf(*Shader))
        {
            Siblings.emplace_back(Shader);
        }
        else
        {
            Unrelated.emplace_back(Shader);
        }
    }

    if (Unrelated.empty() == false)
    {
        ::TopDownPopulation(Unrelated);
    }

    for (auto* Sibling : Siblings)
    {
        for (auto const& Parent : Sibling->Inherits)
        {
            PopulateChildFromParent(Sibling, GetShader(Parent));
        }
    }

    return;
}

void ResolveInheritanceValues(TArray<std::unique_ptr<Jafg::LFetchedShader>>& Shaders)
{
    for (auto Sccs{::FindSccs(Shaders)}; auto const& Scc : Sccs)
    {
        if (Scc.size() > 1)
        {
            LOG_FATAL(LogShaderSubsystem
                , "Found inheritance cycle in shaders: [{}]."
                , algo::join(Scc)
                )
        }
    }

    TArray<Jafg::LFetchedShader*> Temp; Temp.reserve(Shaders.size());
    for (auto& Shader : Shaders)
    {
        if (algo::contains(Shader->Inherits, Shader->Name))
        {
            LOG_FATAL(LogShaderSubsystem
                , "[{}]: Reflexive inheritance are not allowed. [{}] inherits from [{}]."
                , Shader->Path, Shader->Name, algo::join(Shader->Inherits)
                )
        }
        Temp.emplace_back(&*Shader);
    }
    ::TopDownPopulation(Temp);

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

void Jafg::Detail::AddPushConstantProviderImpl(LString Name, LPushConstantSigs const& Sig) noexcept
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

    // TODO: We need to track changes and then change device information accordingly/rebuild pipelines, etc.
    check(this->FetchedShaders.empty())

    LString MissingKey; Json::EError Error;
    for (auto ShaderFiles{Finder::FindFilesRecursively(Finder::GetShadersDir(), true, ".*\\.shader.json")}; auto const& TextureViewFile : ShaderFiles)
    {
        std::unique_ptr Shader{ std::make_unique<LFetchedShader>(TextureViewFile, TextureViewFile.stem().stem().string())};
        json ShaderJson = json::parse(Finder::ReadFile(Shader->Path), nullptr, false);
        if (ShaderJson.is_discarded())
        {
            LOG_FATAL(LogShaderSubsystem, "[{}]: Shader is not valid json. Failed to load.", Shader->Path)
        }

        if (ShaderJson.contains("Inherits"))
        {
            if (ShaderJson["Inherits"].is_string())
            {
                Shader->Inherits.emplace_back(ShaderJson["Inherits"].get<LString>());
            }
            else if (ShaderJson["Inherits"].is_array())
            {
                for (auto const& Inherit : ShaderJson["Inherits"])
                {
                    if (Inherit.is_string() == false)
                    {
                        LOG_FATAL(LogShaderSubsystem, "[{}]: Inherits entry is not a string. Failed to load.", Shader->Path)
                    }
                    Shader->Inherits.emplace_back(Inherit.get<LString>());
                }
            }
            else
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: Inherits entry is neither a string nor an array. Failed to load.", Shader->Path)
            }
        }

        if (ShaderJson.contains("IncludeDirectories"))
        {
            if (ShaderJson["IncludeDirectories"].is_array() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: IncludeDirectories entry is not an array. Failed to load.", Shader->Path)
            }
            for (auto const& IncludeDirectory : ShaderJson["IncludeDirectories"])
            {
                if (IncludeDirectory.is_string() == false)
                {
                    LOG_FATAL(LogShaderSubsystem, "[{}]: IncludeDirectories entry is not a string. Failed to load.", Shader->Path)
                }
                Shader->IncludeDirectories.emplace_back(IncludeDirectory.get<LString>());
            }
        }

        if (ShaderJson.contains("Entrypoints"))
        {
            if (ShaderJson["Entrypoints"].is_array() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: Entrypoints entry is not an array. Failed to load.", Shader->Path)
            }
            for (auto const& EntrypointJson : ShaderJson["Entrypoints"])
            {
                if (EntrypointJson.is_object() == false)
                {
                    LOG_FATAL(LogShaderSubsystem, "[{}]: Entrypoints entry is not an object. Failed to load.", Shader->Path)
                }
                if (Json::DoesObjectContainTypeCheckedKeys(EntrypointJson, {{"Stage", Json::LKeyType::String}, {"Name", Json::LKeyType::String}}, &MissingKey, &Error) == false)
                {
                    Json::DefaultFail(Shader->Path, MissingKey, Error);
                }
                Shader->Entrypoints.emplace_back(Vk_FromString<vk::ShaderStageFlagBits>(EntrypointJson["Stage"].get<LString>()), EntrypointJson["Name"].get<LString>());
            }
        }

        if (ShaderJson.contains("CompileTimeDefinitions"))
        {
            if (ShaderJson["CompileTimeDefinitions"].is_array() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: CompileTimeDefinitions entry is not an array. Failed to load.", Shader->Path)
            }
            for (auto const& CompileTimeDefinitionJson : ShaderJson["CompileTimeDefinitions"])
            {
                if (CompileTimeDefinitionJson.is_object() == false)
                {
                    LOG_FATAL(LogShaderSubsystem, "[{}]: CompileTimeDefinitions entry is not an object. Failed to load.", Shader->Path)
                }
                if (Json::DoesObjectContainTypeCheckedKeys(CompileTimeDefinitionJson, {{"Name", Json::LKeyType::String}, {"Value", Json::LKeyType::String}}, &MissingKey, &Error) == false)
                {
                    Json::DefaultFail(Shader->Path, MissingKey, Error);
                }
                Shader->CompileTimeDefinitions.emplace_back(LFetchedShader::CompileTimeDefinition{CompileTimeDefinitionJson["Name"].get<LString>(), CompileTimeDefinitionJson["Value"].get<LString>()});
            }
        }

        if (ShaderJson.contains("Src"))
        {
            if (ShaderJson["Src"].is_string() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: Src entry is not a string. Failed to load.", Shader->Path)
            }
            Shader->Src = LPath{ShaderJson["Src"].get<LString>()}.make_preferred();
        }
        if (ShaderJson.contains("SrcPrefix"))
        {
            if (ShaderJson["SrcPrefix"].is_string() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: SrcPrefix entry is not a string. Failed to load.", Shader->Path)
            }
            Shader->SrcPrefix = LPath{ShaderJson["SrcPrefix"].get<LString>()}.make_preferred();
        }
        if (ShaderJson.contains("Dst"))
        {
            if (ShaderJson["Dst"].is_string() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: Dst entry is not a string. Failed to load.", Shader->Path)
            }
            Shader->Dst = LPath{ShaderJson["Dst"].get<LString>()}.make_preferred();
        }
        if (ShaderJson.contains("DstPrefix"))
        {
            if (ShaderJson["DstPrefix"].is_string() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: DstPrefix entry is not a string. Failed to load.", Shader->Path)
            }
            Shader->DstPrefix = LPath{ShaderJson["DstPrefix"].get<LString>()}.make_preferred();
        }

        if (ShaderJson.contains("VertexInput"))
        {
            if (ShaderJson["VertexInput"].is_string() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: VertexInput entry is not a string. Failed to load.", Shader->Path)
            }
            Shader->VertexInput = ShaderJson["VertexInput"].get<LString>();
        }

        if (ShaderJson.contains("PushConstants"))
        {
            if (ShaderJson["PushConstants"].is_array() == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: PushConstants entry is not an array. Failed to load.", Shader->Path)
            }
            for (auto const& PushConstant : ShaderJson["PushConstants"])
            {
                if (PushConstant.is_string() == false)
                {
                    LOG_FATAL(LogShaderSubsystem, "[{}]: PushConstants entry is not a string. Failed to load.", Shader->Path)
                }
                Shader->PushConstants.emplace_back(PushConstant.get<LString>());
            }
        }

        for (json LayoutArray = ShaderJson["Layouts"]; auto const& LayoutJson : LayoutArray)
        {
            if (Json::DoesObjectContainTypeCheckedKeys(LayoutJson, {{"Type", Json::LKeyType::String}}, &MissingKey, &Error) == false)
            {
                Json::DefaultFail(Shader->Path, MissingKey, Error);
            }

            if (LString SetType{LayoutJson["Type"].get<LString>()}; SetType == "eUnique")
            {
                if (Json::DoesObjectContainTypeCheckedKeys(LayoutJson, {{"UpdateFrequency", Json::LKeyType::String}, {"Stage", Json::LKeyType::String}, {"Sets", Json::LKeyType::Array}}, &MissingKey, &Error) == false)
                {
                    Json::DefaultFail(Shader->Path, MissingKey, Error);
                }

                TArray<LFetchedShader::Layout::Set> Sets;
                for (auto const& SetJson : LayoutJson["Sets"])
                {
                    if (Json::DoesObjectContainTypeCheckedKeys(SetJson, {{"Identifier", Json::LKeyType::String}, {"DescriptorType", Json::LKeyType::String}}, &MissingKey, &Error) == false)
                    {
                        Json::DefaultFail(Shader->Path, MissingKey, Error);
                    }
                    LString SetIdentifier{SetJson["Identifier"].get<LString>()};
                    algo::for_each(Shader->Layouts, [&Shader, &SetIdentifier](LFetchedShader::Layout const& Layout)
                    {
                        if (Layout.Sets.has_value())
                        {
                            algo::for_each(*Layout.Sets, [&Shader, &SetIdentifier](LFetchedShader::Layout::Set const& Set)
                            {
                                if (Set.Identifier == SetIdentifier)
                                {
                                    LOG_FATAL(LogShaderSubsystem, "[{}]: Set identifier [{}] is not unique among sets.", Shader->Path, SetIdentifier)
                                }
                            });
                        }
                    });

                    Sets.emplace_back(LFetchedShader::Layout::Set{
                        .Identifier=std::move(SetIdentifier),
                        .DescriptorType=Vk_FromString<vk::DescriptorType>(SetJson["DescriptorType"].get<LString>())
                        });
                    continue;
                }

                Shader->Layouts.emplace_back(LFetchedShader::Layout{
                    .Type=LFetchedShader::Layout::eUnique,
                    .UpdateFrequency = LFetchedShader::Layout::StringToUpdateFrequency(LayoutJson["UpdateFrequency"].get<LString>()),
                    .Stage=Vk_FromString<vk::ShaderStageFlags>(LayoutJson["Stage"].get<LString>()),
                    .Sets=std::move(Sets)
                    });
                check(SetType.contains("Identifier") == false)
            }
            else if (SetType == "eShared")
            {
                if (Json::DoesObjectContainTypeCheckedKeys(LayoutJson, {{"Identifier", Json::LKeyType::String}}, &MissingKey, &Error) == false)
                {
                    Json::DefaultFail(Shader->Path, MissingKey, Error);
                }

                LString LayoutIdentifier {LayoutJson["Identifier"].get<LString>()};
                algo::for_each(Shader->Layouts, [&Shader, &LayoutIdentifier](LFetchedShader::Layout const& Layout)
                {
                    if (Layout.Identifier.has_value())
                    {
                        if (Layout.Identifier.value() == LayoutIdentifier)
                        {
                            LOG_FATAL(LogShaderSubsystem, "[{}]: Layout identifier [{}] is not unique among layouts.", Shader->Path, LayoutIdentifier)
                        }
                    }
                });
                Shader->Layouts.emplace_back(LFetchedShader::Layout{.Type=LFetchedShader::Layout::eShared,.Identifier=std::move(LayoutIdentifier)});
                check(SetType.contains("Set") == false)
            }
            else
            {
                LOG_FATAL(LogMaterialSubsystem, "[{}]: Layout entry has invalid type [{}]. Failed to load.", Shader->Path, SetType)
            }

            continue;
        }

        this->FetchedShaders.emplace_back(std::move(Shader));
    }

    for (auto const& FetchedShader : this->FetchedShaders)
    {
        for (auto const& Parent : FetchedShader->Inherits)
        {
            if (this->HasFetchedShader(Parent) == false)
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: Inherits from [{}] which is not found among fetched shaders. Failed to load.", FetchedShader->Path, Parent)
            }
        }
    }

    ::ResolveInheritanceValues(this->FetchedShaders);

    if constexpr (IS_COMPILED_LOG(LogShaderSubsystem, Trace))
    for (auto const& FetchedShader : this->FetchedShaders)
    {
        LOG_TRACE(LogShaderSubsystem, "[{}]: IncludeDirectories [{}], Entrypoints [{}], CompileDefinitions [{}], Src [{}], Dst [{}], VertexInput [{}], PushConstants [{}], Layouts [{}]."
            , FetchedShader->Path
            , algo::join(FetchedShader->IncludeDirectories)
            , algo::join(FetchedShader->Entrypoints, &LShaderEntrypoint::Name)
            , algo::join(FetchedShader->CompileTimeDefinitions, [](auto const& E){ return E.Name + "=" + E.Value; })
            , FetchedShader->Src.has_value() ? FetchedShader->GetSrc().string() : "N/A"
            , FetchedShader->Dst.has_value() ? FetchedShader->GetDst().string() : "N/A"
            , FetchedShader->VertexInput.has_value() ? *FetchedShader->VertexInput : "N/A"
            , algo::join(FetchedShader->PushConstants)
            , algo::join(FetchedShader->Layouts, [](LFetchedShader::Layout const& L) -> LString
            {
                if (L.Type == LFetchedShader::Layout::eUnique)
                {
                    check(L.Sets.has_value())
                    check(L.Stage.has_value())
                    return SprintF("Unique@{}[{}]", vk::to_string(*L.Stage), algo::join(*L.Sets, [](LFetchedShader::Layout::Set const& S) -> LString
                    {
                        return S.Identifier + "(" + vk::to_string(S.DescriptorType) + ")";
                    }));
                }
                else if (L.Type == LFetchedShader::Layout::eShared)
                {
                    check(L.Identifier.has_value())
                    return SprintF("Shared[{}]", *L.Identifier);
                }
                else
                {
                    unreachable()
                }
            }))
    }

    LOG_VERBOSE(LogShaderSubsystem, "Refetched {} shaders.", this->FetchedShaders.size())
    return;
}

void Jafg::JShaderSubsystem::RecompileChangedShaders()
{
    LOG_VERBOSE(LogShaderSubsystem, "Recompiling changed shaders...")

    auto Recompiled{0uz};
    for (auto const& FetchedShaderPtr : this->FetchedShaders)
    {
        check(FetchedShaderPtr.get())
        auto const& FetchedShader{*FetchedShaderPtr};

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
            if (this->RecompileShader({.FetchedShader=FetchedShader}) != 0)
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
            if (this->RecompileShader({.FetchedShader=FetchedShader}) != 0)
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
    check(algo::contains(Request.FetchedShader.GetSrc().native(), LITERAL_TEXT(' ')) == false && "Slangc does not permit spaces in requested in/out paths.")
    check(algo::contains(Request.FetchedShader.GetDst().native(), LITERAL_TEXT(' ')) == false && "Slangc does not permit spaces in requested in/out paths.")
    SS << " " << Request.FetchedShader.GetSrc().string();
    SS << " -o " << Request.FetchedShader.GetDst().string();
    for (LString const& IncludeDirectory : Request.FetchedShader.IncludeDirectories)
    {
        SS << " -I" << IncludeDirectory;
    }
    SS << " -target " << Request.Target;
    SS << " -profile " << Request.Profile;
    SS << " -emit-spirv-directly";

    for (auto const& CompileTimeDefinition : Request.FetchedShader.CompileTimeDefinitions)
    {
        SS << " -D" << CompileTimeDefinition.Name << "=" << CompileTimeDefinition.Value;
    }

    if (Request.FetchedShader.Entrypoints.empty() == false)
    {
        SS << " -fvk-use-entrypoint-name";
        for (auto const& EntryPoint : Request.FetchedShader.Entrypoints)
        {
            SS << " -entry " << EntryPoint.Name;
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

Jafg::Detail::LPushConstantSigs const& Jafg::JShaderSubsystem::GetPushConstant(LString const& Name) const
{
    auto& Providers{::GetStaticPushConstantProviders()};
    if (auto It{Providers.find(Name)}; It != Providers.end())
    {
        return It->second;
    }
    LOG_FATAL(LogShaderSubsystem, "[{}]: No such push constant.", Name)
}

Jafg::Detail::LPushConstantInfo Jafg::JShaderSubsystem::GetPushConstantInfo(LString const& Name) const
{
    auto& Providers{::GetStaticPushConstantProviders()};
    if (auto It{Providers.find(Name)}; It != Providers.end())
    {
        check(It->second.Provider)
        return It->second.Provider();
    }
    LOG_FATAL(LogShaderSubsystem, "[{}]: No such push constant provider.", Name)
}
