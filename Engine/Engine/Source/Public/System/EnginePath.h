// Copyright mzoesch. All rights reserved.

#pragma once

#include "System/SystemForward.h"

namespace Jafg
{

class JUserPreferences;

//#
//# A path that is always relative to a content folder. The content folder that is used is then determined when
//# resolving the absolute path. This allows the user to add their own plugin content folders that have a higher
//# priority than the engine content folder. Absolute paths will then be redirected to the plugin content folder.
//#
template <template <typename, typename> typename TEncoding, Lal::TStringBaseAllocatorConcept TAllocator>
    requires(Lal::TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
class TEnginePathBase : public Lal::TPathBase<TEncoding, TAllocator>
{
public:

    typedef Lal::TPathBase<TEncoding, TAllocator> Super;

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

    using Super::TPathBase;

    static_assert(Super::IsOwningString());

    template <Lal::TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE TEnginePathBase(const EEnginePaths::Type Type, const Lal::TPathBase<TEncoding, UAllocator>& Other) noexcept
        : Super(LexToString(Type) / Other) { }
    FORCEINLINE TEnginePathBase(const EEnginePaths::Type Type, const Pointer String) noexcept
        : Super(LexToString(Type) / String) { }
    FORCEINLINE TEnginePathBase(const EEnginePaths::Type Type, const ConstPointer String) noexcept
        requires(std::is_same_v<Pointer, ConstPointer> == false)
        : Super(LexToString(Type) / String) { }
    FORCEINLINE TEnginePathBase(const EEnginePaths::Type Type) noexcept
        : Super(LexToString(Type)) { }

    FORCEINLINE void  ResolvePathInline() noexcept;
    FORCEINLINE void  ResolvePathToAbsoluteInline() noexcept;
    FORCEINLINE LPath ResolvePath() const noexcept;
    FORCEINLINE LPath ResolvePathToAbsolute() const noexcept;
};

template <template <typename, typename> class TEncoding, Lal::TStringBaseAllocatorConcept TAllocator> requires (Lal::
    TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TEnginePathBase<TEncoding, TAllocator>::ResolvePathInline() noexcept
{
    this->Assign(this->ResolvePath());
    return;
}

template <template <typename, typename> class TEncoding, Lal::TStringBaseAllocatorConcept TAllocator> requires (Lal::
    TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TEnginePathBase<TEncoding, TAllocator>::ResolvePathToAbsoluteInline() noexcept
{
    this->Assign(this->ResolvePathToAbsolute());
    return;
}

template <template <typename, typename> class TEncoding, Lal::TStringBaseAllocatorConcept TAllocator> requires (Lal::
    TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE LPath TEnginePathBase<TEncoding, TAllocator>::ResolvePath() const noexcept
{
    //#
    //# Currently there is no user preference to override the engine content folder and add priorities.
    //# But this logic will be added here.
    //#
    return LPath{this->begin(), this->end()};
}

template <template <typename, typename> class TEncoding, Lal::TStringBaseAllocatorConcept TAllocator> requires (Lal::
    TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE LPath TEnginePathBase<TEncoding, TAllocator>::ResolvePathToAbsolute() const noexcept
{
    return this->GetAbsolute();
}

} /* ~Namespace Jafg */

template <>
struct std::formatter<Jafg::LEnginePath> : std::formatter<std::string_view>
{
    FORCEINLINE auto format
    (
        const Jafg::LEnginePath& Path,
        std::format_context& InContext
    ) const -> std::format_context::iterator
    {
        return std::formatter<std::string_view>::format(std::string_view{Path.begin_ptr(), Path.end_ptr()}, InContext);
    }
};
