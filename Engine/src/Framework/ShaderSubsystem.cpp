// Copyright mzoesch. All rights reserved.

#include "Framework/ShaderSubsystem.h"
#include "Rhi/FromString.h"
#include "Core/App.h"
#include "Rhi/Objects.h"

namespace Jafg
{

SERDE_JSON_TYPE_ONLY_DESERIALIZE(JShaderSubsystem::Shader2::CompileTimeDefinition, Identifier, Value)
inline void from_json(json const& j, JShaderSubsystem::Shader2& Shader) noexcept
{
    j.at("IncludeDirs").get_to(Shader.IncludeDirs);
    j.at("SourceFiles").get_to(Shader.SourceFiles);
    if (j.contains("CompileTimeDefinitions"))
    {
        j.at("CompileTimeDefinitions").get_to(Shader.CompileTimeDefinitions);
    }
}

} /* ~Namespace Jafg */

namespace
{

LPath Slangc{LPath{algo::sprintf("Vendor/slang-2026.11-{}-{}/bin/slangc{}",
    Jafg::App::GetTargetPlatform(), Jafg::App::GetTargetArchitecture(),
#if JAFG_PLATFORM_WINDOWS
    ".exe"
#else /* JAFG_PLATFORM_WINDOWS */
    ""
#endif /* !JAFG_PLATFORM_WINDOWS */
    )}.make_preferred()};

constexpr LStringView SlangTarget{ "spirv" };
constexpr LStringView SlangProfile{ "spirv_1_5" };

TArray<Jafg::LVertexInputProvider>& GetStaticVertexInputProviders() noexcept
{
    check(Jafg::Tasks::IsOnMasterThread())
    static TArray<Jafg::LVertexInputProvider> Providers;
    return Providers;
}

TArray<Jafg::LPushConstantProvider>& GetStaticPushConstantProviders() noexcept
{
    check(Jafg::Tasks::IsOnMasterThread())
    static TArray<Jafg::LPushConstantProvider> Providers;
    return Providers;
}

TArray<Jafg::LBufferObjectProvider>& GetStaticBufferObjectProviders() noexcept
{
    check(Jafg::Tasks::IsOnMasterThread())
    static TArray<Jafg::LBufferObjectProvider> Providers;
    return Providers;
}

} /* ~Namespace <Anonymous> */

void Jafg::Detail::RegisterVertexInputGlobally(LVertexInputProvider Provider) noexcept
{
    LOG_TRACE(LogRhi, "[{}]: Adding vertex input provider.", Provider.Identifier)
    auto& Providers{::GetStaticVertexInputProviders()};
    check(!algo::contains(Providers, Provider.Identifier, &LVertexInputProvider::Identifier))
    Providers.emplace_back(std::move(Provider));
}

void Jafg::Detail::RegisterPushConstantGlobally(LPushConstantProvider Provider) noexcept
{
    LOG_TRACE(LogRhi, "[{}]: Adding push constant provider.", Provider.Identifier)
    auto& Providers{::GetStaticPushConstantProviders()};
    check(!algo::contains(Providers, Provider.Identifier, &LPushConstantProvider::Identifier))
    Providers.emplace_back(std::move(Provider));
}

void Jafg::Detail::RegisterBufferObjectGlobally(LBufferObjectProvider Provider) noexcept
{
    LOG_TRACE(LogRhi, "[{}]: Buffer object provider.", Provider.Identifier)
    auto& Providers{::GetStaticBufferObjectProviders()};
    check(!algo::contains(Providers, Provider.Identifier, &LBufferObjectProvider::Identifier))
    Providers.emplace_back(std::move(Provider));
}

void Jafg::JShaderSubsystem::Initialize(LSubsystemCollection& Collection)
{
    STAT_FUNCTION()
    Super::Initialize(Collection);

    if (!is_regular_file(Slangc))
    {
        LOG_FATAL(LogShaderSubsystem, "No such file: [{}].", Slangc)
    }

    LOG_VERBOSE(LogShaderSubsystem, "Fetching shaders.")
    check(this->ReflectedShaders.empty())

    TArray<Shader2> Shaders;

    for (LPath& Shader: finder::retrieve_files<finder::recursive_directory_iterator>(*JafgInfo, ".*\\.shader\\.json"))
    {
        Shader2& Result{Shaders.emplace_back()};
        json::parse(finder::read_file(Shader), nullptr, false).get_to(Result);

        for (auto& IncludeDir: Result.IncludeDirs)
        {
            IncludeDir = *JafgInfo/IncludeDir;
        }
        for (auto& SourceFile: Result.SourceFiles)
        {
            SourceFile = *JafgInfo/SourceFile;
        }

        Result.Namespace = *JafgInfo;
        Result.Path = std::move(Shader);
    }

    auto Recompiled{0uz};
    for (auto const& Shader: Shaders)
    {
        Recompiled += this->RecompileShaderConditionally(Shader);
    }
    if constexpr (LogShaderSubsystem.CompilesFor<ELogVerbosity::Verbose>)
    {
        if (Recompiled > 0)
        {
            LOG_VERBOSE(LogShaderSubsystem, "Recompiled [{}] shaders.", Recompiled)
        }
        else
        {
            LOG_VERBOSE(LogShaderSubsystem, "All shaders up to date. No shaders recompiled.")
        }
    }

    for (auto const& Shader: Shaders)
    {
        auto& Result{this->ReflectedShaders.emplace_back()};
        check(finder::descendant_of(Shader.Path, *JafgInfo))
        Result.Identifier = relative(weakly_canonical(Shader.Path), *JafgInfo).replace_extension().replace_extension().generic_string();
        for (auto const& SourceFile: Shader.SourceFiles)
        {
            json::parse(finder::read_file(Shader2::GetReflectionFile(SourceFile)), nullptr, false).get_to(Result);
            for (auto& Entry: Result.EntryPoints)
            {
                if (Entry.Code.empty())
                {
                    Entry.Code = Shader2::GetDestination(SourceFile);
                }
            }
            check(exists(Result.EntryPoints.back().Code))
        }

        vk::ShaderStageFlags RequiredStages{vk::ShaderStageFlagBits::eVertex|vk::ShaderStageFlagBits::eFragment};
        for (auto const& EntryPoint: Result.EntryPoints)
        {
            RequiredStages &= EntryPoint.Stage;
        }
        if (RequiredStages != vk::ShaderStageFlags{})
        {
            LOG_FATAL(LogShaderSubsystem, "[{}]: Missing required shader stages [{}]. Failed to fetch shader."
                , Shader.Path, vk::to_string(RequiredStages)
                )
        }
    }

    LOG_VERBOSE(LogShaderSubsystem, "Fetched [{}] shaders.", this->ReflectedShaders.size())
}

LPath Jafg::JShaderSubsystem::Shader2::GetDestination(LPath const& SourceFile) noexcept
{
    std::error_code Ec;
    LPath Source{std::filesystem::absolute(SourceFile, Ec)};
    Source = (Ec ? SourceFile : Source).lexically_normal();

    LPath Destination{finder::temp_dir()/LITERAL_TEXT("ShaderCache")};

    if (Source.has_root_name())
    {
        LPath::string_type Root{Source.root_name().native()};
        std::erase_if(Root, [](auto const C) { return C == ':' || C == '\\' || C == '/'; });
        Destination /= Root;
    }

    Destination /= Source.relative_path();
    Destination += LITERAL_TEXT(".spv");

    return Destination;
}

LPath Jafg::JShaderSubsystem::Shader2::GetReflectionFile(LPath const& SourceFile) noexcept
{
    return Shader2::GetDestination(SourceFile).concat(".json");
}

LPath Jafg::JShaderSubsystem::Shader2::GetDependencyFile(LPath const& SourceFile) noexcept
{
    return Shader2::GetDestination(SourceFile).concat(".d");
}

namespace
{

//#
//# The newest write time of all files the compiled shader depends on (its source file and all of its includes).
//# Nothing if the dependency file is missing or malformed, or if any dependency does not exist anymore. In that case
//# the shader has to be recompiled, as we cannot tell whether it is up to date.
//#
NODISCARD std::optional<std::filesystem::file_time_type> GetNewestDependencyWriteTime(LPath const& DependencyFile)
{
    if (!is_regular_file(DependencyFile))
    {
        return {};
    }
    auto Content{finder::try_read_file(DependencyFile)};
    if (!Content)
    {
        return {};
    }

    //#
    //# Make syntax: "<target>: <dep> <dep> ...". The separator is the first colon followed by whitespace, so that
    //# drive letters of windows paths ("C:/...") are not mistaken for it. Paths never contain spaces, as slangc
    //# does not permit them anyway.
    //#
    LStringView Rest{*Content};
    std::size_t Separator{LStringView::npos};
    for (std::size_t Idx{0}; Idx + 1 < Rest.size(); ++Idx)
    {
        if (Rest[Idx] == ':' && std::isspace(static_cast<unsigned char>(Rest[Idx + 1])))
        {
            Separator = Idx;
            break;
        }
    }
    if (Separator == LStringView::npos)
    {
        return {};
    }
    Rest.remove_prefix(Separator + 1);

    std::optional<std::filesystem::file_time_type> Newest;
    while (!Rest.empty())
    {
        auto const Begin{Rest.find_first_not_of(" \t\r\n\\")};
        if (Begin == LStringView::npos)
        {
            break;
        }
        Rest.remove_prefix(Begin);
        auto const End{std::min(Rest.find_first_of(" \t\r\n"), Rest.size())};
        LPath Dependency{LString{Rest.substr(0, End)}};
        Rest.remove_prefix(End);

        std::error_code Error;
        auto const WriteTime{std::filesystem::last_write_time(Dependency, Error)};
        if (Error)
        {
            return {};
        }
        Newest = Newest ? std::max(*Newest, WriteTime) : WriteTime;
    }

    return Newest;
}

} /* ~Namespace <Anonymous> */

Jafg::LVertexInputProvider const& Jafg::JShaderSubsystem::GetVertexInput(LStringView Identifier) const noexcept
{
    auto& Providers{::GetStaticVertexInputProviders()};
    if (auto It{algo::find(Providers, Identifier, &LVertexInputProvider::Identifier)}; It != Providers.end())
    {
        return *It;
    }
    LOG_FATAL(LogShaderSubsystem, "[{}]: No such vertex input provider.", Identifier)
}

Jafg::LPushConstantProvider const& Jafg::JShaderSubsystem::GetPushConstant(LStringView Identifier) const noexcept
{
    auto& Providers{::GetStaticPushConstantProviders()};
    if (auto It{algo::find(Providers, Identifier, &LPushConstantProvider::Identifier)}; It != Providers.end())
    {
        return *It;
    }
    LOG_FATAL(LogShaderSubsystem, "[{}]: No such push constant provider.", Identifier)
}

Jafg::LBufferObjectProvider const& Jafg::JShaderSubsystem::GetBufferObject(LStringView Identifier) const noexcept
{
    auto& Providers{::GetStaticBufferObjectProviders()};
    if (auto It{algo::find(Providers, Identifier, &LBufferObjectProvider::Identifier)}; It != Providers.end())
    {
        return *It;
    }
    LOG_FATAL(LogShaderSubsystem, "[{}]: No such buffer object provider.", Identifier)
}

std::size_t Jafg::JShaderSubsystem::RecompileShaderConditionally(Shader2 const& Shader, bool bForce /* = false */)
{
    auto Result{0uz};

    auto Recompile{[&Result, &Shader](LPath const& Src, LPath const& Dst)
    {
        std::ostringstream SS; SS << Slangc;

        /* Quoting the requested in/out paths will not work, therefore, spaces are not permitted. */
        jassert(!algo::contains(Src.native(), LITERAL_TEXT(' ')) && "Slangc does not permit spaces in requested in/out paths.")
        jassert(!algo::contains(Dst.native(), LITERAL_TEXT(' ')) && "Slangc does not permit spaces in requested in/out paths.")
        SS << " " << Src.string();
        SS << " -o " << Dst.string();
        for (LString const& IncludeDirectory: Shader.IncludeDirs)
        {
            jassert(!algo::contains(IncludeDirectory, LITERAL_TEXT(' ')) && "Slangc does not permit spaces in include directory paths.")
            SS << " -I" << IncludeDirectory;
        }
        SS << " -target " << ::SlangTarget;
        SS << " -profile " << ::SlangProfile;
        SS << " -emit-spirv-directly";

        for (auto const& CompileTimeDefinition: Shader.CompileTimeDefinitions)
        {
            jassert(!algo::contains(CompileTimeDefinition.Identifier, LITERAL_TEXT(' ')) && "Slangc does not permit spaces in compile time definition identifiers.")
            jassert(!algo::contains(CompileTimeDefinition.Value, LITERAL_TEXT(' ')) && "Slangc does not permit spaces in compile time definition values.")
            SS << " -D" << CompileTimeDefinition.Identifier << "=" << CompileTimeDefinition.Value;
        }
#if JAFG_WITH_EDITOR
        SS << " -DJAFG_WITH_EDITOR=1";
#endif /* JAFG_WITH_EDITOR */

        SS << " -reflection-json " << Shader2::GetReflectionFile(Src).string();
        SS << " -depfile " << Shader2::GetDependencyFile(Src).string();

        auto SSStr{SS.str()};
        LOG_VERBOSE(LogSystem, "Executing: [{}].", SSStr)
        if (std::system(SSStr.c_str()) != 0)
        {
            LOG_FATAL(LogShaderSubsystem, "[{}]: Failed to compile shader from [{}] to [{}].", Shader.Path, Src, Dst)
        }

        ++Result;
    }};

    if (Shader.SourceFiles.empty())
    {
        LOG_FATAL(LogShaderSubsystem, "[{}]: No source files specified. Failed to compile.", Shader.Path)
    }

    for (auto& SourceFile: Shader.SourceFiles)
    {
        if (!exists(SourceFile))
        {
            LOG_FATAL(LogShaderSubsystem, "[{}]: Source file [{}] does not exist. Failed to compile.", Shader.Path, SourceFile)
        }

        auto Destination{Shader2::GetDestination(SourceFile)};
        check(Destination.has_parent_path())
        if (finder::create_directories(Destination.parent_path()))
        {
            LOG_TRACE(LogSystem, "[{}]: Created directories.", Destination.parent_path())
        }

        if (!is_regular_file(Destination))
        {
            if (exists(Destination))
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: Destination [{}] exists but is not a regular file. Failed to compile.", Shader.Path, Destination)
            }
            Recompile(SourceFile, Destination);
        }
        else if (auto ReflectionFile{Shader2::GetReflectionFile(SourceFile)}; !is_regular_file(ReflectionFile))
        {
            if (exists(ReflectionFile))
            {
                LOG_FATAL(LogShaderSubsystem, "[{}]: Reflection file [{}] exists but is not a regular file. Failed to compile.", Shader.Path, ReflectionFile)
            }
            Recompile(SourceFile, Destination);
        }
        else if (bForce)
        {
            Recompile(SourceFile, Destination);
        }
        else
        {
            //# Includes count as well. A shader is outdated as soon as any file it was compiled from changed.
            auto const NewestDependencyLwt{::GetNewestDependencyWriteTime(Shader2::GetDependencyFile(SourceFile))};
            auto SourceLwt{NewestDependencyLwt.value_or(last_write_time(SourceFile))};
            auto DestinationLwt{last_write_time(Destination)};
            if (!NewestDependencyLwt)
            {
                LOG_VERBOSE(LogShaderSubsystem, "[{}]: Dependencies of shader are unknown. Compiling from [{}] to [{}]."
                    , Shader.Path, SourceFile, Destination
                    )
                Recompile(SourceFile, Destination);
            }
            else if (SourceLwt > DestinationLwt)
            {
                LOG_VERBOSE(LogShaderSubsystem, "[{}]: Shader is outdated. Compiling from [{}] to [{}]; Because [{:%c} > {:%c}]."
                    , Shader.Path, SourceFile, Destination, SourceLwt, DestinationLwt
                    )
                Recompile(SourceFile, Destination);
            }
            else
            {
                LOG_TRACE(LogShaderSubsystem, "[{}]: Shader is up to date. No need to compile from [{}] to [{}]; Because [{:%c} <= {:%c}]."
                    , Shader.Path, SourceFile, Destination, SourceLwt, DestinationLwt
                    )
            }
        }
    }

    return Result;
}
