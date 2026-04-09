// Copyright mzoesch. All rights reserved.

#pragma once

#if !JAFG_PLATFORM_USES_STD_FINDER
    #error "JAFG_PLATFORM_USES_STD_FINDER is false."
#endif /* !JAFG_PLATFORM_USES_STD_FINDER */

#if PLATFORM_WINDOWS /* Just some windows nonsense... */
    #pragma push_macro( "CreateFile" )
    #undef CreateFile
#endif /* PLATFORM_WINDOWS */

inline LPath Finder::GetCwd()
{
    return LPath{ std::filesystem::current_path() };
}

inline bool Finder::DoesExist(const LPath& Path)
{
    return std::filesystem::exists(Path);
}

inline bool Finder::DoesFileExist(const LPath& File)
{
    return std::filesystem::is_regular_file(File);
}

inline bool Finder::DoesDirectoryExist(const LPath& Directory)
{
    return std::filesystem::is_directory(Directory);
}

inline void Finder::EnsureFile(const LPath& File)
{
    if (DoesExist(File) == false)
    {
        CreateFile(File, true);
    }

    return;
}

inline void Finder::CheckFile(const LPath& File)
{
    if (DoesFileExist(File) == false)
    {
        LOG_FATAL(LogSystem, "No such file: [{}].", File)
    }

    return;
}

inline bool Finder::AreFilesIdentical(const LPath& A, const LPath& B)
{
    LOG_TRACE(LogSystem, "Reading file [{}].", A)
    std::ifstream F1(A, std::ios::binary);

    LOG_TRACE(LogSystem, "Reading file [{}].", B)
    std::ifstream F2(B, std::ios::binary);

    if (!F1 || !F2)
    {
        return !F1 != !F2;
    }

    LJafgChar C1;
    LJafgChar C2;
    while (true)
    {
        if (F1.get(C1).fail())
        {
            F2.get(C2);
            break;
        }
        if (F2.get(C2).fail())
        {
            break;
        }

        if (C1 != C2)
        {
            return false;
        }

        continue;
    }

    return F1.eof() && F2.eof();
}

inline void Finder::CreateFile(const LPath& File, const bool bMakeParents)
{
    if (bMakeParents)
    {
        CreateDirectories(File.parent_path());
    }

    if (std::ofstream Out{ File }; Out.fail())
    {
        LOG_FATAL(LogSystem, "Failed to create file: [{}].", File.string())
    }
    else
    {
        Out.close();
    }

    return;
}

inline void Finder::CreateDirectories(const LPath& Directory)
{
    if (std::filesystem::create_directories(Directory))
    {
        LOG_VERBOSE(LogSystem, "Created directory [{}].", Directory.string())
    }

    return;
}

inline LString Finder::ReadFile(const LPath& File)
{
    LString Error;
    TOptional Out { TryReadFile(File, &Error) };

    if (!Out)
    {
        LOG_FATAL(LogSystem, "{}", Error)
    }

    LString Val { std::move(Out.value()) };
    return Val;
}

inline TArray<u8> Finder::ReadFileAsBinary(const LPath& File)
{
    LString Error;
    TOptional Out { TryReadFileAsBinary(File, &Error) };

    if (!Out)
    {
        LOG_FATAL(LogSystem, "{}", Error)
    }

    TArray Val { std::move(Out.value()) };
    return Val;
}

inline TOptional<LString> Finder::TryReadFile(const LPath& File, LString* OutHumanReadableError)
{
    LOG_TRACE(LogSystem, "Reading file [{}].", File)

    const std::ifstream F{File, std::ios::in | std::ios::binary};

    if (F.fail())
    {
        if (OutHumanReadableError)
        {
            *OutHumanReadableError = Jafg::SprintF("Failed to open file: [{}].", File);
        }

        return { };
    }

    std::ostringstream Buffer; Buffer << F.rdbuf();
    return Buffer.str();
}

inline TOptional<TArray<u8>> Finder::TryReadFileAsBinary(const LPath& File, LString* OutHumanReadableError)
{
    LOG_TRACE(LogSystem, "Reading file [{}].", File)

    std::ifstream F{ File, std::ios::binary | std::ios::ate};
    if (F.fail())
    {
        if (OutHumanReadableError)
        {
            *OutHumanReadableError = Jafg::SprintF("Failed to open file at [{}].", File);
        }

        return { };
    }

    TArray<u8> Buffer;
    Jafg::AddUninitialized(&Buffer, static_cast<TArray<u8>::size_type>(F.tellg()));

    F.seekg(0, std::ios::beg);

    if (F.read(reinterpret_cast<LJafgChar*>(Buffer.data()), Buffer.size()).fail())
    {
        F.close();
        if (OutHumanReadableError)
        {
            *OutHumanReadableError = Jafg::SprintF("Failed to read file at [{}].", File);
        }

        return { };
    }

    F.close();

    return Buffer;
}

inline void Finder::OverrideFile(LPath const& File, LStringView Content, bool bUseNativeLineEndings)
{
    if (DoesFileExist(File) == false)
    {
        CreateFile(File, true);
    }

    std::ofstream Out
    {
        File,
        std::ios::out | std::ios::trunc | (bUseNativeLineEndings ? static_cast<std::ios::openmode>(0) : std::ios::binary)
    };

    Out.write(&*Content.begin(), Content.size());
    Out.close();

    LOG_TRACE(LogSystem, "File [{}] overridden.", File )

    return;
}

inline void Finder::MakeFileBackup(LPath const& File, bool bMakeIfSame /* = false */, i32 Count /* = 5 */, LStringView Extension /* = ".old" */)
{
    LOG_VERBOSE(LogSystem, "Making backup of file [{}].", File)

    if (Count < 1)
    {
        LOG_WARNING(LogSystem, "Received invalid backup count [{}]. Dodging to one backup.", Count)
        Count = 1;
    }

    if (DoesFileExist(File) == false)
    {
        LOG_FATAL(LogSystem, "No such file: [{}].", File)
        return;
    }

    if (bMakeIfSame)
    {
        LPath MostRecent { File };
        MostRecent.concat(Extension);
        MostRecent.concat("1");

        EnsureFile(MostRecent);
        if (AreFilesIdentical(File, MostRecent))
        {
            return;
        }
    }

    check( Count > 0 )
    for (; Count != 0; --Count)
    {
        checkSlow( Count > 0 )

        LPath Target { File };
        Target.append(Extension);
        Target.append(std::format("{}", Count));

        LPath Previous;
        if (Count == 1)
        {
            Previous.assign(File);
        }
        else
        {
            Previous.assign(File);
            Previous.append(Extension);
            Previous.append(std::format("{}", Count - 1));
        }

        if (DoesFileExist(Previous))
        {
            EnsureFile(Target);
            std::filesystem::copy(Previous, Target, std::filesystem::copy_options::overwrite_existing);
        }

        continue;
    }

    return;
}

inline TArray<LPath> Finder::FindFiles
(
    const LPath& Directory,
    const bool bKeepExtension /* = true */,
    const LStringView& Extension /* = "*" */,
    std::regex_constants::syntax_option_type Options /* = std::regex_constants::ECMAScript */
)
{
    if (DoesDirectoryExist(Directory) == false)
    {
        return {};
    }

    std::regex Pattern;
    if (Extension != "*")
    {
        Pattern = std::regex{Extension.begin(), Extension.end(), Options};
    }

    TArray<LPath> Out;
    for (auto& P : std::filesystem::directory_iterator{Directory})
    {
        if (P.is_directory())
        {
            continue;
        }

        if (Extension == "*" || std::regex_match(P.path().extension().string(), Pattern))
        {
            if (bKeepExtension)
            {
                Out.emplace_back(P.path().filename());
            }
            else
            {
                Out.emplace_back(P.path().stem());
            }
        }

        continue;
    }

    return Out;
}

inline TArray<LPath> Finder::FindFilesRecursively
(
    const LPath& Directory,
    const bool bKeepExtension /* = true */,
    LStringView Regex /* = "*" */,
    std::regex_constants::syntax_option_type Options /* = std::regex_constants::ECMAScript */
)
{
    if (DoesDirectoryExist(Directory) == false)
    {
        return {};
    }

    TArray<LPath> Out;

    std::basic_regex<LPath::value_type> Pattern;
    if (Regex != "*")
    {
        Pattern = std::basic_regex<LPath::value_type>{Regex.begin(), Regex.end(), Options };
    }

    for (const std::filesystem::directory_entry& P : std::filesystem::recursive_directory_iterator
         {
             Directory,
             std::filesystem::directory_options::skip_permission_denied
             | std::filesystem::directory_options::follow_directory_symlink
         })
    {
        if (P.is_directory())
        {
            continue;
        }

        if (Regex == "*" || std::regex_match(P.path().native(), Pattern))
        {
            if (bKeepExtension)
            {
                Out.emplace_back(LPath{P.path()}.make_preferred());
            }
            else
            {
                Out.emplace_back(LPath{P.path()}.replace_extension().make_preferred());
            }
        }

        continue;
    }

    return Out;
}

inline TArray<LPath> Finder::FindFilesRecursivelyByName(LPath const& Directory, std::basic_string_view<LPath::value_type> FileName)
{
    if (DoesDirectoryExist(Directory) == false)
    {
        return {};
    }

    TArray<LPath> Out;

    for (const std::filesystem::directory_entry& P : std::filesystem::recursive_directory_iterator
         {
             Directory,
             std::filesystem::directory_options::skip_permission_denied
             | std::filesystem::directory_options::follow_directory_symlink
         })
    {
        if (P.is_directory())
        {
            continue;
        }

        if (auto F{P.path().filename()}; FileName == F)
        {
            Out.emplace_back(LPath{P.path()}.make_preferred());
        }

        continue;
    }

    return Out;
}

#if PLATFORM_WINDOWS
    #pragma pop_macro( "CreateFile" )
#endif /* PLATFORM_WINDOWS */
