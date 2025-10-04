// Copyright mzoesch. All rights reserved.

#pragma once

#if LAL_DO_CHECKS
    #define PRIVATE_LAL_CHECK_STRING_VALIDITY()                         this->_IsValidString();
    #define PRIVATE_LAL_STRING_CHECK(Predicate, ...)                    this->_Check(Predicate, ##__VA_ARGS__);
#else /* LAL_DO_CHECKS */
    #define PRIVATE_LAL_CHECK_STRING_VALIDITY()
    #define PRIVATE_LAL_STRING_CHECK(Predicate)
#endif /* !LAL_DO_CHECKS */

namespace Lal
{

template<typename TChar, typename TTraits, typename TAlloc>
class TBasicHeapString
{
public:

    static_assert(std::is_trivially_copyable_v<TChar>);
    static_assert(std::is_trivially_default_constructible_v<TChar>);
    static_assert(std::is_standard_layout_v<TChar>);

    static_assert(std::is_same_v<TChar, typename TTraits::char_type>);
    static_assert(std::is_same_v<TChar, typename TAlloc::value_type>);

#if LAL_WITH_CLANG
    typedef __gnu_cxx::__alloc_traits<TAlloc>       AllocTraits;
#else /* LAL_WITH_CLANG */
    #error "Missing implementation for compiler."
#endif /* !LAL_WITH_CLANG */

    typedef TTraits                                 traits_type;
    typedef typename traits_type::char_type         value_type;
    typedef TAlloc                                  allocator_type;
    typedef typename AllocTraits::size_type         size_type;
    typedef typename AllocTraits::difference_type   difference_type;
    typedef typename AllocTraits::reference         reference;
    typedef typename AllocTraits::const_reference   const_reference;
    typedef typename AllocTraits::pointer           pointer;
    typedef typename AllocTraits::const_pointer     const_pointer;
    typedef pointer                                 iterator;
    typedef const_pointer                           const_iterator;
    typedef std::reverse_iterator<iterator>         reverse_iterator;
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

    static const size_type npos = static_cast<size_type>(-1);

    typedef std::basic_string_view<value_type, traits_type> LStringViewType;

    FORCEINLINE constexpr TBasicHeapString() noexcept = default;

    FORCEINLINE constexpr TBasicHeapString(LNullptrTy) noexcept
        : Alloc{}
    {
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

    FORCEINLINE constexpr explicit TBasicHeapString(allocator_type const& Alloc) noexcept
        : Alloc{Alloc}
    {
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

    FORCEINLINE constexpr TBasicHeapString(TBasicHeapString const& Other) noexcept
        : Alloc{Other.Alloc}
    {
        return;
    }

    FORCEINLINE constexpr TBasicHeapString(TBasicHeapString const& Other, size_type Pos)
        : Alloc{}
    {
        this->Construct(std::data(Other) + Pos, std::end(Other));
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

    FORCEINLINE constexpr TBasicHeapString(TBasicHeapString const& Other, size_type Pos, size_type N)
        : Alloc{}
    {
        PRIVATE_LAL_STRING_CHECK([this, &Other, Pos](void){ return Pos <= Other.size(); })
        this->Construct(std::data(Other) + Pos, std::data(Other) + Pos + this->GetLimit(Pos, N));
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

    FORCEINLINE constexpr TBasicHeapString(value_type const* Str) noexcept
        : Alloc{}
    {
        this->Construct(Str, Str + traits_type::length(Str));
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

    FORCEINLINE constexpr TBasicHeapString(value_type const* Str, size_type N) noexcept
        : Alloc{}
    {
        PRIVATE_LAL_STRING_CHECK([Str, N](void){ return traits_type::length(Str) >= N; })
        this->Construct(Str, Str + N);
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

    FORCEINLINE constexpr TBasicHeapString(size_type N, value_type Char) noexcept
        : Alloc{}
    {
        if (N)
        {
            this->reserve(N);
            if (N == 1)
            {
                traits_type::assign(*std::data(this->Alloc), Char);
            }
            else
            {
                traits_type::assign(std::data(this->Alloc), N, Char);
            }
        }

        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

    template <ContainerCompatibleRange<value_type> TRange>
    FORCEINLINE constexpr TBasicHeapString(std::from_range_t, TRange&& Range) noexcept
        : Alloc{std::begin(Range), std::end(Range)}
    {
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

    FORCEINLINE constexpr TBasicHeapString(std::initializer_list<value_type> List) noexcept
        : Alloc{List}
    {
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

    template <typename TIterator, typename UIterator>
    FORCEINLINE constexpr TBasicHeapString(TIterator Begin, UIterator End)
        : Alloc{Begin, End}
    {
        this->Construct(Begin, End);
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

    FORCEINLINE constexpr TBasicHeapString(TBasicHeapString&& Other) noexcept
        : Alloc{std::move(Other.Alloc)}
    {
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

    template <typename LSv> requires std::is_convertible_v<LSv, LStringViewType>
    FORCEINLINE constexpr explicit TBasicHeapString(LSv const& Sv) noexcept
        : Alloc{std::begin(Sv), std::end(Sv)}
    {
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

    template <typename LSv> requires std::is_convertible_v<LSv, LStringViewType>
    FORCEINLINE constexpr TBasicHeapString(LSv const& Sv, size_type Pos, size_type N) noexcept
        : Alloc{}
    {
        this->Construct(Sv + Pos, Sv + Pos + std::min(N, Sv.size() - Pos));
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

    FORCEINLINE constexpr ~TBasicHeapString() noexcept = default;

    FORCEINLINE constexpr TBasicHeapString& operator=(TBasicHeapString const& Other) noexcept
    {
        PRIVATE_LAL_STRING_CHECK([this, &Other](void){ return std::addressof(Other) != this; })
        this->Alloc = Other.Alloc;
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return *this;
    }

    FORCEINLINE constexpr TBasicHeapString& operator=(TBasicHeapString&& Other) noexcept
    {
        PRIVATE_LAL_STRING_CHECK([this, &Other](void){ return std::addressof(Other) != this; })
        this->Alloc = std::move(Other.Alloc);
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return *this;
    }

    FORCEINLINE constexpr TBasicHeapString& operator=(value_type const* Str) noexcept
    {
        this->Construct(Str, Str + traits_type::length(Str));
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return *this;
    }

    FORCEINLINE constexpr TBasicHeapString& operator=(value_type Char) noexcept
    {
        this->Construct(&Char, &Char + 1);
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return *this;
    }

    FORCEINLINE constexpr TBasicHeapString& operator=(std::initializer_list<value_type> List) noexcept
    {
        this->Construct(List.begin(), List.end());
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return *this;
    }

    FORCEINLINE constexpr operator LStringViewType() const noexcept
    {
        return LStringViewType{std::data(this->Alloc), std::size(this->Alloc)};
    }

    FORCEINLINE constexpr TBasicHeapString& operator+=(TBasicHeapString const& Other) noexcept
    {
        this->append(Other);
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return *this;
    }
    FORCEINLINE constexpr TBasicHeapString& operator+=(value_type const* Str) noexcept
    {
        this->append(Str);
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return *this;
    }
    FORCEINLINE constexpr TBasicHeapString& operator+=(value_type Char) noexcept
    {
        this->push_back(Char);
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return *this;
    }
    FORCEINLINE constexpr TBasicHeapString& operator+=(std::initializer_list<value_type> List) noexcept
    {
        this->append(List.begin(), List.end());
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return *this;
    }
    template <typename LSv> requires std::is_convertible_v<LSv, LStringViewType>
    FORCEINLINE constexpr TBasicHeapString& operator+=(LSv const& Sv) noexcept
    {
        this->append(Sv.begin(), Sv.end());
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return *this;
    }

    NODISCARD FORCEINLINE constexpr auto begin() noexcept { return std::data(this->Alloc); }
    NODISCARD FORCEINLINE constexpr auto begin() const noexcept { return std::data(this->Alloc); }
    NODISCARD FORCEINLINE constexpr auto cbegin() const noexcept { return std::data(this->Alloc); }
    NODISCARD FORCEINLINE constexpr auto end() noexcept { return std::begin(this->Alloc) + this->size(); }
    NODISCARD FORCEINLINE constexpr auto end() const noexcept { return std::begin(this->Alloc) + this->size(); }
    NODISCARD FORCEINLINE constexpr auto cend() const noexcept { return std::cbegin(this->Alloc) + this->size(); }
    NODISCARD FORCEINLINE constexpr auto rbegin() noexcept { return reverse_iterator(this->end()); }
    NODISCARD FORCEINLINE constexpr auto rbegin() const noexcept { return const_reverse_iterator(this->end()); }
    NODISCARD FORCEINLINE constexpr auto crbegin() const noexcept { return const_reverse_iterator(this->end()); }
    NODISCARD FORCEINLINE constexpr auto rend() noexcept { return reverse_iterator(this->begin()); }
    NODISCARD FORCEINLINE constexpr auto rend() const noexcept { return const_reverse_iterator(this->begin()); }
    NODISCARD FORCEINLINE constexpr auto crend() const noexcept { return const_reverse_iterator(this->begin()); }

    NODISCARD FORCEINLINE constexpr size_type capacity() const noexcept { return this->Alloc.capacity(); }
    NODISCARD FORCEINLINE constexpr size_type size() const noexcept { if (this->Alloc.size() > 0) return this->Alloc.size() - 1; return 0; }
    NODISCARD FORCEINLINE constexpr size_type length() const noexcept { return this->size(); }
    NODISCARD FORCEINLINE constexpr size_type max_size() const noexcept
    {
#if LAL_WITH_CLANG
        const size_t DiffMax = __gnu_cxx::__numeric_traits<ptrdiff_t>::__max / sizeof(value_type);
        const size_t AllocMax = AllocTraits::max_size(this->Alloc);
        return (std::min)(DiffMax, AllocMax) - 1;
#else /* LAL_WITH_CLANG */
        #error "Missing implementation for compiler."
#endif  /* !LAL_WITH_CLANG */
    }

    FORCEINLINE constexpr void resize(size_type NewSize, value_type Char = value_type{}) noexcept;

    FORCEINLINE constexpr TBasicHeapString reserve(size_type N) noexcept
    {
        this->Alloc.reserve(N + 1);
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return *this;
    }

    FORCEINLINE constexpr void shrink_to_fit() noexcept
    {
        this->Alloc.shrink_to_fit();
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

    template <typename TOperation>
    FORCEINLINE constexpr void resize_and_overwrite(size_type N, TOperation Operation) noexcept;

    NODISCARD FORCEINLINE constexpr bool empty() const noexcept { return this->size() == 0; }
    NODISCARD FORCEINLINE constexpr void clear() noexcept
    {
        this->Alloc.clear();
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

    NODISCARD FORCEINLINE constexpr reference operator[](size_type Index) noexcept
    {
        PRIVATE_LAL_STRING_CHECK([this, Index](void){ return Index < this->size(); })
        return std::data(this->Alloc)[Index];
    }
    NODISCARD FORCEINLINE constexpr const_reference operator[](size_type Index) const noexcept
    {
        PRIVATE_LAL_STRING_CHECK([this, Index](void){ return Index < this->size(); })
        return std::data(this->Alloc)[Index];
    }

    NODISCARD FORCEINLINE constexpr reference front() noexcept
    {
        PRIVATE_LAL_STRING_CHECK([this](void){ return !this->empty(); })
        return this->operator[](0);
    }
    NODISCARD FORCEINLINE constexpr const_reference front() const noexcept
    {
        PRIVATE_LAL_STRING_CHECK([this](void){ return !this->empty(); })
        return this->operator[](0);
    }
    NODISCARD FORCEINLINE constexpr reference back() noexcept
    {
        PRIVATE_LAL_STRING_CHECK([this](void){ return !this->empty(); })
        return this->operator[](this->size() - 1);
    }
    NODISCARD FORCEINLINE constexpr const_reference back() const noexcept
    {
        PRIVATE_LAL_STRING_CHECK([this](void){ return !this->empty(); })
        return this->operator[](this->size() - 1);
    }

    FORCEINLINE constexpr TBasicHeapString& append(TBasicHeapString const& Other) noexcept
    {
        return this->append(Other.begin(), Other.end());
    }
    FORCEINLINE constexpr TBasicHeapString& append(TBasicHeapString const& Other, size_type Pos, size_type N = TBasicHeapString::npos) noexcept
    {
        PRIVATE_LAL_STRING_CHECK([this, &Other, Pos](void){ return Pos <= Other.size(); })
        return this->append(Other.begin() + Pos, Other.begin() + Pos + this->GetLimit(Pos, N));
    }
    FORCEINLINE constexpr TBasicHeapString& append(value_type const* Str) noexcept
    {
        return this->append(Str, Str + traits_type::length(Str));
    }
    FORCEINLINE constexpr TBasicHeapString& append(value_type const* Str, size_type N) noexcept
    {
        PRIVATE_LAL_STRING_CHECK([Str, N](void){ return traits_type::length(Str) >= N; })
        return this->append(Str, Str + N);
    }
    FORCEINLINE constexpr TBasicHeapString& append(size_type N, value_type Char) noexcept;
    FORCEINLINE constexpr TBasicHeapString& append(std::initializer_list<value_type> List) noexcept
    {
        return this->append(List.begin(), List.end());
    }
    template <typename TIterator, typename UIterator>
    FORCEINLINE constexpr TBasicHeapString& append(TIterator Begin, UIterator End) noexcept;

    template <typename LSv> requires std::is_convertible_v<LSv, LStringViewType>
    FORCEINLINE constexpr TBasicHeapString& append(LSv const& Sv) noexcept
    {
        return this->append(Sv.data(), Sv.size());
    }
    template <typename LSv> requires std::is_convertible_v<LSv, LStringViewType>
    FORCEINLINE constexpr TBasicHeapString& append(LSv const& Sv, size_type Pos, size_type N = TBasicHeapString::npos) noexcept
    {
        PRIVATE_LAL_STRING_CHECK([&Sv, Pos](void){ return Pos <= Sv.size(); })
        return this->append(Sv.data() + Pos, Sv.data() + Pos + this->GetLimit(Pos, N));
    }

    template <ContainerCompatibleRange<value_type> TRange>
    FORCEINLINE constexpr TBasicHeapString& append_range(TRange&& Range) noexcept;

    FORCEINLINE constexpr void push_back(value_type Char) noexcept
    {
        this->append(&Char, &Char + 1);
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

    FORCEINLINE constexpr TBasicHeapString& assign(TBasicHeapString const& Other) noexcept
    {
        return this->assign(Other.begin(), Other.end());
    }
    FORCEINLINE constexpr TBasicHeapString& assign(TBasicHeapString&& Other) noexcept
    {
        this->Alloc.assign(std::move(Other.Alloc));
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return *this;
    }
    FORCEINLINE constexpr TBasicHeapString& assign(TBasicHeapString const& Other, size_type Pos, size_type N = TBasicHeapString::npos) noexcept
    {
        PRIVATE_LAL_STRING_CHECK([this, &Other, Pos](void){ return Pos <= Other.size(); })
        return this->assign(Other.begin() + Pos, Other.begin() + Pos + this->GetLimit(Pos, N));
    }
    FORCEINLINE constexpr TBasicHeapString& assign(value_type const* Str) noexcept
    {
        return this->assign(Str, Str + traits_type::length(Str));
    }
    FORCEINLINE constexpr TBasicHeapString& assign(value_type const* Str, size_type N) noexcept
    {
        PRIVATE_LAL_STRING_CHECK([Str, N](void){ return traits_type::length(Str) >= N; })
        return this->assign(Str, Str + N);
    }

    FORCEINLINE constexpr TBasicHeapString& assign(ITERATOR Begin, ITERATOR End) noexcept;

    FORCEINLINE constexpr TBasicHeapString& assign(size_type N, value_type Char) noexcept;

protected:

    template <typename TIterator, typename UIterator>
    FORCEINLINE constexpr void Construct(TIterator Begin, UIterator End)
    {
        this->Alloc.assign(Begin, End);
        PRIVATE_LAL_CHECK_STRING_VALIDITY()
        return;
    }

#if LAL_DO_CHECKS
    FORCEINLINE void _IsValidString();
    template <typename TPredicate>
    FORCEINLINE void _Check(TPredicate&& Predicate);
#endif /* LAL_DO_CHECKS */

    NODISCARD FORCEINLINE constexpr size_type GetLimit(size_type Pos, size_type Off) const noexcept
    {
        if (Off < this->size() - Pos)
        {
            return Off;
        }

        return  this->size() - Pos;
    }

    allocator_type Alloc;
};


} /* ~Namespace Lal */

#undef PRIVATE_LAL_CHECK_STRING_VALIDITY
#undef PRIVATE_LAL_STRING_CHECK
