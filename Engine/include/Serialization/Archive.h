// Copyright mzoesch. All rights reserved.

#pragma once

namespace serde
{

//# Non-intrusive unimplemented enum map getter. Define your own in a types namespace to find it with adl.
template<typename T> inline void get_enum_map(std::unordered_map<T, LStringView> const** Map) noexcept = delete;

//#
//# A generic enum map macro. For easy transformation.
//# Usage example:
//#     enum struct EExample: u8 { A, B, C };
//#     SERDE_ENUM(EExample, A, B, C)
//#
#define DETAIL_SERDE_ENUM_TRANSFORM(X) {_serde_local_enum_t::X, #X},
#define SERDE_ENUM(T, ...)                                                        \
    inline void get_enum_map(std::unordered_map<T, LStringView> const** Map) noexcept \
    {                                                                                 \
        check(Map)                                                                    \
        static_assert(std::is_enum_v<T>);                                             \
        typedef T _serde_local_enum_t;                                                \
        static std::unordered_map<T, LStringView> Members                             \
        {                                                                             \
            JAFG_MAP(DETAIL_SERDE_ENUM_TRANSFORM, __VA_ARGS__)                    \
        };                                                                            \
        *Map = &Members;                                                              \
    }
//#
//# A generic enum map macro for enums where the string counterpart is different from the C++ record name.
//# Usage example:
//#     enum struct EExample: u8 { A, B, C };
//#     SERDE_ENUM_PAIR(EExample, {
//#         {EExample::A, "a"}, {EExample::B, "b"}, {EExample::C, "c"}
//#         })
//#
#define SERDE_ENUM_PAIR(T, ...)                                                   \
    inline void get_enum_map(std::unordered_map<T, LStringView> const** Map) noexcept \
    {                                                                                 \
        check(Map)                                                                    \
        static_assert(std::is_enum_v<T>);                                             \
        typedef T _serde_local_enum_t;                                                \
        static std::unordered_map<T, LStringView> Members __VA_ARGS__;                \
        *Map = &Members;                                                              \
    }

//#
//# Then retrieve the map with this function.
//#     E.g. auto& Map{serde::enum_map<EExample>()};
//# The JSON and archive systems will use these templates. Therefore, this is the most trivial way to hook
//# yourself into the jafg serialization ecosystem with string <=> enum conversions.
//#
//# @see Below for bin <=> enum conversions.
//#
template<typename T> requires std::is_enum_v<T>
NODISCARD std::unordered_map<T, LStringView> const& enum_map() noexcept
{
    std::unordered_map<T, LStringView> const* Map;
    get_enum_map(&Map);
    return *Map;
}

//#
//# Whether this enum is marked as stable.
//# Changes to values of the enum or changing its size will break backwards compatibility.
//# Appending new values is ok. Modifying existing values is not. Changed the std::underlying_t is not ok.
//#
//# TODO: We need some kind of tool that can handle ABI changes. Maybe with ImHex?
//#
template<typename T> struct is_stable_enum: std::false_type {};
template<typename T> inline constexpr bool is_stable_enum_v{is_stable_enum<T>::value};
template<typename T> concept stable_enum = std::is_enum_v<T> && is_stable_enum_v<T>;

template<typename T> inline static constexpr bool is_primitive_v{std::is_arithmetic_v<T>||std::is_enum_v<T>};

//# The architecture type of archive.
enum struct arch_type: u8{ string, binary, /* net */};

//# The behavior of an archive when encountering an invalid or corrupted state.
enum struct behavior
{
    //# Panics jafg. Use for trusted streams only.
    panic,
    //# Logs if logging is enabled for the logging verbosity of the category.
    log,
    //# Ignore and parse what can be parsed.
    relaxed,
};

//#
//# Whether this type can be serialized or deserialized (or both) with the given archive.
//# If the archive is os only, then it only checks for serialization, if the archive is is only, then it only check
//# for deserialization. If the archive is ios, then both serialization and deserialization must be valid.
//#
template<typename TArchive, typename T> inline constexpr bool serde_for_v{ requires(TArchive& Ar, T& Field){serde_non_intrusive(Ar, Field);} };

template<typename TArchive> inline constexpr bool string_archive_v{ std::remove_cvref_t<TArchive>::arch_type == arch_type::string };
template<typename TArchive> inline constexpr bool bin_archive_v{ std::remove_cvref_t<TArchive>::arch_type == arch_type::binary };
template<typename TArchive> inline constexpr bool os_archive_v{ static_cast<bool>(std::remove_cvref_t<TArchive>::open_mode & std::ios::out) };
template<typename TArchive> inline constexpr bool is_archive_v{ static_cast<bool>(std::remove_cvref_t<TArchive>::open_mode & std::ios::in) };
template<typename TArchive> inline constexpr bool os_string_archive_v{ string_archive_v<TArchive> && os_archive_v<TArchive> };
template<typename TArchive> inline constexpr bool is_string_archive_v{ string_archive_v<TArchive> && is_archive_v<TArchive> };
template<typename TArchive> inline constexpr bool os_bin_archive_v{ bin_archive_v<TArchive> && os_archive_v<TArchive> };
template<typename TArchive> inline constexpr bool is_bin_archive_v{ bin_archive_v<TArchive> && is_archive_v<TArchive> };
template<typename TArchive> inline constexpr bool ios_bin_archive_v{ bin_archive_v<TArchive> && is_archive_v<TArchive> && os_archive_v<TArchive> };

template<typename TArchive> inline constexpr bool panic_archive_v{
    !requires(TArchive){ std::remove_cvref_t<TArchive>::behavior_mode; } || std::remove_cvref_t<TArchive>::behavior_mode == behavior::panic };
template<typename TArchive> inline constexpr bool log_archive_v{
    requires(TArchive){ std::remove_cvref_t<TArchive>::behavior_mode; } && std::remove_cvref_t<TArchive>::behavior_mode == behavior::log };
template<typename TArchive> inline constexpr bool relaxed_archive_v{
    requires(TArchive){ std::remove_cvref_t<TArchive>::behavior_mode; } && std::remove_cvref_t<TArchive>::behavior_mode == behavior::relaxed };

//#
//# Whether for the given T the archive is valid.
//# If the archive is an ios then T must not be const. But for is archives this invariant does not hold.
//#
template<typename TArchive, typename T>
inline constexpr bool bin_archive_for_v{
       (ios_bin_archive_v<TArchive> && !std::is_const_v<T>)
    || (!ios_bin_archive_v<TArchive> && is_bin_archive_v<TArchive> && !std::is_const_v<T>)
    || (!ios_bin_archive_v<TArchive> && os_bin_archive_v<TArchive> && (std::is_const_v<T> || !std::is_const_v<T>))
    };
template<typename TArchive, typename T>
inline constexpr bool string_archive_for_v{
       (is_string_archive_v<TArchive> && !std::is_const_v<T>)
    || (os_string_archive_v<TArchive> && (std::is_const_v<T> || !std::is_const_v<T>))
    };

//# Requirement for a specific implementation struct U.
template<typename TArchive, typename T, typename U>
inline constexpr bool string_archive_impl_for_v{ string_archive_for_v<TArchive, T> && std::same_as<U, std::remove_const_t<T>> };
template<typename TArchive, typename T, typename U>
inline constexpr bool is_string_archive_impl_for_v{ is_string_archive_v<TArchive> && std::same_as<U, std::remove_const_t<T>> };
template<typename TArchive, typename T, typename U>
inline constexpr bool os_string_archive_impl_for_v{ os_string_archive_v<TArchive> && std::same_as<U, std::remove_const_t<T>> };

//# Whether T is considered a string. You may add your own.
template<typename T> struct is_string: std::false_type {};
template<typename T> inline constexpr bool string_v{is_string<T>::value};
template<> struct is_string<LString>: std::true_type {};
template<> struct is_string<Lu8String>: std::true_type {};
template<> struct is_string<Lu16String>: std::true_type {};
template<> struct is_string<Lu32String>: std::true_type {};
template<> struct is_string<LStringView>: std::true_type {};
template<> struct is_string<Lu8StringView>: std::true_type {};
template<> struct is_string<Lu16StringView>: std::true_type {};
template<> struct is_string<Lu32StringView>: std::true_type {};
template<> struct is_string<char const*>: std::true_type {};
template<> struct is_string<unsigned char const*>: std::true_type {};
template<> struct is_string<signed char const*>: std::true_type {};
template<> struct is_string<char8_t const*>: std::true_type {};
template<> struct is_string<char16_t const*>: std::true_type {};
template<> struct is_string<char32_t const*>: std::true_type {};

//#
//# Serializes a type to/from its binary representation.
//# Usage example:
//#    struct LExample{ LString Name; i32 Value; };
//#    SERDE_BIN_NON_INTRUSIVE(LExample, Name, Value)
//#
#define DETAIL_SERDE_BIN_NON_INTRUSIVE_TRANSFORM(X) (Field.X)
#define SERDE_BIN_NON_INTRUSIVE(T, ...) \
    template<typename TArchive, typename U> requires(::serde::bin_archive_for_v<TArchive, U> && std::is_same_v<std::remove_const_t<U>, T>) \
    inline decltype(auto) serde_non_intrusive(TArchive&& Ar, U& Field) noexcept \
    { \
        Ar JAFG_MAP(DETAIL_SERDE_BIN_NON_INTRUSIVE_TRANSFORM, __VA_ARGS__) ; \
        return std::forward<decltype(Ar)>(Ar); \
    }

//#
//# Serializes a type to/from a string.
//# Usage example:
//#    struct LExample{ LString Name; i32 Value; };
//#    SERDE_STRING_NON_INTRUSIVE(LExample, Name, Value)
//#
#define DETAIL_SERDE_STRING_NON_INTRUSIVE_TRANSFORM(X) (Field.X)
#define SERDE_STRING_NON_INTRUSIVE(T, ...) \
    template<typename TArchive, typename U> requires(::serde::string_archive_for_v<TArchive, U> && std::is_same_v<std::remove_const_t<U>, T>) \
    inline decltype(auto) serde_non_intrusive(TArchive&& Ar, U& Field) noexcept \
    { \
        Ar JAFG_MAP(DETAIL_SERDE_STRING_NON_INTRUSIVE_TRANSFORM, __VA_ARGS__) ; \
        return std::forward<decltype(Ar)>(Ar); \
    }

enum struct state
{
    //# Everything is ok.
    good = 0x0,
    //# Unrecoverable corruption.
    bad = 0x1 << 0,
    //# End of file reached.
    eof = 0x1 << 1,
    //# Failed logically.
    fail = 0x1 << 2,
};
SERDE_ENUM(state, good, bad, eof, fail)
ENUM_STRUCT_FLAGS(state, state_flags) // TODO: Make an abstraction to automatically generate flags to / from string.

template<typename TArchive>
concept fail_archive = requires(TArchive& Ar)
{
    { Ar.message } -> std::convertible_to<LString>;
    { Ar.exception } -> std::convertible_to<state_flags>;
};
template<typename TArchive>
inline constexpr bool fail_archive_v{fail_archive<TArchive>};

namespace detail
{

template<typename TStream>
struct bin_archive_base
{
    typedef u64 range_size_type;
    typedef bool optional_marker_type;
    static_assert(sizeof(bool) == 1);
    typedef TStream stream_type;
    typedef typename stream_type::char_type char_type;
    typedef typename stream_type::char_type value_type;

    inline static constexpr auto arch_type{arch_type::binary};
    inline static constexpr auto behavior_mode{behavior::panic};

    NODISCARD FORCEINLINE bool operator!(this auto&& Self) noexcept { return !Self.get_stream(); }
};

//# An archive that can fail.
struct fail_archive_base
{
    LString message;
    state_flags exception{state::good};
};

template<typename TStream>
struct os_archive_base: bin_archive_base<TStream>
{
    typedef bin_archive_base<TStream> base_type;
    using typename base_type::range_size_type;
    using typename base_type::optional_marker_type;
    using typename base_type::stream_type;
    using typename base_type::char_type;
    using typename base_type::value_type;

    template<typename T> requires(!std::is_pointer_v<T>)
    decltype(auto) write(this auto&& Self, T const& x) noexcept requires bin_archive_for_v<decltype(Self), T>
    {
        check(Self.out_stream())
        serde_non_intrusive(Self, x);
        return std::forward<decltype(Self)>(Self);
    }

    template<typename T>
        requires(std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T> && (!std::is_enum_v<T> || is_stable_enum_v<T>))
    decltype(auto) write_pod(this auto&& Self, T const& x) noexcept
    {
        check(Self.out_stream())
        if (!Self.m_stream.write(reinterpret_cast<char_type const*>(&x), sizeof(T)))
        {
            LOG_FATAL(LogSerialization, "[{}]: Failed to write primitive type.", algo::type_name<T>())
        }
        return std::forward<decltype(Self)>(Self);
    }

    template<algo::contiguous_range T>
        requires std::is_trivially_copyable_v<typename T::value_type>
    void write_input_range(this auto&& Self, T const& xs) noexcept
    {
        check(Self.out_stream())
        Self.write_pod(static_cast<range_size_type>(algo::size(xs)));
        Self.write_bulk(algo::data(xs), algo::size(xs));
    }
    template<algo::input_range T>
        requires(!(algo::contiguous_range<T> && std::is_trivially_copyable_v<typename T::value_type>))
    decltype(auto) write_input_range(this auto&& Self, T const& xs) noexcept
        requires requires(decltype(Self)& ar, typename T::value_type const& x){ar.write(x);}
    {
        check(Self.out_stream())
        Self.write_pod(static_cast<range_size_type>(algo::size(xs)));
        for (auto const& x: xs)
        {
            Self.operator()(x);
        }
        return std::forward<decltype(Self)>(Self);
    }

    template<typename T>
        requires std::is_trivially_copyable_v<T>
    decltype(auto) write_bulk(this auto&& Self, T const* xs, range_size_type n) noexcept
    {
        check(Self.out_stream())
        if (!Self.m_stream.write(reinterpret_cast<std::iostream::char_type const*>(xs), sizeof(T) * n))
        {
            LOG_FATAL(LogSerialization, "[{}]: Failed to write data of size [{}].", algo::type_name<T>(), sizeof(T) * n)
        }
        return std::forward<decltype(Self)>(Self);
    }
};

template<typename TStream>
struct is_archive_base: bin_archive_base<TStream>
{
    typedef bin_archive_base<TStream> base_type;
    using typename base_type::range_size_type;
    using typename base_type::optional_marker_type;
    using typename base_type::stream_type;
    using typename base_type::char_type;
    using typename base_type::value_type;

    template<typename T> requires(!std::is_pointer_v<T> && !std::is_const_v<T>)
    decltype(auto) read(this auto&& Self, T& x) noexcept requires bin_archive_for_v<decltype(Self), T>
    {
        check(Self.in_stream())
        serde_non_intrusive(Self, x);
        return std::forward<decltype(Self)>(Self);
    }

    template<typename T> requires(!std::is_const_v<T> &&
        std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T> && (!std::is_enum_v<T> || is_stable_enum_v<T>))
    decltype(auto) read_pod(this auto&& Self, T& x) noexcept
    {
        check(Self.in_stream())
        if (!Self.m_stream.read(reinterpret_cast<std::iostream::char_type*>(&x), sizeof(T)))
        {
            LOG_FATAL(LogSerialization, "[{}]: Failed to read primitive type.", algo::type_name<T>())
        }
        if (Self.m_stream.gcount() != sizeof(T))
        {
            LOG_FATAL(LogSerialization, "[{}]: Failed to read enough bytes for primitive type. Expected [{}] but got [{}]."
                , algo::type_name<T>(), sizeof(T), Self.m_stream.gcount())
        }
        return std::forward<decltype(Self)>(Self);
    }

    template<algo::contiguous_range T>
        requires(std::is_trivially_copyable_v<typename T::value_type> && !std::is_const_v<T> && !std::is_const_v<typename T::value_type>)
    decltype(auto) read_input_range(this auto&& Self, T& xs) noexcept
    {
        check(Self.in_stream())
        range_size_type n;
        Self.read_pod(n);
        xs.resize(n);
        Self.read_bulk(algo::data(xs), n);
        return std::forward<decltype(Self)>(Self);
    }
    template<algo::input_range T>
        requires(!(algo::contiguous_range<T> && std::is_trivially_copyable_v<typename T::value_type>)
            && !std::is_const_v<T> && std::is_default_constructible_v<T>)
    decltype(auto) read_input_range(this auto&& Self, T& xs) noexcept
        requires requires(decltype(Self)& ar, typename T::value_type& x){ar.read(x);}
    {
        check(Self.in_stream())
        range_size_type n;
        Self.read_pod(n);
        xs.clear();
        for (range_size_type i{0uz}; i < n; ++i)
        {
            typename T::value_type x;
            Self.operator()(x);
            xs.insert(xs.end(), std::move(x));
        }
        return std::forward<decltype(Self)>(Self);
    }

    template<typename T> requires(!std::is_const_v<T>)
    decltype(auto) read_bulk(this auto&& Self, T* xs, std::size_t n) noexcept
    {
        static_assert(std::is_trivially_copyable_v<T>);
        check(Self.in_stream())
        if (!Self.m_stream.read(reinterpret_cast<std::iostream::char_type*>(xs), sizeof(T) * n))
        {
            LOG_FATAL(LogSerialization, "[{}]: Failed to read data of size [{}].", algo::type_name<T>(), sizeof(T) * n)
        }
        if (static_cast<std::size_t>(Self.m_stream.gcount()) != sizeof(T) * n)
        {
            LOG_FATAL(LogSerialization, "[{}]: Failed to read enough bytes for data. Expected [{}] but got [{}]."
                , algo::type_name<T>(), sizeof(T) * n, Self.m_stream.gcount())
        }
        return std::forward<decltype(Self)>(Self);
    }
};

} /* ~Namespace detail */

//#
//# The default binary archive used for output streams.
//# Use it with any serializable type by calling operator().
//#
struct os_bin_archive final: detail::os_archive_base<std::ostream>
{
    static constexpr auto open_mode{std::ios::out};
    typedef std::ostream stream_type;

    template<typename... TArgs> requires std::constructible_from<std::ostream, TArgs&&...>
    constexpr explicit os_bin_archive(TArgs&&... Args) noexcept: m_stream{std::forward<TArgs>(Args)...} {}

    //# Serde any type that can be serialized.
    template<typename T> requires(!std::is_pointer_v<T>)
    inline decltype(auto) operator()(this auto&& Self, T const& x) noexcept requires bin_archive_for_v<decltype(Self), T>
    {
        serde_non_intrusive(Self, x);
        return std::forward<decltype(Self)>(Self);
    }

    //# This archive is open for input. Therefore, you read from it.
    NODISCARD FORCEINLINE consteval bool in_stream() const noexcept { return false; }
    //# This archive is open for output. Therefore, you write to it.
    NODISCARD FORCEINLINE consteval bool out_stream() const noexcept { return true; }

    NODISCARD FORCEINLINE std::ostream const& get_stream() const noexcept { return this->m_stream; }

    std::ostream m_stream;
};

//#
//# The default binary archive used for input streams.
//# Use it with any serializable type by calling operator().
//#
struct is_bin_archive final: detail::is_archive_base<std::istream>
{
    static constexpr auto open_mode{std::ios::in};

    template<typename... TArgs> requires std::constructible_from<std::istream, TArgs&&...>
    constexpr explicit is_bin_archive(TArgs&&... Args) noexcept: m_stream{std::forward<TArgs>(Args)...} {}

    //# Serde any type that can be deserialized.
    template<typename T> requires(!std::is_const_v<T> && !std::is_pointer_v<T>)
    inline decltype(auto) operator()(this auto&& Self, T& x) noexcept requires bin_archive_for_v<decltype(Self), T>
    {
        serde_non_intrusive(Self, x);
        return std::forward<decltype(Self)>(Self);
    }

    //# This archive is open for input. Therefore, you read from it.
    NODISCARD FORCEINLINE consteval bool in_stream() const noexcept { return true; }
    //# This archive is open for output. Therefore, you write to it.
    NODISCARD FORCEINLINE consteval bool out_stream() const noexcept { return false; }

    NODISCARD FORCEINLINE std::istream const& get_stream() const noexcept { return this->m_stream; }

    std::istream m_stream;
};

//#
//# The default binary archive used for either output or input streams.
//# Use it with any serializable type by calling operator().
//#
struct ios_bin_archive: detail::is_archive_base<std::iostream>, detail::os_archive_base<std::iostream>
{
    static constexpr auto open_mode{std::ios::in|std::ios::out};

    template<typename... TArgs> requires std::constructible_from<std::iostream, TArgs&&...>
    constexpr explicit ios_bin_archive(std::ios_base::openmode mode, TArgs&&... Args) noexcept
        : m_stream{std::forward<TArgs>(Args)...}, m_open_mode{mode}
    {
        check(this->in_stream() != this->out_stream())
    }

    //# Serde any type that can be serialized and deserialized.
    template<typename T> requires(!std::is_const_v<T> && !std::is_pointer_v<T>)
    inline decltype(auto) operator()(this auto&& Self, T& t) noexcept requires bin_archive_for_v<decltype(Self), T>
    {
        serde_non_intrusive(Self, t);
        return std::forward<decltype(Self)>(Self);
    }

    //# This archive is open for input. Therefore, you read from it.
    NODISCARD FORCEINLINE constexpr bool in_stream() const noexcept { return this->m_open_mode & std::ios::in; }
    //# This archive is open for output. Therefore, you write to it.
    NODISCARD FORCEINLINE constexpr bool out_stream() const noexcept { return this->m_open_mode & std::ios::out; }

    NODISCARD FORCEINLINE bool operator!(this auto&& Self) noexcept { return !Self.get_stream(); }
    NODISCARD FORCEINLINE std::iostream const& get_stream() const noexcept { return this->m_stream; }

    std::iostream m_stream;

private:

    const std::ios_base::openmode m_open_mode;
};

//#
//# The default string archive used for output streams.
//# Use it with any serializable type by calling operator().
//#
struct os_string_archive final
{
    inline static constexpr auto open_mode{std::ios::out};
    inline static constexpr auto arch_type{arch_type::string};
    inline static constexpr auto behavior_mode{behavior::panic};

    template<typename... TArgs> requires std::constructible_from<std::stringstream, TArgs&&...>
    constexpr explicit os_string_archive(TArgs&&... Args) noexcept: m_stream{std::forward<TArgs>(Args)...} {}

    //# Serde any type that can be serialized.
    template<typename T> requires(!std::is_pointer_v<T>)
    inline decltype(auto) operator()(this auto&& Self, T& x) noexcept requires string_archive_for_v<decltype(Self), T>
    {
        (void)serde_non_intrusive(Self, x);
        return std::forward<decltype(Self)>(Self);
    }

    //# This archive is open for input. Therefore, you read from it.
    NODISCARD FORCEINLINE consteval bool in_stream() const noexcept { return false; }
    //# This archive is open for output. Therefore, you write to it.
    NODISCARD FORCEINLINE consteval bool out_stream() const noexcept { return true; }

    NODISCARD FORCEINLINE std::stringstream const& get_stream() const noexcept { return this->m_stream; }

    std::stringstream m_stream;
};

//#
//# The default string archive used for input streams.
//# Use it with any serializable type by calling operator().
//#
template<behavior Behavior>
struct is_string_archive final: detail::fail_archive_base
{
    inline static constexpr auto open_mode{std::ios::in};
    inline static constexpr auto arch_type{arch_type::string};
    inline static constexpr auto behavior_mode{Behavior};

    template<typename... TArgs> requires std::constructible_from<LStringView, TArgs&&...>
    constexpr explicit is_string_archive(TArgs&&... Args) noexcept: m_stream{std::forward<TArgs>(Args)...} {}

    //# Serde any type that can be deserialized.
    template<typename T> requires(!std::is_const_v<T> && !std::is_pointer_v<T>)
    inline decltype(auto) operator()(this auto&& Self, T& x) noexcept requires string_archive_for_v<decltype(Self), T>
    {
        (void)serde_non_intrusive(Self, x);

        if constexpr (Behavior == behavior::panic)
        {
            if (Self.exception != state::good)
            {
                LOG_FATAL(LogSerialization, "Failed to deserialize type [{}] from string [{}]. Error: {}"
                    , algo::type_name<T>(), Self.get_stream(), Self.message)
            }
            else
            {
                check(Self.message.empty())
            }
        }
        else if constexpr (Behavior == behavior::log)
        {
            if (Self.exception != state::good)
            {
                LOG_ERROR(LogSerialization, "Failed to deserialize type [{}] from string [{}]. Error: {}"
                    , algo::type_name<T>(), Self.get_stream(), Self.message)
            }
            else
            {
                check(Self.message.empty())
            }
        }

        return std::forward<decltype(Self)>(Self);
    }

    //# This archive is open for input. Therefore, you read from it.
    NODISCARD FORCEINLINE consteval bool in_stream() const noexcept { return true; }
    //# This archive is open for output. Therefore, you write to it.
    NODISCARD FORCEINLINE consteval bool out_stream() const noexcept { return false; }

    NODISCARD FORCEINLINE LStringView const& get_stream() const noexcept { return this->m_stream; }

    NODISCARD FORCEINLINE constexpr explicit operator bool() noexcept { return this->exception == state::good; }
    NODISCARD FORCEINLINE constexpr bool operator!() noexcept { return this->exception != state::good; }

    LStringView m_stream;
};

//# Convert a type to its string representation.
template<typename T> requires string_archive_for_v<os_string_archive, T>// && serde_for_v<os_string_archive, T const>
NODISCARD FORCEINLINE LString to_string(T const& Field) noexcept
{
    static_assert(serde_for_v<os_string_archive, T const>);
    os_string_archive Ar;
    Ar(Field);
    return Ar.get_stream().str();
}

//# Quick conversion of the formation from T to T. If an error occurs the program will panic.
template<typename T> requires string_archive_for_v<is_string_archive<behavior::panic>, T>
FORCEINLINE void from_string(T* Field, LStringView Value) noexcept
{
    is_string_archive<behavior::panic> Ar{Value};
    Ar(*Field);
}
template<typename T> requires string_archive_for_v<is_string_archive<behavior::panic>, T>
    && std::is_default_constructible_v<T> && std::is_move_constructible_v<T>
NODISCARD FORCEINLINE T from_string(LStringView Value) noexcept
{
    T Field{};
    is_string_archive<behavior::panic> Ar{Value};
    Ar(Field);
    return Field;
}

//#
//# Quick conversion of the formation from T to T. If an error occurs it will return false and log a message
//# if logging for #LogSerialization at error verbosity is enabled.
//#
template<typename T> requires string_archive_for_v<is_string_archive<behavior::log>, T>
FORCEINLINE void from_string_logged(T* Field, LStringView Value) noexcept
{
    is_string_archive<behavior::log> Ar{Value};
    Ar(*Field);
}
template<typename T> requires string_archive_for_v<is_string_archive<behavior::log>, T>
    && std::is_default_constructible_v<T> && std::is_move_constructible_v<T>
NODISCARD FORCEINLINE T from_string_logged(LStringView Value) noexcept
{
    T Field{};
    is_string_archive<behavior::log> Ar{Value};
    Ar(Field);
    return Field;
}

//# Quick conversion of the formation from T to T. If an error occurs it will return false.
template<typename T> requires string_archive_for_v<is_string_archive<behavior::relaxed>, T>
FORCEINLINE void from_string_relaxed(T* Field, LStringView Value) noexcept
{
    is_string_archive<behavior::relaxed> Ar{Value};
    Ar(*Field);
}
template<typename T> requires string_archive_for_v<is_string_archive<behavior::relaxed>, T>
    && std::is_default_constructible_v<T> && std::is_move_constructible_v<T>
NODISCARD FORCEINLINE T from_string_relaxed(LStringView Value) noexcept
{
    T Field{};
    is_string_archive<behavior::relaxed> Ar{Value};
    Ar(Field);
    return Field;
}


//////////////////////////////////////////////////////////////////////////
// Core implementation.
//////////////////////////////////////////////////////////////////////////

template<typename TArchive, typename U> requires serde::bin_archive_for_v<TArchive, U> && serde::is_primitive_v<U>
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, U& Field) noexcept
{
    if constexpr (serde::ios_bin_archive_v<TArchive>)
    {
        if (Ar.in_stream())
        {
            Ar.read_pod(Field);
        }
        else
        {
            Ar.write_pod(Field);
        }
    }
    else if constexpr (serde::is_bin_archive_v<TArchive>)
    {
        Ar.read_pod(Field);
    }
    else if constexpr (serde::os_bin_archive_v<TArchive>)
    {
        Ar.write_pod(Field);
    }
    else
    {
        static_assert(algo::always_false_v<TArchive, U>);
    }
    return std::forward<decltype(Ar)>(Ar);
}

template<typename TArchive, algo::input_range U> requires serde::bin_archive_for_v<TArchive, U>
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, U& Field) noexcept
{
    if constexpr (serde::ios_bin_archive_v<TArchive>)
    {
        if (Ar.in_stream())
        {
            Ar.read_input_range(Field);
        }
        else
        {
            Ar.write_input_range(Field);
        }
    }
    else if constexpr (serde::is_bin_archive_v<TArchive>)
    {
        Ar.read_input_range(Field);
    }
    else if constexpr (serde::os_bin_archive_v<TArchive>)
    {
        Ar.write_input_range(Field);
    }
    else
    {
        static_assert(algo::always_false_v<TArchive, U>);
    }
    return std::forward<decltype(Ar)>(Ar);
}

template<typename TArchive, typename U> requires serde::string_archive_impl_for_v<TArchive, U, bool>
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, U& Field) noexcept
{
    if constexpr (serde::is_string_archive_v<TArchive>)
    {
        if (Ar.get_stream() == "1")
        {
            Field = true;
        }
        else if (Ar.get_stream() == "0")
        {
            Field = false;
        }
        else
        {
            if constexpr (serde::panic_archive_v<TArchive>)
            {
                LOG_FATAL(LogSerialization, "[{}]: Failed to deserialize boolean from [{}]."
                    , algo::type_name<U>(), Ar.get_stream())
            }
            else if constexpr (serde::log_archive_v<TArchive>)
            {
                LOG_ERROR(LogSerialization, "[{}]: Failed to deserialize boolean from [{}]."
                    , algo::type_name<U>(), Ar.get_stream())
            }
            else if constexpr (serde::relaxed_archive_v<TArchive>)
            {
            }
            else
            {
                static_assert(algo::always_false_v<TArchive, U>);
            }
        }
    }
    else if constexpr (serde::os_string_archive_v<TArchive>)
    {
        if (Field)
        {
            Ar.m_stream << "1";
        }
        else
        {
            Ar.m_stream << "0";
        }
    }
    else
    {
        static_assert(algo::always_false_v<TArchive, U>);
    }
    return std::forward<decltype(Ar)>(Ar);
}

template<typename TArchive, typename U> requires(serde::string_archive_for_v<TArchive, U>
    && (std::is_integral_v<U> || std::is_floating_point_v<U>)
    && !std::same_as<bool, std::remove_const_t<U>>)
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, U& Field) noexcept
{
    if constexpr (serde::is_string_archive_v<TArchive>)
    {
        auto R{std::from_chars(algo::data(Ar.get_stream()), algo::data(Ar.get_stream()) + algo::size(Ar.get_stream()), Field)};
        if constexpr (serde::panic_archive_v<TArchive>)
        {
            if (R.ec != decltype(R.ec){})
            {
                LOG_FATAL(LogSerialization, "[{}]: Failed to deserialize from [{}] with error [{}]."
                    , algo::type_name<U>(), Ar.get_stream(), std::to_underlying(R.ec))
            }
        }
        else if constexpr (serde::log_archive_v<TArchive>)
        {
            if (R.ec != decltype(R.ec){})
            {
                LOG_ERROR(LogSerialization, "[{}]: Failed to deserialize from [{}] with error [{}]."
                    , algo::type_name<U>(), Ar.get_stream(), std::to_underlying(R.ec))
            }
        }
        else if constexpr (serde::relaxed_archive_v<TArchive>)
        {
        }
        else
        {
            static_assert(algo::always_false_v<TArchive, U>);
        }
    }
    else if constexpr (serde::os_string_archive_v<TArchive>)
    {
        Ar.m_stream << Field;
    }
    else
    {
        static_assert(algo::always_false_v<TArchive, U>);
    }
    return std::forward<decltype(Ar)>(Ar);
}

template<typename TArchive, typename U>
    requires(serde::string_archive_for_v<TArchive, U> && std::is_enum_v<std::remove_cvref_t<U>> && !std::same_as<std::byte, std::remove_cvref_t<U>>)
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, U& Field) noexcept
{
    if constexpr (serde::is_string_archive_v<TArchive>)
    {
        auto& Map{serde::enum_map<std::remove_cvref_t<U>>()};
        if (auto It{algo::find_if(Map, [&Ar](auto const& Member){ return Member.second == Ar.get_stream(); })}; It != std::end(Map))
        {
            Field = It->first;
        }
        else
        {
            if constexpr (serde::panic_archive_v<TArchive>)
            {
                LOG_FATAL(LogSerialization, "[{}]: Failed to deserialize enum from representation [{}]."
                    , algo::type_name<U>(), Ar.get_stream())
            }
            else if constexpr (serde::log_archive_v<TArchive>)
            {
                LOG_ERROR(LogSerialization, "[{}]: Failed to deserialize enum from representation [{}]."
                    , algo::type_name<U>(), Ar.get_stream())
            }
            else if constexpr (serde::relaxed_archive_v<TArchive>)
            {
            }
            else
            {
                static_assert(algo::always_false_v<TArchive, U>);
            }
        }
    }
    else if constexpr (serde::os_string_archive_v<TArchive>)
    {
        auto& Map{serde::enum_map<std::remove_cvref_t<U>>()};
        auto It{Map.find(Field)};
        check(It != Map.end())
        Ar.m_stream << It->second;
    }
    else
    {
        static_assert(algo::always_false_v<TArchive, U>);
    }
    return std::forward<decltype(Ar)>(Ar);
}
template<typename TArchive, typename U> requires serde::string_archive_for_v<TArchive, U> && std::same_as<std::byte, std::remove_cvref_t<U>>
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, U& Field) noexcept
{
    auto Underlying{static_cast<std::size_t>(std::to_underlying(Field))};
    auto&& Result{serde_non_intrusive(Ar, Underlying)};
    if constexpr (serde::is_string_archive_v<TArchive>)
    {
        static_assert(fail_archive_v<TArchive>);
        if (Underlying > 255)
        {
            Ar.exception = state::fail;
            Ar.message = algo::sprintf("Expected a value between 0 and 255 but got [{}] from: {}", Underlying, Ar.get_stream());
        }
        else
        {
            Field = static_cast<std::byte>(Underlying);
        }
    }
    else if constexpr (serde::os_string_archive_v<TArchive>)
    {
    }
    else
    {
        static_assert(algo::always_false_v<TArchive, U>);
    }
    return std::forward<decltype(Result)>(Result);
}

template<typename TArchive, typename U> requires serde::string_archive_impl_for_v<TArchive, U, LString>
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, U& Field) noexcept
{
    if constexpr (serde::is_string_archive_v<TArchive>)
    {
        Field = Ar.get_stream();
    }
    else if constexpr (serde::os_string_archive_v<TArchive>)
    {
        Ar.m_stream << Field;
    }
    else
    {
        static_assert(algo::always_false_v<TArchive, U>);
    }
    return std::forward<decltype(Ar)>(Ar);
}

template<typename TArchive, typename U> requires serde::os_string_archive_impl_for_v<TArchive, U, LStringView>
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, U& Field) noexcept
{
    Ar.m_stream << Field;
    return std::forward<decltype(Ar)>(Ar);
}

template<typename TArchive, typename U> requires serde::string_archive_impl_for_v<TArchive, U, LColor>
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, U& Field) noexcept
{
    if constexpr (serde::is_string_archive_v<TArchive>)
    {
        static_assert(fail_archive_v<TArchive>);

        auto& Value{Ar.m_stream};

        if (!Value.starts_with("0x"))
        {
            Ar.exception = state::fail;
            Ar.message = algo::sprintf("Expected '0x' at the start of: {}", Value);
            return std::forward<decltype(Ar)>(Ar);
        }

        if (Value.size() != 10)
        {
            Ar.exception = state::fail;
            Ar.message = algo::sprintf("Expected a size of '10' but got '{}' from: {}", Value.size(), Value);
            return std::forward<decltype(Ar)>(Ar);
        }

        for (auto Idx{2uz}; Idx < Value.size(); ++Idx)
        {
            if ((Value[Idx] >= '0' && Value[Idx] <= '9') || (Value[Idx] >= 'A' && Value[Idx] <= 'F'))
            {
                continue;
            }

            Ar.exception = state::fail;
            Ar.message = algo::sprintf("Expected one of the values of [0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F] but got '{}' at index '{}' from: {}",
                Value[Idx], Idx, Value
                );
            return std::forward<decltype(Ar)>(Ar);
        }

        Field = Colors::Transparent;

        auto GetValue{[](const LStringView& LambdaValue, const i32 LambdaIndex) -> u8
        {
            check((LambdaValue[LambdaIndex] >= '0' && LambdaValue[LambdaIndex] <= '9')
               || (LambdaValue[LambdaIndex] >= 'A' && LambdaValue[LambdaIndex] <= 'F'))

            return
                (LambdaValue[LambdaIndex] >= '0' && LambdaValue[LambdaIndex] <= '9')
                    ? (LambdaValue[LambdaIndex] - '0')
                    : (LambdaValue[LambdaIndex] >= 'A' && LambdaValue[LambdaIndex] <= 'F')
                        ? (LambdaValue[LambdaIndex] - 'A' + 10)
                        : 0;
                }
        };

        Field.r = GetValue(Value, 2);
        Field.r <<= 4;
        Field.r |= GetValue(Value, 3);

        Field.g = GetValue(Value, 4);
        Field.g <<= 4;
        Field.g |= GetValue(Value, 5);

        Field.b = GetValue(Value, 6);
        Field.b <<= 4;
        Field.b |= GetValue(Value, 7);

        Field.a = GetValue(Value, 8);
        Field.a <<= 4;
        Field.a |= GetValue(Value, 9);
    }
    else if constexpr (serde::os_string_archive_v<TArchive>)
    {
        Ar.m_stream << algo::sprintf("0x{:02X}{:02X}{:02X}{:02X}", Field.r, Field.g, Field.b, Field.a);
    }
    else
    {
        static_assert(algo::always_false_v<TArchive, U>);
    }
    return std::forward<decltype(Ar)>(Ar);
}

template<typename TArchive, typename U> requires serde::string_archive_impl_for_v<TArchive, U, TArray<typename std::remove_cvref_t<U>::value_type>>
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, U& Field) noexcept
{
    typedef typename std::remove_cvref_t<U>::value_type value_type;

    if constexpr (serde::is_string_archive_v<TArchive>)
    {
        static_assert(fail_archive_v<TArchive>);

        auto& Value{Ar.get_stream()};
        TArray<value_type> Out;

        if (!Value.starts_with('['))
        {
            Ar.exception = state::fail;
            Ar.message = algo::sprintf("Expected '[' at the start of: \n{}", Value);
            return std::forward<decltype(Ar)>(Ar);
        }
        if (!Value.ends_with(']'))
        {
            Ar.exception = state::fail;
            Ar.message = algo::sprintf("Expected ']' at the end of: \n{}", Value);
            return std::forward<decltype(Ar)>(Ar);
        }

        LStringView View{Value.begin() + 1, Value.end() - 1};
        if (View.empty())
        {
            Field = std::move(Out);
            return std::forward<decltype(Ar)>(Ar);
        }

        auto ParseElement{[&](LStringView View, value_type& Field)
        {
            if (auto SubAr{std::remove_cvref_t<TArchive>{View}}; !SubAr(Field))
            {
                Ar.exception = state::fail;
                Ar.message = algo::sprintf("Failed to parse subargument [{}] from [{}] with [{}]."
                    , View, Value, SubAr.message.empty() ? "<unknown error>" : SubAr.message);
            }
            return std::forward<decltype(Ar)>(Ar);
        }};

        LString Element;
        bool bEscaped{};
        bool bTrailingComma{};
        for (LStringView::value_type Char: View)
        {
            if (bEscaped)
            {
                Element += Char;
                bEscaped = false;
                continue;
            }

            bTrailingComma = false;

            if (Char == '\\')
            {
                bEscaped = true;
                continue;
            }

            if (Char == ',')
            {
                bTrailingComma = true;

                Out.emplace_back();
                if (!ParseElement(Element, Out.back()))
                {
                    return std::forward<decltype(Ar)>(Ar);
                }

                Element.clear();
                continue;
            }

            Element += Char;
        }

        if (bTrailingComma)
        {
            check(Element.empty())
        }
        else
        {
            Out.emplace_back();
            if (!ParseElement(Element, Out.back()))
            {
                return std::forward<decltype(Ar)>(Ar);
            }
        }

        Field = std::move(Out);
    }
    else if constexpr (serde::os_string_archive_v<TArchive>)
    {
        auto& SS{Ar.m_stream};
        SS << '[';

        bool bFirst{true};
        for (value_type const& Elem: Field)
        {
            if (!bFirst)
            {
                SS << ',';
            }
            bFirst = false;
            for (LString ElemStr{std::remove_cvref_t<TArchive>{Elem}.get_stream().str()}; LString::value_type const& C: ElemStr)
            {
                if (C == '[' || C == ']' || C == ',' || C == '\\')
                {
                    SS << '\\';
                }
                SS << C;
            }
        }

        SS << ']';
    }
    else
    {
        static_assert(algo::always_false_v<TArchive, U>);
    }
    return std::forward<decltype(Ar)>(Ar);
}

template<typename TArchive> requires(serde::bin_archive_for_v<TArchive, LPath> && serde::ios_bin_archive_v<TArchive>)
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, LPath& Field) noexcept
{
    if (Ar.in_stream())
    {
        LString Dummy;
        Ar(Dummy);
        Field = std::move(Dummy);
    }
    else
    {
#if JAFG_PLATFORM_USES_UTF8
        LStringView Dummy{Field.native()};
#elif JAFG_PLATFORM_USES_UTF16
        LString Dummy{Field.string()};
#else /* JAFG_PLATFORM_USES_UTF16 */
    #error "Missing encoding implementation."
#endif /* !JAFG_PLATFORM_USES_UTF16 */
        Ar(Dummy);
    }
    return std::forward<decltype(Ar)>(Ar);
}
template<typename TArchive> requires(serde::bin_archive_for_v<TArchive, LPath> && serde::is_bin_archive_v<TArchive>)
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, LPath& Field) noexcept
{
    LString Dummy;
    Ar(Dummy);
    Field = std::move(Dummy);
    return std::forward<decltype(Ar)>(Ar);
}
template<typename TArchive> requires(serde::bin_archive_for_v<TArchive, LPath> && serde::os_bin_archive_v<TArchive>)
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, LPath const& Field) noexcept
{
#if JAFG_PLATFORM_USES_UTF8
    LStringView Dummy{Field.native()};
#elif JAFG_PLATFORM_USES_UTF16
    LString Dummy{Field.string()};
#else /* JAFG_PLATFORM_USES_UTF16 */
    #error "Missing encoding implementation."
#endif /* !JAFG_PLATFORM_USES_UTF16 */
    Ar(Dummy);
    return std::forward<decltype(Ar)>(Ar);
}

template<typename TArchive, typename U> requires string_archive_impl_for_v<TArchive, U, LPath>
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, U& Field) noexcept
{
    if constexpr (serde::is_string_archive_v<TArchive>)
    {
        Field = LPath{Ar.get_stream()};
    }
    else if constexpr (serde::os_string_archive_v<TArchive>)
    {
#if JAFG_PLATFORM_USES_UTF8
        Ar.m_stream << Field.native();
#else /* JAFG_PLATFORM_USES_UTF8 */
        Ar.m_stream << Field.string();
#endif /* !JAFG_PLATFORM_USES_UTF8 */
    }
    else
    {
        static_assert(algo::always_false_v<TArchive, U>);
    }
}

template<typename TArchive, typename U> requires(serde::bin_archive_for_v<TArchive, U> && serde::ios_bin_archive_v<TArchive>)
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, std::optional<U>& Field) noexcept
{
    static_assert(std::is_default_constructible_v<U>);

    if (Ar.in_stream())
    {
        typename TArchive::optional_marker_type Marker;
        Ar(Marker);
        if (Marker)
        {
            Field.emplace();
            Ar(*Field);
        }
    }
    else
    {
        typename TArchive::optional_marker_type Marker{Field.has_value()};
        Ar(Marker);
        if (Field.has_value())
        {
            Ar(*Field);
        }
    }
    return std::forward<decltype(Ar)>(Ar);
}
template<typename TArchive, typename U> requires(serde::bin_archive_for_v<TArchive, U> && serde::is_bin_archive_v<TArchive>)
inline decltype(auto) serde_non_intrusive(TArchive& Ar, std::optional<U>& Field) noexcept
{
    static_assert(std::is_default_constructible_v<U>);

    typename TArchive::optional_marker_type Marker;
    Ar(Marker);
    if (Marker)
    {
        Field.emplace();
        Ar(*Field);
    }
    return std::forward<decltype(Ar)>(Ar);
}
template<typename TArchive, typename U> requires(serde::bin_archive_for_v<TArchive, U> && serde::os_bin_archive_v<TArchive>)
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, std::optional<U> const& Field) noexcept
{
    typename std::remove_cvref_t<TArchive>::optional_marker_type Marker{Field.has_value()};
    Ar(Marker);
    if (Field.has_value())
    {
        Ar(*Field);
    }
    return std::forward<decltype(Ar)>(Ar);
}

template<typename TArchive, typename U>
    requires(serde::string_archive_for_v<TArchive, U>
    &&
    std::same_as<TVec<
          std::remove_const_t<U>::length()
        , typename std::remove_const_t<U>::value_type
        , maths::defaultp
        >, std::remove_const_t<U>>
    )
inline decltype(auto) serde_non_intrusive(TArchive&& Ar, U& Field) noexcept
{
    if constexpr (serde::is_string_archive_v<TArchive>)
    {
        auto& Value{Ar.m_stream};

        if (!Value.starts_with("vec"))
        {
            Ar.exception = state::fail;
            Ar.message = algo::sprintf("Expected 'vec' at the start of: \n{}", Value);
            return std::forward<decltype(Ar)>(Ar);
        }
        if (!Value.ends_with(")"))
        {
            Ar.exception = state::fail;
            Ar.message = algo::sprintf("Expected ')' at the end of: \n{}", Value);
            return std::forward<decltype(Ar)>(Ar);
        }

        LStringView View{Value.begin() + 3, Value.end() - 1};
        if (View.empty())
        {
            Ar.exception = state::fail;
            Ar.message = algo::sprintf("Expected vector length and components in: \n{}", Value);
            return std::forward<decltype(Ar)>(Ar);
        }

        maths::length_t l{};
        auto R{std::from_chars(algo::data(View), algo::data(View) + 1, l)};
        if (R.ec != std::errc{})
        {
            Ar.exception = state::fail;
            Ar.message = R.ptr ? LString{R.ptr} : LString{};
            return std::forward<decltype(Ar)>(Ar);
        }
        if (l != std::remove_const_t<U>::length())
        {
            Ar.exception = state::fail;
            Ar.message = algo::sprintf("Expected vector of length '{}' but got '{}' in: \n{}", std::remove_const_t<U>::length(), l, Value);
            return std::forward<decltype(Ar)>(Ar);
        }
        View = LStringView{View.begin()+1, View.end()};
        if (View.empty())
        {
            Ar.exception = state::fail;
            Ar.message = algo::sprintf("Expected vector components in: \n{}", Value);
            return std::forward<decltype(Ar)>(Ar);
        }
        if (View[0] != '(')
        {
            Ar.exception = state::fail;
            Ar.message = algo::sprintf("Expected '(' at the start of vector components in: \n{}", Value);
            return std::forward<decltype(Ar)>(Ar);
        }

        View = LStringView{View.begin() + 1, View.end()};

        if constexpr (std::remove_const_t<U>::length() == 1)
        {
            if (auto R{std::from_chars(algo::data(View), algo::data(View) + 1, Field.x)}; R.ec != std::errc{})
            {
                Ar.exception = state::fail;
                Ar.message = R.ptr ? LString{R.ptr} : LString{};
            }
            return std::forward<decltype(Ar)>(Ar);
        }

        // x comp
        if constexpr (std::remove_const_t<U>::length() >= 1)
        {
            auto Pos{View.find_first_of(',')};
            if (Pos == LStringView::npos)
            {
                Ar.exception = state::fail;
                Ar.message = algo::sprintf("Expected ',' after x component in: \n{}", Value);
                return std::forward<decltype(Ar)>(Ar);
            }

            LStringView ComponentX{View.begin(), Pos};
            auto R{std::from_chars(algo::data(ComponentX), algo::data(ComponentX) + algo::size(ComponentX), Field.x)};
            if (R.ec != std::errc{})
            {
                Ar.exception = state::fail;
                Ar.message = R.ptr ? LString{R.ptr} : LString{};
                return std::forward<decltype(Ar)>(Ar);
            }

            View = LStringView{View.begin() + Pos + 1, View.end()};
        }
        if constexpr (std::remove_const_t<U>::length() >= 2)
        {
            // y comp
            auto Pos{View.find_first_of(',')};
            if (Pos == LStringView::npos)
            {
                Ar.exception = state::fail;
                Ar.message = algo::sprintf("Expected ',' after y component in: \n{}", Value);
                return std::forward<decltype(Ar)>(Ar);
            }

            LStringView ComponentY{View.begin(), Pos};
            auto R{std::from_chars(algo::data(ComponentY), algo::data(ComponentY) + algo::size(ComponentY), Field.y)};
            if (R.ec != std::errc{})
            {
                Ar.exception = state::fail;
                Ar.message = R.ptr ? LString{R.ptr} : LString{};
                return std::forward<decltype(Ar)>(Ar);
            }

            View = LStringView{View.begin() + Pos + 1, View.end()};
        }
        if constexpr (std::remove_const_t<U>::length() >= 3)
        {
            // z comp
            auto Pos{View.find_first_of(',')};
            if (Pos == LStringView::npos)
            {
                Ar.exception = state::fail;
                Ar.message = algo::sprintf("Expected ',' after z component in: \n{}", Value);
                return std::forward<decltype(Ar)>(Ar);
            }

            LStringView ComponentZ{View.begin(), Pos};
            auto R{std::from_chars(algo::data(ComponentZ), algo::data(ComponentZ) + algo::size(ComponentZ), Field.z)};
            if (R.ec != std::errc{})
            {
                Ar.exception = state::fail;
                Ar.message = R.ptr ? LString{R.ptr} : LString{};
                return std::forward<decltype(Ar)>(Ar);
            }

            View = LStringView{View.begin() + Pos + 1, View.end()};
        }
        if constexpr (std::remove_const_t<U>::length() >= 4)
        {
            // w comp
            auto Pos{View.find_first_of(',')};
            if (Pos == LStringView::npos)
            {
                Ar.exception = state::fail;
                Ar.message = algo::sprintf("Expected ',' after w component in: \n{}", Value);
                return std::forward<decltype(Ar)>(Ar);
            }

            LStringView ComponentW{View.begin(), Pos};
            auto R{std::from_chars(algo::data(ComponentW), algo::data(ComponentW) + algo::size(ComponentW), Field.w)};
            if (R.ec != std::errc{})
            {
                Ar.exception = state::fail;
                Ar.message = R.ptr ? LString{R.ptr} : LString{};
                return std::forward<decltype(Ar)>(Ar);
            }

            View = LStringView{View.begin() + Pos + 1, View.end()};
        }
        if constexpr (std::remove_const_t<U>::length() >= 5)
        {
            static_assert(algo::always_false_v<TArchive, U>, "Unsupported vector length for deserialization.");
        }

        return std::forward<decltype(Ar)>(Ar);
    }
    else if constexpr (serde::os_string_archive_v<TArchive>)
    {
        Ar.m_stream << "vec" << std::remove_const_t<U>::length() << "(";
        for (maths::length_t Idx{0}; Idx < std::remove_const_t<U>::length(); ++Idx)
        {
            if (Idx > 0)
            {
                Ar.m_stream << ',';
            }
            Ar.m_stream << Field[Idx];
        }
        Ar.m_stream << ")";
    }
    else
    {
        static_assert(algo::always_false_v<TArchive, U>);
    }
}

} /* ~Namespace serde */

//# Convenience specialization.
template<typename T> requires requires(T t) { {t.to_string()} -> std::convertible_to<LString>; }
struct std::formatter<T>: std::formatter<std::string>
{
    FORCEINLINE std::format_context::iterator format(T const& Value, std::format_context& Context) const
    {
        return std::formatter<std::string>::format(Value.to_string(), Context);
    }
};

//# Convenience specialization.
template<typename T> requires requires(T t) { {t.ToString()} -> std::convertible_to<LString>; }
struct std::formatter<T>: std::formatter<std::string>
{
    FORCEINLINE std::format_context::iterator format(T const& Value, std::format_context& Context) const
    {
        return std::formatter<std::string>::format(Value.ToString(), Context);
    }
};
