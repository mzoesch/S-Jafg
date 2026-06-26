// Copyright mzoesch. All rights reserved.

#pragma once

namespace serde
{

//#
//# Whether this enum is marked as stable.
//# Changes to values of the enum or changing its size will break backwards compatibility.
//# Appending new values is ok. Modifying existing values is not. Changed the std::underlying_t is not ok.
//#
//# TODO: We need some kind of tool that can handle ABI changes. Maybe with ImHex?
//#
template<typename T> struct is_stable_enum : std::false_type {};
template<typename T> inline constexpr bool is_stable_enum_v{is_stable_enum<T>::value};
template<typename T> concept stable_enum = std::is_enum_v<T> && is_stable_enum_v<T>;

template<typename T> inline static constexpr bool is_primitive_v{std::is_arithmetic_v<T>||std::is_enum_v<T>};

enum struct arch_type : u8{ string, binary, };

enum struct behavior
{
    panic,
    log,
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

template<typename TArchive> inline constexpr bool panic_archive_v{ !requires(TArchive){ TArchive::behavior_mode; } || TArchive::behavior_mode == behavior::panic };
template<typename TArchive> inline constexpr bool log_archive_v{ requires(TArchive){ TArchive::behavior_mode; } && TArchive::behavior_mode == behavior::log };
template<typename TArchive> inline constexpr bool relaxed_archive_v{ requires(TArchive){ TArchive::behavior_mode; } && TArchive::behavior_mode == behavior::relaxed };

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

//# Struct to signal an archive to copy prefs.
struct LCopyPrefs final{};
template<typename TArchive, typename... TArgs> inline constexpr bool archive_prefs_copyable_v{ std::is_constructible_v<TArchive, LCopyPrefs, TArchive const&, TArgs&&...> };

template<typename T> struct is_string : std::false_type {};
template<typename T> inline constexpr bool string_v{is_string<T>::value};

//# The base struct for all serializers.
template<typename T, typename TArchive>
struct TSerializer;
//# The base struct for all deserializers.
template<typename T, typename TArchive>
struct TDeserializer;
//# The result of a deserialization.
struct LDeserializationResult final
{
    std::errc Errc;
    std::optional<LString> Error;
};
//# Whether T is serializable.
template<typename T, typename TArchive>
concept CSerializable = std::is_default_constructible_v<TSerializer<T, TArchive>> && (
       (string_archive_v<TArchive> && requires(TSerializer<T, TArchive> const& F, TArchive& Ar, T const& t){{F(Ar, t)}->std::same_as<void>;})
    || (bin_archive_v<TArchive> && requires(TSerializer<T, TArchive> const& F, TArchive& Ar, T const& t){{F(Ar, t)}->std::same_as<void>;})
    );
//# Whether T is deserializable.
template<typename T, typename TArchive>
concept CDeserializable = std::is_default_constructible_v<TDeserializer<T, TArchive>> && (
       (string_archive_v<TArchive> && requires(TDeserializer<T, TArchive> const& F, TArchive const& Ar, T& t){{F(Ar, t)}->std::same_as<LDeserializationResult>;})
    || (bin_archive_v<TArchive> && requires(TDeserializer<T, TArchive> const& F, TArchive& Ar, T& t){{F(Ar, t)}->std::same_as<LDeserializationResult>;})
    );
//# Whether T can be serialized and deserialized.
template<typename T, typename TArchive>
concept CTwoWaySerializable = CSerializable<T, TArchive> && CDeserializable<T, TArchive>;

//#
//# Serializes a type to/from its binary representation.
//# Usage example:
//#    struct LExample { LString Name; i32 Value; };
//#    SERDE_BIN_NON_INTRUSIVE(LExample, Name, Value)
//#
#define DETAIL_SERDE_BIN_NON_INTRUSIVE_TRANSFORM(X) (Field.X)
#define SERDE_BIN_NON_INTRUSIVE(T, ...) \
    template<typename TArchive, typename U> requires(::serde::bin_archive_for_v<TArchive, U> && std::is_same_v<std::remove_const_t<U>, T>) \
    inline void serde_non_intrusive(TArchive& Ar, U& Field) noexcept \
    { \
        Ar JAFG_MAP(DETAIL_SERDE_BIN_NON_INTRUSIVE_TRANSFORM, __VA_ARGS__) ; \
    }

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

template<typename TStream>
struct os_archive_base : public bin_archive_base<TStream>
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
    decltype(auto) write_pod(this auto&& Self, T const& x) noexcept
    {
        static_assert(std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T>);
        static_assert(!std::is_enum_v<T> || is_stable_enum_v<T>);
        check(Self.out_stream())
        if (!Self.m_stream.write(reinterpret_cast<char_type const*>(&x), sizeof(T)))
        {
            LOG_FATAL(LogSerialization, "[{}]: Failed to write primitive type.", algo::type_name<T>())
        }
        return std::forward<decltype(Self)>(Self);
    }

    template<algo::contiguous_range T> requires std::is_trivially_copyable_v<typename T::value_type>
    void write_input_range(this auto&& Self, T const& xs) noexcept
    {
        check(Self.out_stream())
        Self.write_pod(static_cast<range_size_type>(algo::size(xs)));
        Self.write_bulk(algo::data(xs), algo::size(xs));
        return;
    }
    template<algo::input_range T> requires(!(algo::contiguous_range<T> && std::is_trivially_copyable_v<typename T::value_type>))
    decltype(auto) write_input_range(this auto&& Self, T const& xs) noexcept
        requires requires(decltype(Self)& ar, typename T::value_type const& x){ar.write(x);}
    {
        check(Self.out_stream())
        Self.write_pod(static_cast<range_size_type>(algo::size(xs)));
        for (auto const& x : xs)
        {
            Self.operator()(x);
        }
        return std::forward<decltype(Self)>(Self);
    }

    template<typename T>
    decltype(auto) write_bulk(this auto&& Self, T const* xs, range_size_type n) noexcept
    {
        static_assert(std::is_trivially_copyable_v<T>);
        check(Self.out_stream())
        if (!Self.m_stream.write(reinterpret_cast<std::iostream::char_type const*>(xs), sizeof(T) * n))
        {
            LOG_FATAL(LogSerialization, "[{}]: Failed to write data of size [{}].", algo::type_name<T>(), sizeof(T) * n)
        }
        return std::forward<decltype(Self)>(Self);
    }
};

template<typename TStream>
struct is_archive_base : public bin_archive_base<TStream>
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

    template<typename T> requires(!std::is_const_v<T>)
    decltype(auto) read_pod(this auto&& Self, T& x) noexcept
    {
        static_assert(std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T>);
        static_assert(!std::is_enum_v<T> || is_stable_enum_v<T>);
        check(Self.in_stream())
        if (!Self.m_stream.read(reinterpret_cast<std::iostream::char_type*>(&x), sizeof(T)))
        {
            LOG_FATAL(LogSerialization, "[{}]: Failed to read primitive type.", algo::type_name<T>())
        }
        if (Self.m_stream.gcount() != sizeof(T))
        {
            LOG_FATAL(LogSerialization, "[{}]: Failed to read enough bytes for primitive type. Expected {} but got {}."
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
            LOG_FATAL(LogSerialization, "[{}]: Failed to read enough bytes for data. Expected {} but got {}."
                , algo::type_name<T>(), sizeof(T) * n, Self.m_stream.gcount())
        }
        return std::forward<decltype(Self)>(Self);
    }
};

} /* ~Namespace detail */

struct os_bin_archive final : public detail::os_archive_base<std::ostream>
{
    inline static constexpr auto open_mode{std::ios::out};
    typedef std::ostream stream_type;

    template<typename... TArgs> requires std::constructible_from<std::ostream, TArgs&&...>
    constexpr explicit os_bin_archive(TArgs&&... Args) noexcept
        : m_stream{std::forward<TArgs>(Args)...}
    {
    }

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

struct is_bin_archive final : public detail::is_archive_base<std::istream>
{
    inline static constexpr auto open_mode{std::ios::in};

    template<typename... TArgs> requires std::constructible_from<std::istream, TArgs&&...>
    constexpr explicit is_bin_archive(TArgs&&... Args) noexcept
        : m_stream{std::forward<TArgs>(Args)...}
    {
    }

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

struct ios_bin_archive : public detail::is_archive_base<std::iostream>, public detail::os_archive_base<std::iostream>
{
    inline static constexpr auto open_mode{std::ios::in|std::ios::out};

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
//# Serializes a type to/from a string.
//# Usage example:
//#    struct LExample { LString Name; i32 Value; };
//#    SERDE_STRING_NON_INTRUSIVE(LExample, Name, Value)
//#
#define DETAIL_SERDE_STRING_NON_INTRUSIVE_TRANSFORM(X) (Field.X)
#define SERDE_STRING_NON_INTRUSIVE(T, ...) \
    template<typename TArchive, typename U> requires(::serde::string_archive_for_v<TArchive, U> && std::is_same_v<std::remove_const_t<U>, T>) \
    inline void serde_non_intrusive(TArchive& Ar, U& Field) noexcept \
    { \
        Ar JAFG_MAP(DETAIL_SERDE_STRING_NON_INTRUSIVE_TRANSFORM, __VA_ARGS__) ; \
    }

//#
//# Serializes an enum to/from a string.
//# Usage example:
//#     enum struct EExample : u8 { A, B, C };
//#     SERDE_STRING_ENUM_NON_INTRUSIVE(EExample, A, B, C)
//#
#define DETAIL_SERDE_STRING_ENUM_NON_INTRUSIVE_TRANSFORM(X) {_serde_local_enum_t::X, #X},
#define SERDE_STRING_ENUM_NON_INTRUSIVE(T, ...) \
    template<typename TArchive, typename U> requires(::serde::string_archive_for_v<TArchive, U> && std::is_same_v<std::remove_const_t<U>, T>) \
    inline void serde_non_intrusive(TArchive& Ar, U& Field) noexcept \
    { \
        static_assert(std::is_enum_v<T>); \
        typedef T _serde_local_enum_t; \
        static constexpr std::pair<T, LStringView> const Members[]{ \
            JAFG_MAP(DETAIL_SERDE_STRING_ENUM_NON_INTRUSIVE_TRANSFORM, __VA_ARGS__) \
            }; \
        if constexpr (::serde::is_string_archive_v<TArchive>) \
        { \
            auto It{algo::find_if(Members, [&Ar](std::pair<T, LStringView> const& Member) \
            {\
                return Member.second == Ar.get_stream(); \
            })}; \
            if (It != std::end(Members)) \
            { \
                Field = It->first; \
            } \
            else \
            { \
                LOG_FATAL(LogSerialization, "Failed to deserialize enum [{}] from string [{}]." \
                    , algo::type_name<T>(), Ar.get_stream()) \
            } \
        } \
        else if constexpr (::serde::os_string_archive_v<TArchive>) \
        { \
            auto It{algo::find_if(Members, [&Field](std::pair<T, LStringView> const& Member) \
            {\
                return Member.first == Field; \
            })}; \
            if (It != std::end(Members)) \
            { \
                Ar.m_stream << It->second; \
            } \
            else \
            { \
                LOG_FATAL(LogSerialization, "Failed to serialize enum [{}] to string. No matching string found for value [{}]." \
                    , algo::type_name<T>(), std::to_underlying(Field)) \
            } \
        } \
        else \
        { \
            static_assert(::algo::always_false_v<T>, "Unsupported archive type for enum serialization."); \
        } \
    }

//#
//# Convenience macro that can define both string-archive serde and to_json/from_json non-intrusive functions.
//# Usage example:
//#     enum struct EExample : u8 { A, B, C };
//#     SERDE_STRING_AND_JSON_ENUM(EExample, A, B, C)
//#
#define SERDE_STRING_AND_JSON_ENUM(T, ...) \
    SERDE_STRING_ENUM_NON_INTRUSIVE(T, __VA_ARGS__) \
    SERDE_JSON_ENUM(T, __VA_ARGS__)

struct os_string_archive final
{
    inline static constexpr auto open_mode{std::ios::out};
    inline static constexpr auto arch_type{arch_type::string};
    inline static constexpr auto behavior_mode{behavior::panic};

    template<typename... TArgs> requires std::constructible_from<std::stringstream, TArgs&&...>
    constexpr explicit os_string_archive(TArgs&&... Args) noexcept
        : m_stream{std::forward<TArgs>(Args)...}
    {
    }

    //# Serde any type that can be serialized.
    template<typename T> requires(!std::is_pointer_v<T>)
    inline decltype(auto) operator()(this auto&& Self, T& x) noexcept requires string_archive_for_v<decltype(Self), T>
    {
        serde_non_intrusive(Self, x);
        return std::forward<decltype(Self)>(Self);
    }

    //# This archive is open for input. Therefore, you read from it.
    NODISCARD FORCEINLINE consteval bool in_stream() const noexcept { return false; }
    //# This archive is open for output. Therefore, you write to it.
    NODISCARD FORCEINLINE consteval bool out_stream() const noexcept { return true; }

    NODISCARD FORCEINLINE std::stringstream const& get_stream() const noexcept { return this->m_stream; }

    std::stringstream m_stream;
};

template<behavior Behavior>
struct is_string_archive final
{
    inline static constexpr auto open_mode{std::ios::in};
    inline static constexpr auto arch_type{arch_type::string};
    inline static constexpr auto behavior_mode{Behavior};

    template<typename... TArgs> requires std::constructible_from<LStringView, TArgs&&...>
    constexpr explicit is_string_archive(TArgs&&... Args) noexcept
        : m_stream{std::forward<TArgs>(Args)...}
    {
    }

    //# Serde any type that can be deserialized.
    template<typename T> requires(!std::is_const_v<T> && !std::is_pointer_v<T>)
    inline decltype(auto) operator()(this auto&& Self, T& x) noexcept requires string_archive_for_v<decltype(Self), T>
    {
        serde_non_intrusive(Self, x);
        return std::forward<decltype(Self)>(Self);
    }

    //# This archive is open for input. Therefore, you read from it.
    NODISCARD FORCEINLINE consteval bool in_stream() const noexcept { return true; }
    //# This archive is open for output. Therefore, you write to it.
    NODISCARD FORCEINLINE consteval bool out_stream() const noexcept { return false; }

    NODISCARD FORCEINLINE LStringView const& get_stream() const noexcept { return this->m_stream; }

    LStringView m_stream;
};




//# Lightweight default archive for output streams. Panics if anything goes wrong.
struct LOStringArchive final
{
    typedef char value_type;
    inline static constexpr auto open_mode{std::ios::out};
    inline static constexpr auto arch_type{arch_type::string};

    LOStringArchive() noexcept = default;
    LOStringArchive(LCopyPrefs, LOStringArchive const&) noexcept {}
    PROHIBIT_COPY(LOStringArchive)
    DEFAULT_MOVE(LOStringArchive)
    ~LOStringArchive() noexcept = default;

    template<typename T>
    inline decltype(auto) operator<<(this auto&& Self, T const& t) noexcept
        requires CSerializable<T, std::remove_cvref_t<decltype(Self)>>
    {
        TSerializer<T, LOStringArchive>{}(Self, t);
        return std::forward<decltype(Self)>(Self);
    }

    std::stringstream Stream;
};

//# Lightweight default archive for input streams. Panics if anything goes wrong.
template<typename TStream, behavior Behavior> requires std::is_same_v<LString, TStream> || std::is_same_v<LStringView, TStream>
struct LIStringArchive final
{
    typedef char value_type;
    inline static constexpr auto open_mode{std::ios::in};
    inline static constexpr auto arch_type{arch_type::string};

    constexpr LIStringArchive(TStream&& InStream) noexcept
        requires std::is_same_v<TStream, LString>
        : Stream(std::move(InStream)) {}
    constexpr LIStringArchive(TStream InStream) noexcept
        requires std::is_same_v<TStream, LStringView>
        : Stream(InStream) {}
    constexpr LIStringArchive(LCopyPrefs, LIStringArchive const&, TStream&& InStream) noexcept
        requires std::is_same_v<TStream, LString>
        : Stream(std::move(InStream)) {}
    constexpr LIStringArchive(LCopyPrefs, LIStringArchive const&, TStream InStream) noexcept
        requires std::is_same_v<TStream, LStringView>
        : Stream(InStream) {}

    DEFAULT_CONSTEXPR_REALLOC_OF_ANY_FORM(LIStringArchive)
    constexpr ~LIStringArchive() noexcept = default;

    template<typename T> requires(Behavior == behavior::panic)
    inline decltype(auto) operator>>(this auto&& Self, T& t) noexcept
        requires CDeserializable<T, std::remove_cvref_t<decltype(Self)>>
    {
        if (auto R{TDeserializer<T, LIStringArchive>{}(Self, t)}; R.Errc != decltype(R.Errc){})
        {
            if (R.Error.has_value())
            {
                LOG_ERROR(LogSerialization, "Serialization from string to [{}] failed: ", algo::type_name<T>(), *R.Error)
            }
            panicMsgf("Serialization from string to [{}] failed with [{}]", algo::type_name<T>(), std::to_underlying(R.Errc))
        }
        return std::forward<decltype(Self)>(Self);
    }

    template<typename T> requires(Behavior != behavior::panic)
    inline bool operator>>(this auto&& Self, T& t) noexcept
        requires CDeserializable<T, std::remove_cvref_t<decltype(Self)>>
    {
        if (auto R{TDeserializer<T, LIStringArchive>{}(Self, t)}; R.Errc != decltype(R.Errc){})
        {
            if constexpr (Behavior == behavior::log || Behavior == behavior::panic)
            {
                if (R.Error.has_value())
                {
                    LOG_ERROR(LogSerialization, "Serialization from string to [{}] failed: ", algo::type_name<T>(), *R.Error)
                }
            }
           return false;
        }
        return true;
    }

    TStream Stream;
};

template<typename T, typename TArchive> requires CSerializable<T, TArchive>
    && os_string_archive_v<TArchive>
    && archive_prefs_copyable_v<TArchive>
struct TSerializer<TArray<T>, TArchive>
{
    void operator()(TArchive& Ar, TArray<T> const& Field) const noexcept
    {
        auto& Ss{Ar.Stream};
        Ss << '[';

        bool bFirst{true};
        for (T const& Elem : Field)
        {
            if (!bFirst)
            {
                Ss << ',';
            }
            bFirst = false;

            TArchive IntermediateAr{LCopyPrefs{}, Ar};
            TSerializer<T, TArchive>{}(IntermediateAr, Elem);
            for (LString Intermediate{IntermediateAr.Stream.str()}; auto const& C : Intermediate)
            {
                if (C == '[' || C == ']' || C == ',' || C == '\\')
                {
                    Ss << '\\';
                }
                Ss << C;
            }
        }

        Ss << ']';
        return;
    }
};

template<typename T, typename TArchive> requires CDeserializable<T, TArchive>
    && is_string_archive_v<TArchive>
    && archive_prefs_copyable_v<TArchive, LString&&>
struct TDeserializer<TArray<T>, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, TArray<T>& Field) const noexcept
    {
        auto& Value{Ar.Stream};
        TArray<T> Out;

        if (Value.starts_with('[') == false)
        {
            return {.Errc=std::errc::invalid_argument, .Error=algo::sprintf("Expected '[' at the start of: \n{}", Value)};
        }
        if (Value.ends_with(']') == false)
        {
            return {.Errc=std::errc::invalid_argument, .Error=algo::sprintf("Expected ']' at the end of: \n{}", Value)};
        }

        const LStringView View{Value.begin() + 1, Value.end() - 1};
        if (View.empty())
        {
            Field = std::move(Out);
            return {};
        }

        LString Element;
        bool bEscaped{};
        bool bTrailingComma{};
        for (const auto Char : View)
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
                TArchive IntermediateAr{LCopyPrefs{}, Ar, std::move(Element)};
                if (auto R{TDeserializer<T, TArchive>{}(IntermediateAr, Out.back())}; R.Errc != decltype(R.Errc){})
                {
                    return {
                        .Errc = R.Errc,
                        .Error = algo::sprintf("Subargument failed with [{}] at [{}]."
                            , R.Error.has_value() ? R.Error.value() : "<unknown error>"
                            , Element
                            )
                        };
                }

                Element.clear();
                continue;
            }

            Element += Char;
            continue;
        }

        if (bTrailingComma)
        {
            check(Element.empty())
        }
        else
        {
            Out.emplace_back();
            TArchive IntermediateAr{LCopyPrefs{}, Ar, std::move(Element)};
            if (auto R{TDeserializer<T, TArchive>{}(IntermediateAr, Out.back())}; R.Errc != decltype(R.Errc){})
            {
                return {
                    .Errc = R.Errc,
                    .Error = algo::sprintf("Subargument failed with [{}] at [{}]."
                        , R.Error.has_value() ? R.Error.value() : "<unknown error>"
                        , Element
                        )
                    };
            }
        }

        Field = std::move(Out);
        return {};
    }
};

template<typename T, typename TArchive> requires((std::is_same_v<T, LString> || std::is_same_v<T, LPath> || std::is_same_v<T, LStringView>)
    && os_string_archive_v<TArchive>)
struct TSerializer<T, TArchive>
{
    void operator()(TArchive& Ar, T const& Field) const noexcept
    {
        Ar.Stream << Field;
    }
};
template<typename TArchive> requires is_string_archive_v<TArchive>
struct TDeserializer<LString, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, LString& Field) const noexcept
    {
        Field.assign(Ar.Stream);
        return {};
    }
};
template<typename TArchive> requires is_string_archive_v<TArchive>
struct TDeserializer<LPath, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, LPath& Field) const noexcept
    {
        Field.assign(Ar.Stream);
        return {};
    }
};
template<typename TArchive> requires is_string_archive_v<TArchive>
struct TDeserializer<LStringView, TArchive>
{
    //# Cannot store intermediate text in a non onwing object.
    constexpr TDeserializer() noexcept = delete;
};

template<typename T, typename TArchive> requires((std::is_integral_v<T> || std::is_floating_point_v<T>)
    && os_string_archive_v<TArchive>)
struct TSerializer<T, TArchive>
{
    void operator()(TArchive& Ar, T const& Field) const noexcept
    {
        Ar.Stream << Field;
    }
};
template<typename T, typename TArchive> requires((std::is_integral_v<T> || std::is_floating_point_v<T>)
    && is_string_archive_v<TArchive>)
struct TDeserializer<T, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, T& Field) const noexcept
    {
        auto R{std::from_chars(algo::data(Ar.Stream), algo::data(Ar.Stream) + algo::size(Ar.Stream), Field)};
        return {
            .Errc = R.ec,
            .Error = R.ptr ? LString{R.ptr} : std::optional<LString>{},
            };
    }
};

template<maths::length_t L, typename TReal, maths::qual_t Q, typename TArchive>
    requires((std::is_integral_v<TReal> || std::is_floating_point_v<TReal>) && os_string_archive_v<TArchive>)
struct TSerializer<TVec<L,TReal,Q>, TArchive>
{
    void operator()(TArchive& Ar, TVec<L,TReal,Q> const& Field) const noexcept
    {
        Ar.Stream << "vec" << L << "(";
        for (maths::length_t Idx{0}; Idx < L; ++Idx)
        {
            if (Idx > 0)
            {
                Ar.Stream << ',';
            }
            Ar.Stream << Field[Idx];
        }
        Ar.Stream << ")";
    }
};
template<maths::length_t L, typename TReal, maths::qual_t Q, typename TArchive>
    requires((std::is_integral_v<TReal> || std::is_floating_point_v<TReal>) && is_string_archive_v<TArchive>)
struct TDeserializer<TVec<L,TReal,Q>, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, TVec<L,TReal,Q>& Field) const noexcept
    {

        auto& Value{Ar.Stream};

        if (!Value.starts_with("vec"))
        {
            return {.Errc=std::errc::invalid_argument, .Error=algo::sprintf("Expected 'vec' at the start of: \n{}", Value)};
        }
        if (!Value.ends_with(")"))
        {
            return {.Errc=std::errc::invalid_argument, .Error=algo::sprintf("Expected ')' at the end of: \n{}", Value)};
        }

        LStringView View{Value.begin() + 3, Value.end() - 1};
        if (View.empty())
        {
            return {
                .Errc = std::errc::invalid_argument,
                .Error = algo::sprintf("Expected vector length and components in: \n{}", Value),
                };
        }

        maths::length_t l{};
        auto R{std::from_chars(algo::data(View), algo::data(View) + 1, l)};
        if (R.ec != std::errc{})
        {
            return {
                .Errc = R.ec,
                .Error = R.ptr ? LString{R.ptr} : std::optional<LString>{},
                };
        }
        if (l != L)
        {
            return {
                .Errc = std::errc::invalid_argument,
                .Error = algo::sprintf("Expected vector of length '{}' but got '{}' in: \n{}", L, l, Value),
                };
        }
        View = LStringView{View.begin()+1, View.end()};
        if (View.empty())
        {
            return {
                .Errc = std::errc::invalid_argument,
                .Error = algo::sprintf("Expected vector components in: \n{}", Value),
                };
        }
        if (View[0] != '(')
        {
            return {
                .Errc = std::errc::invalid_argument,
                .Error = algo::sprintf("Expected '(' at the start of vector components in: \n{}", Value),
                };
        }

        View = LStringView{View.begin() + 1, View.end()};

        if constexpr (L == 1)
        {
            auto R{std::from_chars(algo::data(View), algo::data(View) + 1, Field.x)};
            return {
                .Errc = R.ec,
                .Error = R.ptr ? LString{R.ptr} : std::optional<LString>{},
                };
        }

        // x comp
        if constexpr (L >= 1)
        {
            auto Pos{View.find_first_of(',')};
            if (Pos == LStringView::npos)
            {
                return {
                    .Errc = std::errc::invalid_argument,
                    .Error = algo::sprintf("Expected ',' after x component in: \n{}", Value),
                    };
            }

            LStringView ComponentX{View.begin(), Pos};
            auto R{std::from_chars(algo::data(ComponentX), algo::data(ComponentX) + algo::size(ComponentX), Field.x)};
            if (R.ec != std::errc{})
            {
                return {
                    .Errc = R.ec,
                    .Error = R.ptr ? LString{R.ptr} : std::optional<LString>{},
                    };
            }

            View = LStringView{View.begin() + Pos + 1, View.end()};
        }
        if constexpr (L >= 2)
        {
            // y comp
            auto Pos{View.find_first_of(',')};
            if (Pos == LStringView::npos)
            {
                return {
                    .Errc = std::errc::invalid_argument,
                    .Error = algo::sprintf("Expected ',' after y component in: \n{}", Value),
                    };
            }

            LStringView ComponentY{View.begin(), Pos};
            auto R{std::from_chars(algo::data(ComponentY), algo::data(ComponentY) + algo::size(ComponentY), Field.y)};
            if (R.ec != std::errc{})
            {
                return {
                    .Errc = R.ec,
                    .Error = R.ptr ? LString{R.ptr} : std::optional<LString>{},
                    };
            }

            View = LStringView{View.begin() + Pos + 1, View.end()};
        }
        if constexpr (L >= 3)
        {
            // z comp
            auto Pos{View.find_first_of(',')};
            if (Pos == LStringView::npos)
            {
                return {
                    .Errc = std::errc::invalid_argument,
                    .Error = algo::sprintf("Expected ',' after z component in: \n{}", Value),
                    };
            }

            LStringView ComponentZ{View.begin(), Pos};
            auto R{std::from_chars(algo::data(ComponentZ), algo::data(ComponentZ) + algo::size(ComponentZ), Field.z)};
            if (R.ec != std::errc{})
            {
                return {
                    .Errc = R.ec,
                    .Error = R.ptr ? LString{R.ptr} : std::optional<LString>{},
                    };
            }

            View = LStringView{View.begin() + Pos + 1, View.end()};
        }
        if constexpr (L >= 4)
        {
            // w comp
            auto Pos{View.find_first_of(',')};
            if (Pos == LStringView::npos)
            {
                return {
                    .Errc = std::errc::invalid_argument,
                    .Error = algo::sprintf("Expected ',' after w component in: \n{}", Value),
                    };
            }

            LStringView ComponentW{View.begin(), Pos};
            auto R{std::from_chars(algo::data(ComponentW), algo::data(ComponentW) + algo::size(ComponentW), Field.w)};
            if (R.ec != std::errc{})
            {
                return {
                    .Errc = R.ec,
                    .Error = R.ptr ? LString{R.ptr} : std::optional<LString>{},
                    };
            }

            View = LStringView{View.begin() + Pos + 1, View.end()};
        }
        if constexpr (L >= 5)
        {
            static_assert(algo::always_false_v<TVec<L,TReal,Q>>, "Unsupported vector length for deserialization.");
        }

        return {};
    }
};

template<typename TArchive> requires os_string_archive_v<TArchive>
struct TSerializer<bool, TArchive>
{
    void operator()(TArchive& Ar, bool const& Field) const noexcept
    {
        if (Field)
        {
            Ar.Stream << "1";
        }
        else
        {
            Ar.Stream << "0";
        }
    }
};
template<typename TArchive> requires is_string_archive_v<TArchive>
struct TDeserializer<bool, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, bool& Field) const noexcept
    {
        auto& Value{Ar.Stream};
        if (Value == "false" || Value == "0")
        {
            Field = false;
            return {};
        }
        if (Value == "true" || Value == "1")
        {
            Field = true;
            return {};
        }
        return {
            .Errc = std::errc::invalid_argument,
            .Error = algo::sprintf("Could not interpret [{}] as a boolean.", Value)
            };
    }
};


template<typename T, typename TArchive> requires std::is_enum_v<T>
    && os_string_archive_v<TArchive>
struct TSerializer<T, TArchive>
{
    void operator()(TArchive& Ar, T const& Field) const noexcept
    {
        // As this is a text o archive,
        // we have to avoid printing something like a char that is null, as this would end the string.
        if constexpr (std::is_signed_v<std::underlying_type_t<T>>)
        {
            TSerializer<i64, TArchive>{}(Ar, static_cast<i64>(std::to_underlying(Field)));
        }
        else
        {
            TSerializer<u64, TArchive>{}(Ar, static_cast<u64>(std::to_underlying(Field)));
        }
    }
};

//# TODO: Is is of course shit. Because we cannot verify the cast. But lets wait for c++26 to fix this...
template<typename T, typename TArchive> requires std::is_enum_v<T>
    && is_string_archive_v<TArchive>
struct TDeserializer<T, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, T& Field) const noexcept
    {
        std::underlying_type_t<T> Temp{static_cast<std::underlying_type_t<T>>(Field)};
        auto R{TDeserializer<std::underlying_type_t<T>, TArchive>{}(Ar, Temp)};
        Field = static_cast<T>(Temp);
        return R;
    }
};

template<typename TArchive> requires os_string_archive_v<TArchive>
struct TSerializer<LColor, TArchive>
{
    void operator()(TArchive& Ar, LColor const& Field) const noexcept
    {
        Ar.Stream << algo::sprintf("0x{:02X}{:02X}{:02X}{:02X}", Field.r, Field.g, Field.b, Field.a);
    }
};
template<typename TArchive> requires is_string_archive_v<TArchive>
struct TDeserializer<LColor, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, LColor& Field) const noexcept
    {
        auto& Value{Ar.Stream};

        if (Value.starts_with("0x") == false)
        {
            return {
                .Errc = std::errc::invalid_argument,
                .Error = algo::sprintf("Expected '0x' at the start of: {}", Value)
                };
        }

        if (Value.size() != 10)
        {
            return {
                .Errc = std::errc::invalid_argument,
                .Error = algo::sprintf("Expected a size of '10' but got '{}' from: {}", Value.size(), Value)
                };
        }

        for (auto Idx{2uz}; Idx < Value.size(); ++Idx)
        {
            if ((Value[Idx] >= '0' && Value[Idx] <= '9') || (Value[Idx] >= 'A' && Value[Idx] <= 'F'))
            {
                continue;
            }

            return{
                .Errc = std::errc::invalid_argument,
                .Error = algo::sprintf("Expected one of the values of [0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F] but got '{}' at index '{}' from: {}",
                    Value[Idx], Idx, Value
                    )
                };
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

        return {};
    }
};

//# Quick conversion of the field T to a string with default formation.
template<typename T> requires CSerializable<T, LOStringArchive>
NODISCARD FORCEINLINE LString ToString(T const& Field) noexcept
{
    LOStringArchive Ar;
    Ar << Field;
    return Ar.Stream.str();
}



template<typename T> requires string_archive_for_v<os_string_archive, T> && serde_for_v<os_string_archive, T const>
NODISCARD FORCEINLINE LString to_string(T const& Field) noexcept
{
    os_string_archive Ar;
    Ar(Field);
    return Ar.get_stream().str();
}

//# Quick conversion of the formation from T to T. If an error occurs the program will panic.
template<typename T> requires CDeserializable<T, LIStringArchive<LStringView, behavior::panic>>
FORCEINLINE void FromString(T* Field, LStringView Value) noexcept
{
    LIStringArchive<LStringView, behavior::panic> Ar{Value};
    Ar >> *Field;
    return;
}

//# Quick conversion of the formation from T to T. If an error occurs the program will panic.
template<typename T> requires string_archive_for_v<is_string_archive<behavior::panic>, T>
FORCEINLINE void from_string(T* Field, LStringView Value) noexcept
{
    is_string_archive<behavior::panic> Ar{Value};
    Ar(*Field);
    return;
}
template<typename T> requires string_archive_for_v<is_string_archive<behavior::panic>, T>
    && std::is_default_constructible_v<T> && std::is_move_constructible_v<T>
FORCEINLINE T from_string(LStringView Value) noexcept
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
    return;
}
template<typename T> requires string_archive_for_v<is_string_archive<behavior::log>, T>
    && std::is_default_constructible_v<T> && std::is_move_constructible_v<T>
FORCEINLINE T from_string_logged(LStringView Value) noexcept
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
    return;
}
template<typename T> requires string_archive_for_v<is_string_archive<behavior::relaxed>, T>
    && std::is_default_constructible_v<T> && std::is_move_constructible_v<T>
FORCEINLINE T from_string_relaxed(LStringView Value) noexcept
{
    T Field{};
    is_string_archive<behavior::relaxed> Ar{Value};
    Ar(Field);
    return Field;
}

//#
//# Quick conversion of the formation from T to T. If an error occurs it will return false and log a message
//# if logging for LogSerialization at error verbosity is enabled.
//#
template<typename T> requires CDeserializable<T, LIStringArchive<LStringView, behavior::log>>
FORCEINLINE bool FromStringLogged(T* Field, LStringView Value) noexcept
{
    LIStringArchive<LStringView, behavior::log> Ar{Value};
    return Ar >> *Field;
}

//# Quick conversion of the formation from T to T. If an error occurs it will return false.
template<typename T> requires CDeserializable<T, LIStringArchive<LStringView, behavior::relaxed>>
FORCEINLINE bool FromStringRelaxed(T* Field, LStringView Value) noexcept
{
    LIStringArchive<LStringView, behavior::relaxed> Ar{Value};
    return Ar >> *Field;
}

template<> struct is_string<LString> : std::true_type {};
template<> struct is_string<Lu8String> : std::true_type {};
template<> struct is_string<Lu16String> : std::true_type {};
template<> struct is_string<Lu32String> : std::true_type {};
template<> struct is_string<LStringView> : std::true_type {};
template<> struct is_string<Lu8StringView> : std::true_type {};
template<> struct is_string<Lu16StringView> : std::true_type {};
template<> struct is_string<Lu32StringView> : std::true_type {};
template<> struct is_string<char const*> : std::true_type {};
template<> struct is_string<unsigned char const*> : std::true_type {};
template<> struct is_string<signed char const*> : std::true_type {};
template<> struct is_string<char8_t const*> : std::true_type {};
template<> struct is_string<char16_t const*> : std::true_type {};
template<> struct is_string<char32_t const*> : std::true_type {};

template<typename TArchive, typename U> requires(bin_archive_for_v<TArchive, U> && is_primitive_v<U>)
inline void serde_non_intrusive(TArchive& Ar, U& Field) noexcept
{
    if constexpr (ios_bin_archive_v<TArchive>)
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
    else if constexpr (is_bin_archive_v<TArchive>)
    {
        Ar.read_pod(Field);
    }
    else if constexpr (os_bin_archive_v<TArchive>)
    {
        Ar.write_pod(Field);
    }
    else
    {
        static_assert(algo::always_false_v<TArchive, U>);
    }

    return;
}

template<typename TArchive, typename U> requires(string_archive_for_v<TArchive, U> && (std::is_integral_v<U> || std::is_floating_point_v<U>))
inline void serde_non_intrusive(TArchive& Ar, U& Field) noexcept
{
    if constexpr (is_string_archive_v<TArchive>)
    {
        auto R{std::from_chars(algo::data(Ar.get_stream()), algo::data(Ar.get_stream()) + algo::size(Ar.get_stream()), Field)};
        if constexpr (panic_archive_v<TArchive>)
        {
            if (R.ec != decltype(R.ec){})
            {
                LOG_FATAL(LogSerialization, "[{}]: Failed to deserialize from [{}] with error [{}]."
                    , algo::type_name<U>(), Ar.get_stream(), std::to_underlying(R.ec))
            }
        }
        else if constexpr (log_archive_v<TArchive>)
        {
            if (R.ec != decltype(R.ec){})
            {
                LOG_ERROR(LogSerialization, "[{}]: Failed to deserialize from [{}] with error [{}]."
                    , algo::type_name<U>(), Ar.get_stream(), std::to_underlying(R.ec))
            }
        }
        else if constexpr (relaxed_archive_v<TArchive>)
        {
        }
        else
        {
            static_assert(algo::always_false_v<TArchive, U>);
        }
    }
    else if constexpr (os_string_archive_v<TArchive>)
    {
        Ar.m_stream << Field;
    }
    else
    {
        static_assert(algo::always_false_v<TArchive, U>);
    }

    return;
}

template<typename TArchive, algo::input_range U> requires(bin_archive_for_v<TArchive, U>)
inline void serde_non_intrusive(TArchive& Ar, U& Field) noexcept
{
    if constexpr (ios_bin_archive_v<TArchive>)
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
    else if constexpr (is_bin_archive_v<TArchive>)
    {
        Ar.read_input_range(Field);
    }
    else if constexpr (os_bin_archive_v<TArchive>)
    {
        Ar.write_input_range(Field);
    }
    else
    {
        static_assert(algo::always_false_v<TArchive, U>);
    }

    return;
}

template<typename TArchive> requires(bin_archive_for_v<TArchive, LPath> && ios_bin_archive_v<TArchive>)
inline void serde_non_intrusive(TArchive& Ar, LPath& Field) noexcept
{
    if (Ar.in_stream())
    {
        LString Dummy;
        Ar(Dummy);
        Field = std::move(Dummy);
    }
    else
    {
        LString Dummy{Field};
        Ar(Dummy);
    }

    return;
}

template<typename TArchive> requires(bin_archive_for_v<TArchive, LPath> && is_bin_archive_v<TArchive>)
inline void serde_non_intrusive(TArchive& Ar, LPath& Field) noexcept
{
    LString Dummy;
    Ar(Dummy);
    Field = std::move(Dummy);
    return;
}

template<typename TArchive> requires(bin_archive_for_v<TArchive, LPath> && os_bin_archive_v<TArchive>)
inline void serde_non_intrusive(TArchive& Ar, LPath const& Field) noexcept
{
    LString Dummy{Field};
    Ar(Dummy);
    return;
}

template<typename TArchive, typename U> requires(bin_archive_for_v<TArchive, U> && ios_bin_archive_v<TArchive>)
inline void serde_non_intrusive(TArchive& Ar, std::optional<U>& Field) noexcept
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

    return;
}

template<typename TArchive, typename U> requires(bin_archive_for_v<TArchive, U> && is_bin_archive_v<TArchive>)
inline void serde_non_intrusive(TArchive& Ar, std::optional<U>& Field) noexcept
{
    static_assert(std::is_default_constructible_v<U>);

    typename TArchive::optional_marker_type Marker;
    Ar(Marker);
    if (Marker)
    {
        Field.emplace();
        Ar(*Field);
    }

    return;
}

template<typename TArchive, typename U> requires(bin_archive_for_v<TArchive, U> && os_bin_archive_v<TArchive>)
inline void serde_non_intrusive(TArchive& Ar, std::optional<U> const& Field) noexcept
{
    typename TArchive::optional_marker_type Marker{Field.has_value()};
    Ar(Marker);
    if (Field.has_value())
    {
        Ar(*Field);
    }

    return;
}

} /* ~Namespace serde */

template<typename T> requires requires(T t) { {t.ToString()} -> std::convertible_to<LString>; }
struct std::formatter<T> : std::formatter<std::string>
{
    FORCEINLINE std::format_context::iterator format(T const& Value, std::format_context& Context) const
    {
        return std::formatter<std::string>::format(Value.ToString(), Context);
    }
};
