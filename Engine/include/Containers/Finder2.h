// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace finder
{

using namespace std::filesystem;


///////////////////////////////////////////////////////////////////////////////
// Jafg paths.
///////////////////////////////////////////////////////////////////////////////

//# Default configs for the engine. User configs should never be here.
constexpr path default_config_dir() noexcept { return LITERAL_TEXT("Config"); }

//# Everything that should be saved for a longer period of time. But if they get deleted by the user, it's ok.
constexpr path temp_dir() noexcept { return LITERAL_TEXT("Temp"); }
constexpr path dumps_dir() noexcept { return temp_dir()/LITERAL_TEXT("Dumps"); }
constexpr path most_recent_mem_dump_file() noexcept { return dumps_dir()/LITERAL_TEXT("proc.dmp"); }
constexpr path most_recent_stack_trace_file() noexcept { return dumps_dir()/LITERAL_TEXT("stack.trace"); }

//#
//# User folder. Store user specific stuff here. That should be saved between sessions and for a long time.
//# If a user uninstalls the program, this folder should contain only files that they might want to keep, for a
//# potential reinstallation.
//# This contains user preferences, progression data, etc.
//#
constexpr path saved_dir() noexcept { return LITERAL_TEXT("Saved"); }
constexpr path user_preferences_file() noexcept { return saved_dir()/LITERAL_TEXT("MyPreferences.cfg"); }

//# Content path for each module.
constexpr path content_dir() noexcept { return LITERAL_TEXT("Content"); }

namespace detail
{

//#
//# For internal purposes only. Do not use.
//# After ssi the engine will always have its cwd to the engine root. You should therefore use #finder::current_path.
//#
ENGINE_API path _engine_root_dir_slow();
//# Retrieve the proc file for self.
ENGINE_API path self_proc_slow();
//# Retrieve the proc dir for self.
inline path self_proc_dir_slow()
{
    path Result{self_proc_slow()};
    if (Result.has_parent_path())
    {
        Result = Result.parent_path();
    }
    return Result;
}

//#
//# When in a corrupted state, avoid touching the heap. Therefore, this is a legal way to access the
//# path for the target core dump. It is guaranteed to be always valid and accessible.
//#
//# Modifying this variable in any way is UB und a race condition which could lead to catastrophic SIGSEGVs.
//#
ENGINE_API extern path dump_file;
#if JAFG_PLATFORM_LINUX
//# Same as #DumpFile -- no modifying allowed.
ENGINE_API extern std::optional<path> _lnx_gdb;
#endif /* JAFG_PLATFORM_LINUX */

} /* ~Namespace detail */


///////////////////////////////////////////////////////////////////////////////
// std::filesystem utilities.
///////////////////////////////////////////////////////////////////////////////

enum EMakeParents: u8
{
    MakeParents,
    SkipParents,
};
enum ELineEndings: u8
{
    NativeLineEndings,
    UnixLineEndings,
};

template<EMakeParents InMakeParents = SkipParents>
inline void ensure_file(path const& Path)
{
    if (!is_regular_file(Path))
    {
        if constexpr (InMakeParents == MakeParents)
        {
            if (Path.has_parent_path())
            {
                create_directories(Path.parent_path());
            }
        }
        if (std::ofstream Out{Path}; !Out)
        {
            LOG_FATAL(LogSystem, "[{}]: Failed to create file.", Path)
        }
    }
}

inline bool descendant_of(path const& Descendant, path const& Base = ".")
{
    auto Relative{relative(weakly_canonical(Descendant), weakly_canonical(Base))};
    return !Relative.empty() && Relative != "." && *Relative.begin() != "..";
}

inline std::size_t directory_entry_count(path const& Path)
{
    std::size_t Count{0};
    algo::for_each(directory_iterator{Path}, [&Count](auto const&){ ++Count; });
    return Count;
}

inline LString human_readable_time(file_time_type const& Time)
{
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            Time - std::filesystem::file_time_type::clock::now()
            + std::chrono::system_clock::now()
        );

    std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&cftime), "%d.%m.%Y %H:%M");
    return ss.str();
}
inline LString human_readable_entry_size(directory_entry const& Entry)
{
    if (Entry.is_directory())
    {
        return algo::sprintf("{} items", directory_entry_count(Entry.path()));
    }

    auto Size{Entry.file_size()};
    if (Size < 1024)
    {
        return algo::sprintf("{} B", Size);
    }
    if (Size < 1024 * 1024)
    {
        return algo::sprintf("{:.2f} KiB", static_cast<f64>(Size) / 1024.0);
    }
    if (Size < 1024 * 1024 * 1024)
    {
        return algo::sprintf("{:.2f} MiB", static_cast<f64>(Size) / (1024.0 * 1024));
    }
    if (Size < 1024ull * 1024 * 1024 * 1024)
    {
        return algo::sprintf("{:.2f} GiB", static_cast<f64>(Size) / (1024.0 * 1024 * 1024));
    }
    return algo::sprintf("{:.2f} TiB", static_cast<f64>(Size) / (1024.0 * 1024 * 1024 * 1024));
}

inline std::optional<TArray<std::byte>> try_read_binary_file(path const& Path)
{
    std::ifstream F{Path, std::ios::binary};
    if (!F)
    {
        return {};
    }

    F.seekg(0, std::ios::end);
    std::streampos end{F.tellg()};
    if (end == std::ifstream::pos_type{-1})
    {
        return {};
    }

    TArray<std::byte> Result(static_cast<TArray<std::byte>::size_type>(end));
    F.seekg(0, std::ios::beg);
    if (!F.read(reinterpret_cast<std::ifstream::char_type*>(Result.data()), static_cast<i64>(Result.size())))
    {
        return {};
    }

    return Result;
}
inline TArray<std::byte> read_binary_file(path const& Path)
{
    std::optional<TArray<std::byte>> Result{try_read_binary_file(Path)};
    if (!Result)
    {
        LOG_FATAL(LogSystem, "[{}]: Failed to read." , Path)
    }
    return std::move(*Result);
}

inline std::optional<LString> try_read_file(path const& Path)
{
    std::ifstream F{Path, std::ios::binary};
    if (!F)
    {
        return {};
    }

    F.seekg(0, std::ios::end);
    std::streampos end{F.tellg()};
    if (end == std::ifstream::pos_type{-1})
    {
        return {};
    }

    LString Result(static_cast<LString::size_type>(end), '\0');
    F.seekg(0, std::ios::beg);
    if (!F.read(Result.data(), static_cast<i64>(Result.size())))
    {
        return {};
    }

    return Result;
}
inline LString read_file(path const& Path)
{
    std::optional<LString> Result{try_read_file(Path)};
    if (!Result)
    {
        LOG_FATAL(LogSystem, "[{}]: Failed to read." , Path)
    }
    return std::move(*Result);
}

inline bool identical_file_content(path const& A, path const& B)
{
    std::ifstream F1(A, std::ios::binary);
    std::ifstream F2(B, std::ios::binary);
    if (!F1 || !F2)
    {
        return !F1 != !F2;
    }

    std::ifstream::char_type C1;
    std::ifstream::char_type C2;
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
    }

    return F1.eof() && F2.eof();
}

template<EMakeParents InMakeParents = SkipParents, ELineEndings InLineEndings = UnixLineEndings>
void override_file(path const& Path, LStringView Content)
{
    ensure_file<InMakeParents>(Path);

    std::ofstream Out{Path,
        std::ios::out | std::ios::trunc | (InLineEndings == NativeLineEndings ? static_cast<std::ios::openmode>(0) : std::ios::binary)};
    if (!Out)
    {
        LOG_FATAL(LogSystem, "[{}]: Failed to open file.", Path)
    }

    Out.write(Content.data(), Content.size());
}

//# @return True, if the file was overridden.
template<EMakeParents InMakeParents = SkipParents, ELineEndings InLineEndings = UnixLineEndings>
bool override_file_if_different(path const& Path, LStringView Content)
{
    static_assert(InLineEndings == UnixLineEndings, "This function does not support native line endings.");

    ensure_file<InMakeParents>(Path);

    std::ostringstream Buffer;
    {
        std::ifstream F{Path, std::ios::binary};
        if (!F)
        {
            LOG_FATAL(LogSystem, "[{}]: Failed to open file.", Path)
        }
        Buffer << F.rdbuf();
    }

    if (Buffer.str() != Content)
    {
        override_file<InMakeParents, InLineEndings>(Path, Content);
        return true;
    }
    return false;
}

inline void backup_file(path const& Path, u32 Count, LStringView Extension = ".old")
{
    if (!is_regular_file(Path))
    {
        LOG_FATAL(LogSystem, "[{}]: No such file.", Path)
    }

    auto GetForCount{[&](u32 Count)
    {
        if (Count == 0)
        {
            return Path;
        }
        LPath Result{Path}; Result.append(Extension); Result.append(std::format("{}", Count - 1));
        return Result;
    }};

    if (auto MostRecent{GetForCount(1)}; is_regular_file(MostRecent))
    {
        /* Do not do backups, if the current file's most recent backup is up to date. */
        if (identical_file_content(Path, MostRecent))
        {
            return;
        }
    }

    for (; Count != 0; --Count)
    {
        LPath Backup{Path}; Backup.append(Extension); Backup.append(std::format("{}", Count));
        LPath Previous{GetForCount(Count)};
        if (is_regular_file(Previous))
        {
            ensure_file(Backup);
            copy(Previous, Backup, copy_options::overwrite_existing);
        }
    }
}

template<typename TIter>
TArray<path> retrieve_files(path const& Base, LStringView Regex = ".*", std::regex_constants::syntax_option_type Flags = std::regex_constants::ECMAScript)
{
    if (!is_directory(Base))
    {
        return {};
    }

    TArray<path> Result;
    std::basic_regex Pattern{std::basic_regex<LChar>{Regex.begin(), Regex.end(), Flags}};

    for (directory_entry const& Entry : TIter{Base, directory_options::skip_permission_denied})
    {
        if (!Entry.is_directory())
        {
            if (std::regex_match(Entry.path().native(), Pattern))
            {
                Result.emplace_back(path{Entry.path()}.make_preferred());
            }
        }

        continue;
    }

    return Result;
}

} /* ~Namespace finder */
