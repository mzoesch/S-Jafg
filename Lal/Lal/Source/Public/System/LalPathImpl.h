// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TPathBase<TEncoding, TAllocator>::DoesExist() const noexcept
{
    return Finder::DoesExist(LPathView{this->begin(), this->end()});
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TPathBase<TEncoding, TAllocator>::IsFile() const noexcept
{
    return Finder::DoesFileExist(LPathView{this->begin(), this->end()});
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TPathBase<TEncoding, TAllocator>::IsDirectory() const noexcept
{
    return Finder::DoesDirectoryExist(LPathView{this->begin(), this->end()});
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TPathBase<TEncoding, TAllocator>::IsAbsolute() const noexcept
{
#if LAL_PLATFORM_USES_POSIX
    return this->IsRootDirectoryValid();
#else /* LAL_PLATFORM_USES_POSIX */
    return this->IsRootNameValid() && this->IsRootDirectoryValid();
#endif /* !LAL_PLATFORM_USES_POSIX */
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TPathBase<TEncoding, TAllocator>::MakeAbsolute() noexcept
    requires(Super::IsOwningString() && Super::IsContentMutable())
{
    if (this->IsAbsolute() == false)
    {
#if LAL_PLATFORM_USES_POSIX
        check( *this->ToPtr() != '/' )
        const LPath P { Finder::GetCwd() };
        this->ReserveAdditionally(1ul + P.GetRuneCount());
        this->AppendAt(0ul, '/');
        this->AppendAt(0ul , P);
#else /* LAL_PLATFORM_USES_POSIX */
        const std::filesystem::path Temp { std::filesystem::absolute(std::filesystem::path{this->begin().Data(), this->end().Data()}) };
        this->Assign(Temp.native().begin().base(), Temp.native().end().base());
#endif /* !LAL_PLATFORM_USES_POSIX */
    }

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseConcept TOther>
    requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase<TEncoding, TAllocator>, TOther>)
FORCEINLINE TOther TPathBase<TEncoding, TAllocator>::GetAbsolute() const noexcept
{
    TOther Out { *this };
    Out.MakeAbsolute();
    return Out;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TPathBase<TEncoding, TAllocator>::IsRelative() const noexcept
{
    return this->IsAbsolute() == false;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TPathBase<TEncoding, TAllocator>::IsRelativeValid() const noexcept
{
#if LAL_PLATFORM_USES_POSIX
    const SizeType RuneCount { this->GetRuneCount() };
    return (RuneCount == 1 && (*this->begin_ptr()) != '/') || RuneCount > 1;
#else /* LAL_PLATFORM_USES_POSIX */
    return std::filesystem::path{this->begin().Data(), this->end().Data()}.has_relative_path();
#endif /* LAL_PLATFORM_USES_POSIX */
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TPathBase<TEncoding, TAllocator>::MakeRelativePath() noexcept
{
    auto Ptr { this->begin_ptr() };

    if (Ptr == nullptr || Ptr == this->end_ptr())
    {
        return;
    }

#if LAL_PLATFORM_USES_POSIX
    if (*Ptr == '/')
    {
        this->Drop();
    }
#else /* LAL_PLATFORM_USES_POSIX */
    std::filesystem::path Temp {this->begin().Data(), this->end().Data()};
    Temp.assign(Temp.relative_path());
    this->Assign(Temp.native().begin().base(), Temp.native().end().base());
#endif /* LAL_PLATFORM_USES_POSIX */

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseConcept TOther>
    requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase<TEncoding, TAllocator>, TOther>)
FORCEINLINE TOther TPathBase<TEncoding, TAllocator>::GetRelativePath() const noexcept
{
    TOther Out{ *this };
    Out.MakeRelativePath();
    return Out;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
typename TPathBase<TEncoding, TAllocator>::ConstIterator TPathBase<TEncoding, TAllocator>::FindExtension() const noexcept
{
    const ConstIterator End { this->end() };

    ConstIterator Slash { this->FindLast('/') };
#if !LAL_PLATFORM_USES_POSIX
    if (Slash == End)
    {
        Slash = this->FindLast('\\');
    }
#endif /* !LAL_PLATFORM_USES_POSIX */

    const ConstIterator Dot { this->FindLast('.') };

    if (Dot == this->begin() || Dot == End)
    {
        return End;
    }

    if (Dot < Slash || Dot == Slash + 1)
    {
        return End;
    }

    return Dot;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TPathBase<TEncoding, TAllocator>::IsExtensionValid() const noexcept
{
    return this->FindExtension() != this->end();
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseConcept TOther>
    requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase<TEncoding, TAllocator>, TOther>)
FORCEINLINE TOther TPathBase<TEncoding, TAllocator>::GetExtension() const noexcept
{
    const ConstIterator End { this->end() };
    if (const ConstIterator It { this->FindExtension() }; It != End)
    {
        return TOther{ It, End };
    }

    return { };
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE TPathBase<TEncoding, TAllocator>& TPathBase<TEncoding, TAllocator>::RemoveExtension() & noexcept
{
    this->RemoveExtensionImpl();
    return *this;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE TPathBase<TEncoding, TAllocator>& TPathBase<TEncoding, TAllocator>::RemoveExtension() && noexcept
{
    this->RemoveExtensionImpl();
    return std::move(*this);
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE TPathBase<TEncoding, TAllocator>& TPathBase<TEncoding, TAllocator>::ReplaceExtension(const TPathBase& InExtension) noexcept
    requires(Super::IsContentMutable() && Allocator::IsAllowedToPushItems())
{
    const Iterator End { this->end() };

    const ConstIterator It { this->FindExtension() };
    if (It == End)
    {
        if ((*InExtension.begin_ptr()) != '.')
        {
            this->ReserveAdditionally(1ul + InExtension.GetRuneCount());
            this->Append('.');
        }

        this->Append(InExtension);
    }
    else
    {

        if ((*InExtension.begin_ptr()) == '.')
        {
            this->Substitute(this->begin() + (std::to_address(It) - this->begin_ptr()), End, InExtension.begin(), InExtension.end());
        }
        else
        {
            if (const Iterator First { this->begin() + (std::to_address(It) - this->begin_ptr()) + 1 }; this->IsValidIterator(First))
            {
                this->Substitute(First, End, InExtension.begin(), InExtension.end());
            }
            else
            {
                this->Append(InExtension);
            }
        }
    }

    return *this;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE typename TPathBase<TEncoding, TAllocator>::ConstIterator TPathBase<TEncoding, TAllocator>::FindFileName() const noexcept
{
    const ConstIterator End { this->end() };

    const ConstIterator Slash { this->FindLast('/') };
#if !LAL_PLATFORM_USES_POSIX
    if (Slash == End)
    {
        Slash = this->FindLast('\\');
    }
#endif /* !LAL_PLATFORM_USES_POSIX */

    if (Slash == End)
    {
        return this->begin();
    }

    return Slash + 1;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TPathBase<TEncoding, TAllocator>::IsFilenameValid() const noexcept
{
    return this->FindFileName() != this->end();
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseConcept TOther>
    requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase<TEncoding, TAllocator>, TOther>)
FORCEINLINE TOther TPathBase<TEncoding, TAllocator>::GetFilename() const noexcept
{
    const ConstIterator End { this->end() };
    if (const ConstIterator It { this->FindFileName() }; It != End)
    {
        return TOther{ It, End };
    }

    return { };
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE TPathBase<TEncoding, TAllocator>& TPathBase<TEncoding, TAllocator>::RemoveFilename() noexcept
{
    const ConstIterator It { this->FindFileName() };
    if (It != this->end())
    {
        this->InlineLeftChop(this->end() - It);
    }

    return *this;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE TPathBase<TEncoding, TAllocator>& TPathBase<TEncoding, TAllocator>::ReplaceFilename(const TPathBase& InExtension) noexcept
    requires(Super::IsContentMutable() && Allocator::IsAllowedToPushItems())
{
    const Iterator End { this->end() };

    const ConstIterator It { this->FindFileName() };
    if (It == End)
    {
        this->AppendPath(InExtension);
    }
    else
    {
        this->Substitute(this->begin() + (std::to_address(It) - this->begin_ptr()), End, InExtension.begin(), InExtension.end());
    }

    return *this;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE typename TPathBase<TEncoding, TAllocator>::ConstIterator TPathBase<TEncoding, TAllocator>::FindParentEnd() const noexcept
{
    const ConstIterator End { this->end() };

    const ConstIterator Slash { this->FindLast('/') };
#if !LAL_PLATFORM_USES_POSIX
    if (Slash == End)
    {
        Slash = this->FindLast('\\');
    }
#endif /* !LAL_PLATFORM_USES_POSIX */

    if (Slash != End)
    {
        if (Slash != this->begin())
        {
            return Slash;
        }
    }

    return End;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TPathBase<TEncoding, TAllocator>::IsParentValid() const noexcept
{
    return this->FindParentEnd() != this->end();
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseConcept TOther> requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase<TEncoding,
    TAllocator>, TOther>)
FORCEINLINE TOther TPathBase<TEncoding, TAllocator>::GetParent() const noexcept
{
    const ConstIterator End { this->end() };
    if (const ConstIterator It { this->FindParentEnd() }; It != End)
    {
        return TOther{ this->begin(), It };
    }

    return { };
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator> requires (
    TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
void TPathBase<TEncoding, TAllocator>::ToParent() noexcept requires (TPathBase::IsOwningString())
{
    if (const auto It { this->FindParentEnd() }; It != this->end())
    {
        this->InlineLeftCut(It);
        return;
    }

    this->Empty();

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TPathBase<TEncoding, TAllocator>::IsRootValid() const noexcept
{
#if LAL_PLATFORM_USES_POSIX
    return (*this->begin_ptr()) == '/';
#else /* LAL_PLATFORM_USES_POSIX */
    return std::filesystem::path{this->begin().Data(), this->end().Data() }.has_root_path();
#endif /* !LAL_PLATFORM_USES_POSIX */
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseConcept TOther>
    requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase<TEncoding, TAllocator>, TOther>)
FORCEINLINE TOther TPathBase<TEncoding, TAllocator>::GetRoot() const noexcept
{
#if LAL_PLATFORM_USES_POSIX
    return this->GetRootDirectory<TOther>();
#else /* LAL_PLATFORM_USES_POSIX */
    const std::filesystem::path D { std::filesystem::path{this->begin().Data(), this->end().Data() }.root_path() };
    return TOther{ D.native().begin().base(), D.native().end().base() };
#endif /* !LAL_PLATFORM_USES_POSIX */
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TPathBase<TEncoding, TAllocator>::IsRootNameValid() const noexcept
{
#if LAL_PLATFORM_USES_POSIX
    return false;
#else /* LAL_PLATFORM_USES_POSIX */
    return std::filesystem::path::has_root_name(std::filesystem::path{this->begin(), this->end()});
#endif /* !LAL_PLATFORM_USES_POSIX */
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseConcept TOther>
    requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase<TEncoding, TAllocator>, TOther>)
FORCEINLINE TOther TPathBase<TEncoding, TAllocator>::GetRootName() const noexcept
{
#if LAL_PLATFORM_USES_POSIX
    return TOther{ };
#else /* LAL_PLATFORM_USES_POSIX */
    return std::filesystem::path::root_name(std::filesystem::path{this->begin(), this->end()});
#endif /* !LAL_PLATFORM_USES_POSIX */
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TPathBase<TEncoding, TAllocator>::IsRootDirectoryValid() const noexcept
{
#if LAL_PLATFORM_USES_POSIX
    return (*this->begin_ptr()) == '/';
#else /* LAL_PLATFORM_USES_POSIX */
    return std::filesystem::path::has_root_directory(std::filesystem::path{this->begin(), this->end()});
#endif /* !LAL_PLATFORM_USES_POSIX */
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseConcept TOther>
    requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase<TEncoding, TAllocator>, TOther>)
FORCEINLINE TOther TPathBase<TEncoding, TAllocator>::GetRootDirectory() const noexcept
{
#if LAL_PLATFORM_USES_POSIX
    if (this->IsAbsolute())
    {
        return TOther{ '/' };
    }

    return TOther{ };
#else /* LAL_PLATFORM_USES_POSIX */
    const std::filesystem::path D { std::filesystem::path{this->begin().Data(), this->end().Data() }.root_directory() };
    return TOther{ D.native().begin().base(), D.native().end().base() };
#endif /* LAL_PLATFORM_USES_POSIX */
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TPathBase<TEncoding, TAllocator>::IsStemValid() const noexcept
{
    const ConstIterator End { this->end() };

    const ConstIterator It { this->FindExtension() };
    if (It != this->end())
    {
        return true;
    }

    const ConstIterator Slash { this->FindLast('/') };
#if !LAL_PLATFORM_USES_POSIX
    if (Slash == End)
    {
        Slash = this->FindLast('\\');
    }
#endif /* !LAL_PLATFORM_USES_POSIX */

    if (Slash == End)
    {
        return this->GetRuneCount() > 0;
    }

    if (Slash + 1 == End)
    {
        return false;
    }

    return true;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseConcept TOther>
    requires(TOther::IsOwningString() && TValidOtherStringConcept<TPathBase<TEncoding, TAllocator>, TOther>)
FORCEINLINE TOther TPathBase<TEncoding, TAllocator>::GetStem() const noexcept
{
    const ConstIterator End { this->end() };

    const ConstIterator It { this->FindLast('/') };
#if !LAL_PLATFORM_USES_POSIX
    if (It == End)
    {
        It = this->FindLast('\\');
    }
#endif /* !LAL_PLATFORM_USES_POSIX */

    const ConstIterator Extension { this->FindExtension() };
    if (It == End)
    {
        check( this->begin() <= Extension )
        return TOther{ this->begin(), Extension };
    }

    check( It + 1 <= End )
    return TOther{ It + 1, Extension };
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TPathBase<TEncoding, TAllocator>::RemoveExtensionImpl() noexcept
{
    const ConstIterator It { this->FindExtension() };
    const ConstIterator End { this->end() };
    if (It != End)
    {
        this->InlineLeftChop(End - It);
    }

    return;
}

} /* ~Namespace Lal */
