// Copyright mzoesch. All rights reserved.

#include "Framework/ShaderSubsystem.h"
#include "Platform/PlatformMisc.h"

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

} /* ~Namespace <Anonymous> */

void Jafg::JShaderSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    if (Finder::DoesFileExist(Slangc) == false)
    {
        LOG_FATAL(LogShaderSubsystem, "No such file: [{}].", Slangc)
    }

    this->DefaultRequestTemplate.IncludeDirectories = this->GetDefaultShaderIncludeDirectories();
    this->DefaultRequestTemplate.EntryPoints.reflexive_emplace_back("vertMain").emplace_back("fragMain");

    this->RecompileChangedShaders();

    return;
}

void Jafg::JShaderSubsystem::RecompileChangedShaders()
{
    LOG_VERBOSE(LogShaderSubsystem, "Recompiling changed shaders...")

    auto Recompiled{0uz};

    if (Finder::DoesDirectoryExist(Finder::GetSlangShadersDir()) == false)
    {
        LOG_FATAL(LogShaderSubsystem, "Slang shaders directory [{}] does not exist.", Finder::GetSlangShadersDir())
    }
    if (Finder::DoesDirectoryExist(Finder::GetSpirShadersDir()) == false)
    {
        Finder::CreateDirectories(Finder::GetSpirShadersDir());
    }

    for (auto& Entry : std::filesystem::directory_iterator{Finder::GetSlangShadersDir()})
    {
        if (Entry.is_directory())
        {
            continue;
        }
        if (Entry.path().extension() != LITERAL_TEXT(".slang"))
        {
            continue;
        }

        auto Dst{Finder::GetSpirShadersDir() / Entry.path().filename().replace_extension(LITERAL_WIDE(".spv"))};

        if (Finder::DoesFileExist(Dst) == false)
        {
            LOG_VERBOSE(LogShaderSubsystem, "Shader [{}] does not exist. Compiling from [{}].", Dst, Entry.path())
            this->DefaultRequestTemplate.In = Entry.path();
            this->DefaultRequestTemplate.Out = Dst;
            if (this->RecompileShader(this->DefaultRequestTemplate) != 0)
            {
                LOG_FATAL(LogShaderSubsystem, "Failed to compile shader [{}] from [{}].", Dst, Entry.path())
            }
            ++Recompiled;
            continue;
        }

        auto EntryLastWriteTime{std::filesystem::last_write_time(Entry.path())};
        auto DstLastWriteTime{std::filesystem::last_write_time(Dst)};

        if (EntryLastWriteTime > DstLastWriteTime)
        {
            LOG_VERBOSE(LogShaderSubsystem, "Shader [{}] is outdated. Compiling from [{}]; Because [{:%c} > {:%c}]."
                , Dst, Entry.path(), EntryLastWriteTime, DstLastWriteTime
                )

            this->DefaultRequestTemplate.In = Entry.path();
            this->DefaultRequestTemplate.Out = Dst;
            if (this->RecompileShader(this->DefaultRequestTemplate) != 0)
            {
                LOG_FATAL(LogShaderSubsystem, "Failed to compile shader [{}] from [{}].", Dst, Entry.path())
            }
            ++Recompiled;
            continue;
        }

        LOG_TRACE(LogShaderSubsystem, "Shader [{}] is up to date. No need to compile from [{}]; Because [{:%c} <= {:%c}]."
            , Dst, Entry.path(), EntryLastWriteTime, DstLastWriteTime
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

TArray<LString> Jafg::JShaderSubsystem::GetDefaultShaderIncludeDirectories() const
{
    return {"Content/Shaders/Slang"};
}
