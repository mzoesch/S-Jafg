// Copyright mzoesch. All rights reserved.

#include "Lal.afx"

#if LAL_PLATFORM_USES_STD_FINDER

#if PLATFORM_WINDOWS /* Just some windows nonsense... */
    #pragma push_macro( "CreateFile" )
    #undef CreateFile
#endif /* PLATFORM_WINDOWS */

namespace
{

bool DoesExist(const std::filesystem::path& Path);
bool DoesFileExist(const std::filesystem::path& File);
bool DoesDirectoryExist(const std::filesystem::path& Directory);
void CreateFile(const std::filesystem::path& File, const bool bMakeParents);
void CreateDirectories(const std::filesystem::path& Directory);

bool DoesExist(const std::filesystem::path& Path)
{
    return std::filesystem::exists(Path);
}

bool DoesFileExist(const std::filesystem::path& File)
{
    return std::filesystem::is_regular_file(File);
}

bool DoesDirectoryExist(const std::filesystem::path& Directory)
{
    return std::filesystem::is_directory(Directory);
}

void CreateFile(const std::filesystem::path& File, const bool bMakeParents)
{
    if (bMakeParents)
    {
        ::CreateDirectories(File.parent_path());
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

void CreateDirectories(const std::filesystem::path& Directory)
{
    if (std::filesystem::create_directories(Directory))
    {
        LOG_VERBOSE(LogSystem, "Created directory [{}].", Directory.string())
    }

    return;
}

} /* ~Namespace <Anonymous> */

LPath Finder::GetCwd()
{
    const std::filesystem::path Cwd{std::filesystem::current_path()};
    return LPath{ Cwd.native().cbegin().base(), Cwd.native().cend().base() };
}

bool Finder::DoesExist(const LPathView& Path)
{
    return ::DoesExist(std::filesystem::path{Path.begin_ptr(), Path.end_ptr()});
}

bool Finder::DoesFileExist(const LPathView& File)
{
    return ::DoesFileExist(std::filesystem::path{File.begin_ptr(), File.end_ptr()});
}

bool Finder::DoesDirectoryExist(const LPathView& Directory)
{
    return ::DoesDirectoryExist(std::filesystem::path{Directory.begin_ptr(), Directory.end_ptr()});
}

void Finder::EnsureFile(const LPathView& File)
{
    if (const std::filesystem::path StdPath{File.begin_ptr(), File.end_ptr()}; ::DoesFileExist(StdPath) == false)
    {
        ::CreateFile(StdPath, true);
    }

    return;
}

void Finder::CheckFile(const LPathView& File)
{
    if (::DoesFileExist(std::filesystem::path{File.begin_ptr(), File.end_ptr()}) == false)
    {
        LOG_FATAL(LogSystem, "No such file: [{}].", File)
    }

    return;
}

bool Finder::AreFilesIdentical(const LPathView& A, const LPathView& B)
{

    LOG_TRACE(LogSystem, "Reading file [{}].", A)
    std::ifstream F1(std::filesystem::path{A.begin_ptr(), A.end_ptr()}, std::ios::binary);

    LOG_TRACE(LogSystem, "Reading file [{}].", B)
    std::ifstream F2(std::filesystem::path{B.begin_ptr(), B.end_ptr()}, std::ios::binary);

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

void Finder::CreateFile(const LPathView& File, const bool bMakeParents)
{
    ::CreateFile(std::filesystem::path{File.begin_ptr(), File.end_ptr()}, bMakeParents);
}

void Finder::CreateDirectories(const LPathView& Directory)
{
    ::CreateDirectories(std::filesystem::path{Directory.begin_ptr(), Directory.end_ptr()});
}

LString Finder::ReadFile(const LPathView& File)
{
    LString Error;
    Jafg::TOptional<LString> Out { TryReadFile(File, &Error) };

    if (!Out)
    {
        LOG_FATAL(LogSystem, "{}", Error)
    }

    LString Val { std::move(Out.GetValue()) };
    check( Out.GetValue().IsEmpty() )
    return Val;
}

TArray<u8> Finder::ReadFileAsBinary(const LPathView& File)
{
    LString Error;
    Jafg::TOptional<TArray<u8>> Out { TryReadFileAsBinary(File, &Error) };

    if (!Out)
    {
        LOG_FATAL(LogSystem, "{}", Error)
    }

    TArray<u8> Val { std::move(Out.GetValue()) };
    check( Out.GetValue().IsEmpty() )
    return Val;
}

Jafg::TOptional<LString> Finder::TryReadFile(const LPathView& File, LString* OutHumanReadableError)
{
    LOG_TRACE(LogSystem, "Reading file [{}].", File)

    const std::ifstream F(std::filesystem::path{File.begin_ptr(), File.end_ptr()}, std::ios::in | std::ios::binary);

    if (F.fail())
    {
        if (OutHumanReadableError)
        {
            *OutHumanReadableError = LString::SprintF("Failed to open file: [{}].", File);
        }

        return { };
    }

    std::ostringstream Buffer; Buffer << F.rdbuf();
    std::string S { Buffer.str() };
    return LString{ S.begin().base(), S.end().base() };
}

Jafg::TOptional<TArray<u8>> Finder::TryReadFileAsBinary(const LPathView& File, LString* OutHumanReadableError)
{
    LOG_TRACE(LogSystem, "Reading file [{}].", File)

    std::ifstream F
    {
        std::filesystem::path{File.begin_ptr(), File.end_ptr()},
        std::ios::binary | std::ios::ate
    };

    if (F.fail())
    {
        if (OutHumanReadableError)
        {
            *OutHumanReadableError = LString::SprintF("Failed to open file at [{}].", File);
        }

        return { };
    }

    TArray<u8> Buffer; Buffer.AddUninitialized(F.tellg());

    F.seekg(0, std::ios::beg);

    if (F.read(reinterpret_cast<LJafgChar*>(Buffer.GetDataPointer()), Buffer.GetSize()).fail())
    {
        F.close();
        if (OutHumanReadableError)
        {
            *OutHumanReadableError = LString::SprintF("Failed to read file at [{}].", File);
        }

        return { };
    }

    F.close();

    return Buffer;
}

void Finder::OverrideFile(const LPathView& File, const LStringView& Content, const bool bUseNativeLineEndings)
{
    std::filesystem::path F{File.begin_ptr(), File.end_ptr()};

    if (::DoesFileExist(F) == false)
    {
        ::CreateFile(F, true);
    }

    std::ofstream Out
    {
        F,
        std::ios::out | std::ios::trunc | (bUseNativeLineEndings ? static_cast<std::ios::openmode>(0) : std::ios::binary)
    };

    Out.write(Content.begin_ptr(), Content.GetRuneCount());
    Out.close();

    LOG_TRACE(LogSystem, "File [{}] overridden.", File )

    return;
}

void Finder::MakeFileBackup(const LPathView& File, const bool bMakeIfSame /* = false */, i32 Count /* = 5 */, const LStringView& Extension /* = ".old" */)
{
    LOG_VERBOSE(LogSystem, "Making backup of file [{}].", File)

    if (Count < 1)
    {
        LOG_WARNING(LogSystem, "Received invalid backup count [{}]. Dodging to one backup.", Count)
        Count = 1;
    }

    const std::filesystem::path F{File.begin_ptr(), File.end_ptr()};

    if (::DoesFileExist(F) == false)
    {
        LOG_FATAL(LogSystem, "No such file: [{}].", File)
        return;
    }

    if (bMakeIfSame)
    {
        const LPath MostRecent { File.AppendPathToNew<LPath>(Extension) / '1' };
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

        const LPath Target { File.AppendPathToNew<LPath>(Extension) / std::to_string(Count).c_str() };

        LPath Previous;
        if (Count == 1)
        {
            Previous.Assign(File);
        }
        else
        {
            Previous = File.AppendPathToNew<LPath>(Extension) / std::to_string(Count - 1).c_str();
        }

        const std::filesystem::path StdTarget {Target.begin_ptr(), Target.end_ptr()};
        const std::filesystem::path StdPrevious {Previous.begin_ptr(), Previous.end_ptr()};

        if (::DoesFileExist(StdPrevious))
        {
            EnsureFile(Target);
            std::filesystem::copy(StdPrevious, StdTarget, std::filesystem::copy_options::overwrite_existing);
        }

        continue;
    }

    return;
}

TArray<LString> Finder::FindFiles
(
    const LPathView& Directory,
    const bool bKeepExtension /* = true */,
    const LStringView& Extension /* = "*" */,
    std::regex_constants::syntax_option_type Options /* = std::regex_constants::ECMAScript */
)
{
    std::filesystem::path D { Directory.begin_ptr(), Directory.end_ptr() };

    if (::DoesDirectoryExist(D) == false)
    {
        return { };
    }

    std::regex Pattern;
    if (Extension != '*')
    {
        Pattern = std::regex{ std::string{Extension.begin_ptr(), Extension.end_ptr()} };
    }

    TArray<LString> Out;
    for (auto& P : std::filesystem::directory_iterator{D})
    {
        if (P.is_directory())
        {
            continue;
        }

        if (Extension == '*' || std::regex_match(P.path().extension().string(), Pattern))
        {
            if (bKeepExtension)
            {
                const std::string S { P.path().filename().string() };
                Out.Emplace(LString{ S.c_str(), S.size() });
            }
            else
            {
                const std::string S { P.path().stem().string() };
                Out.Emplace( LString{ S.c_str(), S.size() });
            }
        }

        continue;
    }

    return Out;
}

TArray<LString> Finder::FindFilesRecursively
(
    const LPathView& Directory,
    const bool bKeepExtension /* = true */,
    const LStringView& Extension /* = "*" */,
    std::regex_constants::syntax_option_type Options /* = std::regex_constants::ECMAScript */
)
{
    const std::filesystem::path D { Directory.begin_ptr(), Directory.end_ptr() };

    if (::DoesDirectoryExist(D) == false)
    {
        return { };
    }

    TArray<LString> Out;

    std::regex Pattern;
    if (Extension != '*')
    {
        Pattern = std::regex{ std::string{Extension.begin_ptr(), Extension.end_ptr()}, Options };
    }

    for (const std::filesystem::directory_entry& P : std::filesystem::recursive_directory_iterator
         {
             D,
             std::filesystem::directory_options::skip_permission_denied
             | std::filesystem::directory_options::follow_directory_symlink
         })
    {
        if (P.is_directory())
        {
            continue;
        }

        if (Extension == '*' || std::regex_match(P.path().native(), Pattern))
        {
            if (bKeepExtension)
            {
                Out.Emplace(LString{ P.path().native().begin().base(), P.path().native().size() });
            }
            else
            {
                std::filesystem::path NoExtension { P.path().native() };
                NoExtension.replace_extension();
                Out.Emplace( LString{ NoExtension.native().begin().base(), NoExtension.native().size() } );
            }
        }

        continue;
    }

    return Out;
}

TArray<LString> Finder::FindFilesRecursivelyByName(const LPathView& Directory, const LStringView& FileName)
{
    const std::filesystem::path D { Directory.begin_ptr(), Directory.end_ptr() };

    if (::DoesDirectoryExist(D) == false)
    {
        return { };
    }

    TArray<LString> Out;

    for (const std::filesystem::directory_entry& P : std::filesystem::recursive_directory_iterator
         {
             D,
             std::filesystem::directory_options::skip_permission_denied
             | std::filesystem::directory_options::follow_directory_symlink
         })
    {
        if (P.is_directory())
        {
            continue;
        }

        if (const std::string F { P.path().filename().string() }; FileName.Equals(F.begin().base(), F.end().base()))
        {
            Out.Emplace(LString{ P.path().native().begin().base(), P.path().native().size() });
        }

        continue;
    }

    return Out;
}

#if PLATFORM_WINDOWS
    #pragma pop_macro( "CreateFile" )
#endif /* PLATFORM_WINDOWS */

#endif /* LAL_PLATFORM_USES_STD_FINDER */
