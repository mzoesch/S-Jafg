// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template <typename TIn, std::integral TSizeType>
using TPathBaseDefaultUtf8Traits = TStringBaseDefaultUtf8Traits<TIn, TSizeType>;

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
class TPathBase : public TStringBase<TEncoding, TAllocator>
{
public:

    typedef TStringBase<TEncoding, TAllocator> Super;

    typedef typename Super::Encoding       Encoding;
    typedef typename Super::Allocator      Allocator;
    typedef typename Super::T              T;
    typedef typename Super::SizeType       SizeType;
    typedef typename Super::Iterator       Iterator;
    typedef typename Super::ConstIterator  ConstIterator;
    typedef typename Super::Pointer        Pointer;
    typedef typename Super::ConstPointer   ConstPointer;
    typedef typename Super::Reference      Reference;
    typedef typename Super::ConstReference ConstReference;

    typedef TPathBase<TPathBaseDefaultUtf8Traits, TArrayView<LJafgChar>> _WeakPathRepr;
    typedef TPathBase<TPathBaseDefaultUtf8Traits, TMutableArrayView<LJafgChar>> _WeakMutablePathRepr;

    using Super::TStringBase;

    template <TStringBaseAllocatorConcept UAllocator>
        requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>)
    FORCEINLINE constexpr TPathBase(const TPathBase<TEncoding, UAllocator>& Other) noexcept
        requires(std::is_constructible_v<Allocator, const UAllocator&> && (std::is_same_v<TPathBase, _WeakPathRepr> || std::is_same_v<TPathBase, _WeakMutablePathRepr>));

    template <TStringBaseConcept TOther = TPathBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase, TOther>)
    FORCEINLINE TOther AppendPathToNew(const auto& Other) const noexcept{ return this->template AppendPathToNew<ConstIterator, ConstIterator, TOther>(Other.begin(), Other.end()); }
    template <TStringBaseConcept TOther = TPathBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase, TOther>)
    FORCEINLINE TOther AppendPathToNew(const ConstPointer String) const noexcept { return this->template AppendPathToNew<TOther>(String, Encoding::GetStringLength(String)); }
    template <TStringBaseConcept TOther = TPathBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase, TOther>)
    FORCEINLINE TOther AppendPathToNew(const ConstPointer String, const SizeType Length) const noexcept { return this->template AppendPathToNew<TOther>(String, String + Length); }
    template <TStringBaseConcept TOther = TPathBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase, TOther>)
    FORCEINLINE TOther AppendPathToNew(const ConstPointer Begin, const ConstPointer End) const noexcept { return this->template AppendPathToNew<ConstIterator, ConstIterator, TOther>(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TStringBaseConcept TOther = TPathBase> requires(TIteratorPairConcept<UIterator, VIterator> && TValidOtherStringConcept<TPathBase, TOther> && TOther::IsOwningString())
    FORCEINLINE TOther AppendPathToNew(const UIterator Begin, const VIterator End) const noexcept { return Super::template AppendPathToNew<UIterator, VIterator, TOther>(Begin, End); }
    template <TStringBaseConcept TOther = TPathBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase, TOther>)
    FORCEINLINE TOther AppendPathToNew(const T Rune) const noexcept { return this->template AppendPathToNew<TOther>(&Rune, &Rune + 1); }

    FORCEINLINE TPathBase operator/(const auto& Other) const & noexcept requires(typename Allocator::IsAllowedToPushItems()) { return this->template AppendPathToNew<TPathBase>(Other.begin(), Other.end()); }
    FORCEINLINE TPathBase operator/(const ConstPointer String) const & noexcept requires(Allocator::IsAllowedToPushItems()) { return this->template AppendPathToNew<TPathBase>(String); }
    FORCEINLINE TPathBase operator/(const T Rune) const & noexcept requires(Allocator::IsAllowedToPushItems()) { return this->template AppendPathToNew<TPathBase>(Rune); }
    FORCEINLINE TPathBase&& operator/(const auto& Other) && noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(Other.begin(), Other.end()); return std::move(*this); }
    FORCEINLINE TPathBase&& operator/(const ConstPointer String) && noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(String); return std::move(*this); }
    FORCEINLINE TPathBase&& operator/(const T Rune) && noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(Rune); return std::move(*this); }

    FORCEINLINE TPathBase& operator/=(const auto& Other) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(Other.begin(), Other.end()); return *this; }
    FORCEINLINE TPathBase& operator/=(const ConstPointer String) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(String); return *this; }
    FORCEINLINE TPathBase& operator/=(const T Rune) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(Rune); return *this; }

    FORCEINLINE bool IsPosix() const noexcept { return this->FindFirst('\\') == this->end(); }
    //# Will normalize the path if the underlying platform is not Unix like. Else this method resolves to nothing.
    FORCEINLINE void ToPosix() noexcept requires(Allocator::IsContentMutable());
    template <TStringBaseConcept TOther = TPathBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase, TOther>)
    FORCEINLINE TOther GetPosix() const noexcept;
    //# Will always convert this to a Posix path.
    FORCEINLINE void ToPosixSecure() noexcept requires(Allocator::IsContentMutable()) { this->Replace('\\', '/'); }
    template <TStringBaseConcept TOther = TPathBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase, TOther>)
    FORCEINLINE TOther GetPosixSecure() const noexcept;

    // canonical

    //# Whether the path exists on the underlying filesystem.
    FORCEINLINE bool DoesExist() const noexcept;
    //# Whether the path is a file.
    FORCEINLINE bool IsFile() const noexcept;
    //# Whether the path is a directory.
    FORCEINLINE bool IsDirectory() const noexcept;

    //# Whether the path is absolute.
    bool IsAbsolute() const noexcept;
    void MakeAbsolute() noexcept requires(Super::IsOwningString() && Super::IsContentMutable());
    template <TStringBaseConcept TOther = TPathBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase, TOther>)
    TOther GetAbsolute() const noexcept;

    //# Whether the path is relative.
    bool IsRelative() const noexcept;
    bool IsRelativeValid() const noexcept;
    void MakeRelativePath() noexcept;
    template <TStringBaseConcept TOther = TPathBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase, TOther>)
    TOther GetRelativePath() const noexcept;

    //# The actual extension of the filename.
    ConstIterator FindExtension() const noexcept;
    bool IsExtensionValid() const noexcept;
    template <TStringBaseConcept TOther = TPathBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase, TOther>)
    TOther GetExtension() const noexcept;
    TPathBase& RemoveExtension() & noexcept;
    TPathBase& RemoveExtension() && noexcept;
    TPathBase& ReplaceExtension(const TPathBase& InExtension) noexcept requires(Super::IsContentMutable() && Allocator::IsAllowedToPushItems());

    //# The actual filename with the extension.
    ConstIterator FindFileName() const noexcept;
    bool IsFilenameValid() const noexcept;
    template <TStringBaseConcept TOther = TPathBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase, TOther>)
    TOther GetFilename() const noexcept;
    TPathBase& RemoveFilename() noexcept;
    TPathBase& ReplaceFilename(const TPathBase& InExtension) noexcept requires(Super::IsContentMutable() && Allocator::IsAllowedToPushItems());

    //# The parent directory of the path, which is the directory that contains the file or directory represented by this path.
    NODISCARD ConstIterator FindParentEnd() const noexcept;
    NODISCARD bool IsParentValid() const noexcept;
    template <TStringBaseConcept TOther = TPathBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase, TOther>)
    NODISCARD TOther GetParent() const noexcept;
    void ToParent() noexcept requires(TPathBase::IsOwningString());

    //# Combination of the root name (if available on the underlying platform) and its directory.
    NODISCARD bool IsRootValid() const noexcept;
    template <TStringBaseConcept TOther = TPathBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase, TOther>)
    NODISCARD TOther GetRoot() const noexcept;

    //# On Posix platforms this is always false, else this checks the driver letter of the underlying path.
    NODISCARD bool IsRootNameValid() const noexcept;
    template <TStringBaseConcept TOther = TPathBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase, TOther>)
    NODISCARD TOther GetRootName() const noexcept;

    //# On Posix platforms this checks for a leading slash, else this checks for a backslash after the driver letter.
    NODISCARD bool IsRootDirectoryValid() const noexcept;
    template <TStringBaseConcept TOther = TPathBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase, TOther>)
    NODISCARD TOther GetRootDirectory() const noexcept;

    //# Whether the filename (without the extension) results to a non-empty string.
    NODISCARD bool IsStemValid() const noexcept;
    template <TStringBaseConcept TOther = TPathBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase, TOther>)
    NODISCARD TOther GetStem() const noexcept;

    template <TStringBaseConcept TOther = LString> requires(std::same_as<typename TOther::Encoding, Encoding>)
    NODISCARD
    FORCEINLINE TOther ToString() noexcept { return TOther{ this->begin(), this->end() }; }

    template <TStringBaseConcept TOther = LString> requires(std::same_as<typename TOther::Encoding, Encoding>)
    NODISCARD
    FORCEINLINE TOther ToString() const noexcept { return TOther{ this->begin(), this->end() }; }

private:

    FORCEINLINE void RemoveExtensionImpl() noexcept;
};

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>)
constexpr TPathBase<TEncoding, TAllocator>::TPathBase(const TPathBase<TEncoding, UAllocator>& Other) noexcept
    requires
    (
            std::is_constructible_v<typename Super::Allocator, const UAllocator&>
        && (std::is_same_v<TPathBase, _WeakPathRepr> || std::is_same_v<TPathBase, _WeakMutablePathRepr>)
    )
    : Super{static_cast<const Lal::TStringBase<TEncoding, UAllocator>&>(Other)}
{
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TPathBase<TEncoding, TAllocator>::ToPosix() noexcept
    requires(Allocator::IsContentMutable())
{
#if PLATFORM_WINDOWS
        this->Replace('\\', '/');
#endif /* PLATFORM_WINDOWS */

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseConcept TOther>
    requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase<TEncoding, TAllocator>, TOther>)
FORCEINLINE TOther TPathBase<TEncoding, TAllocator>::GetPosix() const noexcept
{
    TOther Result{ *this };
    Result.ToPosix();
    return Result;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseConcept TOther>
    requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase<TEncoding, TAllocator>, TOther>)
FORCEINLINE TOther TPathBase<TEncoding, TAllocator>::GetPosixSecure() const noexcept
{
    TOther Result{ *this };
    Result.ToPosixSecure();
    return Result;
}

template <template <typename, typename> typename TEncoding, typename T>
using THeapPath = TPathBase<TEncoding, TArray<T>>;
template <template <typename, typename> typename TEncoding, typename T, LSize TPathCapacity>
using TSmallPath = TPathBase<TEncoding, TStackArray<T, TPathCapacity>>;
template <template <typename, typename> typename TEncoding, typename T, LSize TPathCapacity>
using TOptimizedPath = TPathBase<TEncoding, TStackOptimizedArray<T, TPathCapacity>>;

template <template <typename, typename> typename TEncoding, typename T>
using TPathView = TPathBase<TEncoding, TArrayView<T>>;
template <template <typename, typename> typename TEncoding, typename T>
using TMutablePathView = TPathBase<TEncoding, TMutableArrayView<T>>;

template <typename T>
using THeapPathUtf8 = THeapPath<TPathBaseDefaultUtf8Traits, T>;
template <typename T, LSize TPathCapacity>
using TSmallPathUtf8 = TSmallPath<TPathBaseDefaultUtf8Traits, T, TPathCapacity>;
template <typename T, LSize TPathCapacity>
using TOptimizedPathUtf8 = TOptimizedPath<TPathBaseDefaultUtf8Traits, T, TPathCapacity>;

template <typename T>
using TPathViewUtf8 = TPathView<TPathBaseDefaultUtf8Traits, T>;
template <typename T>
using TMutablePathViewUtf8 = TMutablePathView<TPathBaseDefaultUtf8Traits, T>;

} /* ~Namespace Lal */

typedef Lal::TOptimizedPathUtf8<LJafgChar, 8> LPath;
typedef Lal::THeapPathUtf8<LJafgChar> LBigPath;
template <LSize TCapacity>
using LSmallPath = Lal::TSmallPathUtf8<LJafgChar, TCapacity>;

typedef Lal::TPathViewUtf8<LJafgChar> LPathView;
typedef Lal::TMutablePathViewUtf8<LJafgChar> LMutablePathView;

template <>
struct std::formatter<LPath> : std::formatter<std::string_view>
{
    FORCEINLINE auto format
    (
        const LPath& Path,
        std::format_context& InContext
    ) const -> std::format_context::iterator
    {
        return std::formatter<std::string_view>::format(std::string_view(Path.begin_ptr(), Path.end_ptr()), InContext);
    }
};

template <>
struct std::formatter<LBigPath> : std::formatter<std::string_view>
{
    FORCEINLINE auto format
    (
        const LBigPath& Path,
        std::format_context& InContext
    ) const -> std::format_context::iterator
    {
        return std::formatter<std::string_view>::format(std::string_view(Path.begin_ptr(), Path.end_ptr()), InContext);
    }
};

template <LSize TCapacity>
struct std::formatter<LSmallPath<TCapacity>> : std::formatter<std::string_view>
{
    FORCEINLINE auto format
    (
        const LSmallPath<TCapacity>& Path,
        std::format_context& InContext
    ) const -> std::format_context::iterator
    {
        return std::formatter<std::string_view>::format(std::string_view(Path.begin_ptr(), Path.end_ptr()), InContext);
    }
};

template <>
struct std::formatter<LPathView> : std::formatter<std::string_view>
{
    FORCEINLINE auto format
    (
        const LPathView& Path,
        std::format_context& InContext
    ) const -> std::format_context::iterator
    {
        return std::formatter<std::string_view>::format(std::string_view(Path.begin_ptr(), Path.end_ptr()), InContext);
    }
};

template <>
struct std::formatter<LMutablePathView> : std::formatter<std::string_view>
{
    FORCEINLINE auto format
    (
        const LMutablePathView& Path,
        std::format_context& InContext
    ) const -> std::format_context::iterator
    {
        return std::formatter<std::string_view>::format(std::string_view(Path.begin_ptr(), Path.end_ptr()), InContext);
    }
};
