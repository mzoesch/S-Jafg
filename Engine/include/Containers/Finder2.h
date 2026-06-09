// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace finder
{

using namespace std::filesystem;

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

inline std::size_t directory_entry_count(path const& Path)
{
    std::size_t Count{0};
    algo::for_each(directory_iterator{Path}, [&Count](auto const&){ ++Count; });
    return Count;
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
        return algo::sprintf("{:.2f} KiB", Size / 1024.0);
    }
    if (Size < 1024 * 1024 * 1024)
    {
        return algo::sprintf("{:.2f} MiB", Size / (1024.0 * 1024));
    }
    if (Size < 1024ull * 1024 * 1024 * 1024)
    {
        return algo::sprintf("{:.2f} GiB", Size / (1024.0 * 1024 * 1024));
    }
    return algo::sprintf("{:.2f} TiB", Size / (1024.0 * 1024 * 1024 * 1024));
}

// @return True, iff A is a descendant of B.
inline bool descendant_of(path const& A, path const& B = ".")
{
    auto Relative{relative(weakly_canonical(A), weakly_canonical(B))};
    return !Relative.empty() && Relative != "." && *Relative.begin() != "..";
}

} /* ~Namespace finder */
