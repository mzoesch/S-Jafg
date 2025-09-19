// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Containers/JafgArray.h"

namespace Jafg
{

template <typename InT>
struct TStringTraits;
template <typename Derived, typename InTraits, typename InAlloc>
class TStringBase;
template <typename InTraits, typename InAlloc>
class _TStringBase;

template <typename InT>
using TStringBasic = _TStringBase<TStringTraits<InT>, TArrayOldv2<InT>>;
template <typename InT>
using TStringViewBasic = _TStringBase<TStringTraits<InT>, TArrayViewOldv2<InT>>;
template <typename InT>
using TMutableStringViewBasic = _TStringBase<TStringTraits<InT>, TMutableArrayViewOldv2<InT>>;

//#
//# The default string for jafg.
//# This string is utf8-encoded and dynamic.
//#
typedef TStringBasic<char> LString;
//#
//# The default string view for jafg.
//# This string view expects an utf8-encoded string.
//#
typedef TStringViewBasic<char>        LStringView;
typedef TMutableStringViewBasic<char> LMutableStringView;

} /* ~Namespace Jafg */

#if LAL_CHECK_STRING_VALIDITY
    #define PRIVATE_JAFG_CHECK_STRING_STATE()   this->EnsureValidState();
    #define JAFG_CHECK_STRING(Expr)             jassert(Expr)
#else /* LAL_CHECK_STRING_VALIDITY */
    #define PRIVATE_JAFG_CHECK_STRING_STATE()
    #define JAFG_CHECK_STRING(Expr)
#endif /* !LAL_CHECK_STRING_VALIDITY */

template <typename TOtherStringCandidate>
concept Jafg_HasTAndTraits = requires(TOtherStringCandidate OtherString)
{
    typename TOtherStringCandidate::T;
    typename TOtherStringCandidate::Traits;
};

namespace Jafg
{

//# Common traits. Define your own if you need to. These are the most basic UTF-8 traits.
template <typename InT>
struct TStringTraits
{
    using T    = InT;
    using Self = TStringTraits<T>;

    inline static const T Terminator = '\0';

    template <typename SizeType>
    FORCEINLINE static SizeType GetStringLength(const T* InString) noexcept; // In Runes.

    template <typename SizeType>
    static SizeType GetCharacterCount(const T* InString) noexcept; // In Characters. But just basic UFT-8 stuff; So no compound shit.

    template <typename SizeType>
    static SizeType GetCharacterCount(const T* InString, const T* InEnd) noexcept;

    template <typename SizeType>
    static SizeType GetCharacterSize(const T* InCharacter) noexcept;

    template <typename SizeType>
    static void GoToMostRecentValidCharacter(const T* Self, SizeType* InOutCursor) noexcept;

    FORCEINLINE static T ToLower(const T InRune);
};

//#
//# !!! @Deprecated Use LalString.h instead. !!!
//#
//# The base class for all strings.
//#
template <typename Derived, typename InTraits, typename InAlloc>
class TStringBase
{
public:

    using Self       = Derived;
    using Traits     = InTraits;
    using Alloc      = InAlloc;
    using T          = typename Traits::T;
    using SizeType   = typename Alloc::SizeType;

    static_assert(std::is_integral_v<SizeType>, "SizeType must be an integral type.");
    static_assert(std::is_signed_v<SizeType>, "SizeType must be a signed integral type.");

    template <typename TMemberField>
    friend void OnDefaultOnlyMallocMember(TMemberField* MemberField);
    template <typename TMemberField>
    friend void OnDefaultOnlyMallocMember(TArray<TMemberField>* MemberField);

    //# @return True, if the TOtherString a valid string in terms of its rune type and traits.
    template <typename TOtherString>
    FORCEINLINE consteval static bool IsValidOtherString() noexcept requires (Jafg_HasTAndTraits<TOtherString> == false)
    {
        return false; // Default no. We might wanna do something weaker here in the future but this works for now.
                      // But only for Jafg::TStringBase... So more like a future problem.
    }
    template <typename TOtherString>
    FORCEINLINE consteval static bool IsValidOtherString() noexcept requires (Jafg_HasTAndTraits<TOtherString>)
    {
        return std::is_same_v<Self, TOtherString> == false
            && std::is_same_v<typename TOtherString::T, T>
            && std::is_same_v<typename TOtherString::Traits, Traits>;
    }

    //# Default implementation but may be shadowed by a derived class.
    FORCEINLINE consteval static bool IsDynamic()        noexcept { return Alloc::IsDynamic();     }
    FORCEINLINE consteval static bool IsWeakAlloc()      noexcept { return Alloc::IsWeakAlloc();   }
    FORCEINLINE consteval static bool IsStrongAlloc()    noexcept { return Alloc::IsStrongAlloc(); }
    FORCEINLINE consteval static bool IsContentConst()   noexcept { return Alloc::IsContentConst(); }
    FORCEINLINE consteval static bool IsContentMutable() noexcept { return Alloc::IsContentMutable(); }

    static_assert(TStringBase::IsWeakAlloc()    ? TStringBase::IsDynamic()      == false : true, "Weak allocators must not be dynamic.");
    static_assert(TStringBase::IsDynamic()      ? TStringBase::IsWeakAlloc()    == false : true, "Dynamic allocators must not be weak.");
    static_assert(TStringBase::IsDynamic()      ? TStringBase::IsContentConst() == false : true, "Dynamic allocators must not be content const.");
    static_assert(TStringBase::IsContentConst() ? TStringBase::IsDynamic()      == false : true, "Const content allocators must not be dynamic.");

    FORCEINLINE  TStringBase() noexcept = default;
    FORCEINLINE  TStringBase(LNullptrTy) noexcept : Impl() { }
    FORCEINLINE  TStringBase(const TStringBase& Other) noexcept = default;
    FORCEINLINE  TStringBase(TStringBase&& Other) noexcept = default;
    explicit     TStringBase(const T InRune) noexcept requires (Self::IsStrongAlloc());
    explicit     TStringBase(const T& InRune) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst());
    explicit     TStringBase(      T& InRune) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable());
    FORCEINLINE  TStringBase(const T* InString) noexcept requires (Self::IsStrongAlloc());
    FORCEINLINE  TStringBase(const T* InString) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst());
    FORCEINLINE  TStringBase(      T* InString) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable());
    FORCEINLINE  TStringBase(const T* InString, const SizeType InLength) noexcept requires (Self::IsStrongAlloc());
    FORCEINLINE  TStringBase(const T* InString, const SizeType InLength) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst());
    FORCEINLINE  TStringBase(      T* InString, const SizeType InLength) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable());
    FORCEINLINE  TStringBase(const T* InString, const T* InEnd) noexcept requires (Self::IsStrongAlloc());
    FORCEINLINE  TStringBase(const T* InString, const T* InEnd) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst());
    FORCEINLINE  TStringBase(      T* InString,       T* InEnd) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable());
    FORCEINLINE ~TStringBase() noexcept = default;
    template <typename TOtherString>
    FORCEINLINE  TStringBase(const TOtherString& Other) noexcept requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>());
    template <typename TOtherString>
    FORCEINLINE  TStringBase(const TOtherString& Other) noexcept requires (Self::IsStrongAlloc() && Self::template IsValidOtherString<TOtherString>());
    template <typename TOtherString>
    FORCEINLINE  TStringBase(TOtherString&& Other) noexcept requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) = delete;
    template <typename TOtherString>
    FORCEINLINE  TStringBase(const TOtherString&& Other) noexcept requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) = delete;
    template <typename TOtherString>
    FORCEINLINE  TStringBase(TOtherString&& InString) noexcept requires (Self::IsWeakAlloc() && TOtherString::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>());
    template <typename TOtherString>
    FORCEINLINE  TStringBase(TOtherString&& InString) noexcept requires (Self::IsStrongAlloc() && TOtherString::IsStrongAlloc() && Self::template IsValidOtherString<TOtherString>());

    FORCEINLINE Self& operator=(LNullptrTy) noexcept requires (Self::IsStrongAlloc()) { this->Empty(); return this->GetSelf(); }
    FORCEINLINE Self& operator=(LNullptrTy) noexcept requires (Self::IsWeakAlloc()) { this->GetUnderlyingDataStructure().Invalidate(); return this->GetSelf(); }
    FORCEINLINE Self& operator=(const Self& InOther) noexcept { this->Impl = InOther.Impl; return this->GetSelf(); }
    FORCEINLINE Self& operator=(Self&& InOther) noexcept { this->Impl = std::move(InOther.Impl); return this->GetSelf(); }
    FORCEINLINE Self& operator=(const T InRune) noexcept requires (Self::IsStrongAlloc());
    FORCEINLINE Self& operator=(const T& InRune) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst());
    FORCEINLINE Self& operator=(      T& InRune) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable());
    FORCEINLINE Self& operator=(const T* InString) noexcept requires (Self::IsStrongAlloc());
    FORCEINLINE Self& operator=(const T* InString) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst());
    FORCEINLINE Self& operator=(      T* InString) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable());
    template <typename TOtherString>
    FORCEINLINE Self& operator=(const TOtherString& Other) noexcept requires (Self::IsWeakAlloc() && std::is_same_v<Self, TOtherString> == false);
    template <typename TOtherString>
    FORCEINLINE Self& operator=(const TOtherString& Other) noexcept requires (Self::IsStriongAlloc() && Self::template IsValidOtherString<TOtherString>());
    template <typename TOtherString>
    FORCEINLINE Self& operator=(TOtherString&& Other) noexcept requires (Self::IsWeakAlloc() && std::is_same_v<Self, TOtherString> == false) = delete;
    template <typename TOtherString>
    FORCEINLINE Self& operator=(const TOtherString&& Other) noexcept requires (Self::IsWeakAlloc() && std::is_same_v<Self, TOtherString> == false) = delete;
    template <typename TOtherString>
    FORCEINLINE Self& operator=(TOtherString&& InString) noexcept requires (Self::IsWeakAlloc() && TOtherString::IsWeakAlloc() && std::is_same_v<TOtherString, Self> == false);
    template <typename TOtherString>
    FORCEINLINE Self& operator=(TOtherString&& InString) noexcept requires (Self::IsStrongAlloc() && TOtherString::IsStrongAlloc() && std::is_same_v<TOtherString, Self> == false);

    FORCEINLINE void     Invalidate() requires (Self::IsWeakAlloc()) { this->Impl.Invalidate(); }
    FORCEINLINE T*       MoveDataPointerUp(T* NewDataPointer) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable()) { return this->Impl.MoveDataPointerUp(NewDataPointer); }
    FORCEINLINE const T* MoveDataPointerUp(const T* NewDataPointer) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst()) { return this->Impl.MoveDataPointerUp(NewDataPointer); }
    FORCEINLINE bool     MoveDataPointerUp() noexcept requires (Self::IsWeakAlloc()) { return this->Impl.MoveDataPointerUp(); }
    FORCEINLINE SizeType MoveDataPointerUp(const SizeType InOffset) noexcept requires (Self::IsWeakAlloc()) { return this->Impl.MoveDataPointerUp(InOffset); }
    FORCEINLINE T*       MoveSlackPointerDown(T* NewSlackPointer) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable()) { return this->Impl.MoveSlackPointerDown(NewSlackPointer); }
    FORCEINLINE const T* MoveSlackPointerDown(const T* NewSlackPointer) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst()) { return this->Impl.MoveSlackPointerDown(NewSlackPointer); }
    FORCEINLINE bool     MoveSlackPointerDown() noexcept requires (Self::IsWeakAlloc()) { return this->Impl.MoveSlackPointerDown(); }
    FORCEINLINE SizeType MoveSlackPointerDown(const SizeType InOffset) noexcept requires (Self::IsWeakAlloc()) { return this->Impl.MoveSlackPointerDown(InOffset); }

    NODISCARD FORCEINLINE       Self& GetSelf()       noexcept { return *static_cast<Self*>(this); }
    NODISCARD FORCEINLINE const Self& GetSelf() const noexcept { return *static_cast<const Self*>(this); }

    NODISCARD FORCEINLINE const T* ToPtr() const noexcept requires (Self::IsStrongAlloc()) { const T* Out = this->Impl.GetFirst(); return Out ? Out : &Traits::Terminator; }
    NODISCARD FORCEINLINE SizeType GetRuneCount() const noexcept requires (Self::IsStrongAlloc()) { return Maths::Max(this->GetSize() - /*Terminator*/1, 0); }
    NODISCARD FORCEINLINE SizeType GetRuneCount() const noexcept requires (Self::IsWeakAlloc()) { return this->GetSize(); }
    NODISCARD FORCEINLINE SizeType GetCharacterCount() const noexcept requires (Self::IsStrongAlloc()) { return Traits::template GetCharacterCount<SizeType>(this->ToPtr()); }
    NODISCARD FORCEINLINE SizeType GetCharacterCount() const noexcept requires (Self::IsWeakAlloc()) { return Traits::template GetCharacterCount<SizeType>(this->GetBegin(), this->GetEnd()); }
    NODISCARD FORCEINLINE SizeType GetRuneCountOfCharacterAt(const SizeType InRuneIndex) const noexcept;

    NODISCARD FORCEINLINE SizeType GetSize()     const noexcept { return this->Impl.GetSize();      }
    NODISCARD FORCEINLINE SizeType GetByteSize() const noexcept { return this->Impl.GetByteSize();  }
    NODISCARD FORCEINLINE bool     IsEmpty()     const noexcept { return this->GetRuneCount() == 0; }

    FORCEINLINE bool IsValidIndex(const SizeType InRuneIndex) const noexcept { return InRuneIndex > INDEX_NONE && InRuneIndex < this->GetRuneCount(); }

    FORCEINLINE       T& operator[](const SizeType InRuneIndex)       noexcept requires (Self::IsStrongAlloc() && Self::IsContentMutable());
    FORCEINLINE const T& operator[](const SizeType InRuneIndex) const noexcept requires (Self::IsStrongAlloc());
    FORCEINLINE       T& operator[](const SizeType InRuneIndex)       noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable());
    FORCEINLINE const T& operator[](const SizeType InRuneIndex) const noexcept requires (Self::IsWeakAlloc());

    FORCEINLINE void Reserve(const SizeType InRuneCount) noexcept requires (Self::IsDynamic()) { this->Impl.Reserve(InRuneCount + /*Terminator*/1); }
    FORCEINLINE void Reset(const SizeType InRuneCount) noexcept requires (Self::IsDynamic()) { this->Impl.Reset(InRuneCount + /*Terminator*/1); }
    FORCEINLINE void Shrink() noexcept requires (Self::IsDynamic()) { this->Impl.Shrink(); }
    FORCEINLINE void Empty() noexcept requires (Self::IsDynamic()) { this->Impl.Empty(); }
    FORCEINLINE void SwapStrings(Self& InOther) noexcept { this->Impl.SwapBuffers(InOther.Impl); }

    FORCEINLINE bool operator==(LNullptrTy) const noexcept { return this->IsEmpty(); }
    FORCEINLINE bool operator==(const Self& InOther) const noexcept;
    FORCEINLINE bool operator==(const T* InString) const noexcept;
    FORCEINLINE bool operator==(const T InRune) const noexcept;
    FORCEINLINE bool operator!=(LNullptrTy) const noexcept { return !(*this == nullptr); }
    FORCEINLINE bool operator!=(const Self& InOther) const noexcept { return !(*this == InOther); }
    FORCEINLINE bool operator!=(const T* InString) const noexcept { return !(*this == InString); }
    FORCEINLINE bool operator!=(const T InRune) const noexcept { return !(*this == InRune); }
    FORCEINLINE bool Equals(LNullptrTy) const noexcept { return *this == nullptr; }
    FORCEINLINE bool Equals(const Self& InOther) const noexcept { return *this == InOther; }
    FORCEINLINE bool Equals(const T* InString) const noexcept { return *this == InString; }
    FORCEINLINE bool Equals(const T InRune) const noexcept { return *this == InRune; }
    FORCEINLINE bool Equals(const T* InString, const T* InEnd) const noexcept;

    FORCEINLINE std::strong_ordering operator<=>(const Self& InOther) const noexcept;
    FORCEINLINE std::strong_ordering operator<=>(const T* InString) const noexcept;
    template <typename TOtherString>
    FORCEINLINE std::strong_ordering operator<=>(const TOtherString& InOther) const noexcept requires (Self::template IsValidOtherString<TOtherString>());
    FORCEINLINE std::strong_ordering SpaceShip(const T* InString, const T* InSlack) const noexcept;

    FORCEINLINE Self& operator+=(const Self& InOther) noexcept requires (Self::IsDynamic()) { this->Append(InOther); return this->GetSelf(); }
    FORCEINLINE Self& operator+=(const T InRune) noexcept requires (Self::IsDynamic()) { this->Add(InRune); return this->GetSelf(); }
    FORCEINLINE Self& operator+=(const T* InString) noexcept requires (Self::IsDynamic()) { this->Append(InString); return this->GetSelf(); }

    FORCEINLINE Self  operator /(const T InRune) const noexcept requires (Self::IsStrongAlloc());
    FORCEINLINE Self  operator /(const T* InString) const noexcept requires (Self::IsStrongAlloc());
    FORCEINLINE Self  operator /(const Self& InOther) const noexcept requires (Self::IsStrongAlloc());
    FORCEINLINE Self& operator/=(const T InRune) noexcept requires (Self::IsDynamic()) { return this->AppendPath(InRune); }
    FORCEINLINE Self& operator/=(const T* InString) noexcept requires (Self::IsDynamic()) { return this->AppendPath(InString); }
    FORCEINLINE Self& operator/=(const Self& InOther) noexcept requires (Self::IsDynamic()) { return this->AppendPath(InOther); }
    FORCEINLINE Self& AppendPath(const T InRune) noexcept requires (Self::IsDynamic());
    FORCEINLINE Self& AppendPath(const T* InString) noexcept requires (Self::IsDynamic());
    FORCEINLINE Self& AppendPath(const T* InString, const SizeType InLength) noexcept requires (Self::IsDynamic());
    FORCEINLINE Self& AppendPath(const Self& InOther) noexcept requires (Self::IsDynamic());
    FORCEINLINE Self& AppendPath(const Self& InOther, const SizeType InLength) noexcept requires (Self::IsDynamic());

    FORCEINLINE SizeType Add(const T InRune) noexcept requires (Self::IsDynamic());
    FORCEINLINE void     AddAt(const SizeType InRuneIndex, const T InRune) noexcept requires (Self::IsDynamic());

    FORCEINLINE void Append(const Self& InOther) noexcept requires (Self::IsDynamic());
    FORCEINLINE void Append(const T* InString) noexcept requires (Self::IsDynamic());
    FORCEINLINE void Append(const T* InString, const SizeType InLength) noexcept requires (Self::IsDynamic());
    FORCEINLINE void AppendAt(const SizeType InRuneIndex, const Self& InOther) noexcept requires (Self::IsDynamic());
    FORCEINLINE void AppendAt(const SizeType InRuneIndex, const T* InString) noexcept requires (Self::IsDynamic());
    FORCEINLINE void AppendAt(const SizeType InRuneIndex, const T* InString, const SizeType InLength) noexcept requires (Self::IsDynamic());

    FORCEINLINE void RemoveAt(const SizeType InRuneIndex) noexcept requires (Self::IsDynamic());
    FORCEINLINE void RemoveAt(const SizeType InRuneIndex, const SizeType InLength) noexcept requires (Self::IsDynamic());
    //# @return The number of runes removed.
    FORCEINLINE SizeType RemoveCharacterAt(const SizeType InRuneIndex) noexcept requires (Self::IsDynamic());
    FORCEINLINE SizeType RemoveCharacterAt(const SizeType InRuneIndex, const SizeType InCharacters) noexcept requires (Self::IsDynamic());

    FORCEINLINE const T* Peek() const noexcept;
    FORCEINLINE bool Pop() noexcept requires (Self::IsDynamic());
    FORCEINLINE bool PopCharacter() noexcept requires (Self::IsDynamic());
    FORCEINLINE void Pop(const SizeType InRuneCount) noexcept requires (Self::IsDynamic());
    FORCEINLINE void PopCharacter(SizeType InCharacterCount) noexcept requires (Self::IsDynamic());

    FORCEINLINE bool StartsWith(const T InRune) const noexcept;
    FORCEINLINE bool StartsWith(const T* InString) const noexcept;
    FORCEINLINE bool StartsWith(const T* InString, const SizeType InLength) const noexcept;
    FORCEINLINE bool StartsWith(const Self& InOther) const noexcept;
    FORCEINLINE bool StartsWith(const Self& InOther, const SizeType InLength) const noexcept;
    FORCEINLINE static bool StartsWith(const T* InString, const T* InEnd, const T* InSubString, const T* InSubEnd) noexcept;
    FORCEINLINE bool EndsWith(const T InRune) const noexcept;
    FORCEINLINE bool EndsWith(const T* InString) const noexcept;
    FORCEINLINE bool EndsWith(const T* InString, const SizeType InLength) const noexcept;
    FORCEINLINE bool EndsWith(const Self& InOther) const noexcept;
    FORCEINLINE bool EndsWith(const Self& InOther, const SizeType InLength) const noexcept;

    FORCEINLINE        SizeType FindFirst(                                              const T InRune) const noexcept;
    FORCEINLINE static SizeType FindFirst(const T* InString, const T* InEnd,            const T InRune) noexcept;
    FORCEINLINE static SizeType FindFirst(const T* InString, const SizeType InLength,   const T InRune) noexcept;
    FORCEINLINE        SizeType FindFirst(                                              const T* InSubString) const noexcept;
    FORCEINLINE static SizeType FindFirst(const T* InString, const T* InEnd,            const T* InSubString) noexcept;
    FORCEINLINE static SizeType FindFirst(const T* InString, const SizeType InLength,   const T* InSubString) noexcept;
    FORCEINLINE        SizeType FindFirst(                                              const T* InSubString, const T* InSubEnd) const noexcept;
    FORCEINLINE static SizeType FindFirst(const T* InString, const T* InEnd,            const T* InSubString, const T* InSubEnd) noexcept;
    FORCEINLINE static SizeType FindFirst(const T* InString, const SizeType InLength,   const T* InSubString, const T* InSubEnd) noexcept;
    FORCEINLINE        SizeType FindFirst(                                              const T* InSubString, const SizeType InSubLength) const noexcept;
    FORCEINLINE static SizeType FindFirst(const T* InString, const T* InEnd,            const T* InSubString, const SizeType InSubLength) noexcept;
    FORCEINLINE static SizeType FindFirst(const T* InString, const SizeType InLength,   const T* InSubString, const SizeType InSubLength) noexcept;
    FORCEINLINE        SizeType FindFirst(                                              const Self& InSubString) const noexcept;
    FORCEINLINE static SizeType FindFirst(const T* InString, const T* InEnd,            const Self& InSubString) noexcept;
    FORCEINLINE static SizeType FindFirst(const T* InString, const SizeType InLength,   const Self& InSubString) noexcept;
    FORCEINLINE        SizeType FindFirst(                                              const Self& InSubString, const T* InSubEnd) const noexcept;
    FORCEINLINE static SizeType FindFirst(const T* InString, const T* InEnd,            const Self& InSubString, const T* InSubEnd) noexcept;
    FORCEINLINE static SizeType FindFirst(const T* InString, const SizeType InLength,   const Self& InSubString, const T* InSubEnd) noexcept;
    FORCEINLINE        SizeType FindFirst(                                              const Self& InSubString, const SizeType InSubLength) const noexcept;
    FORCEINLINE static SizeType FindFirst(const T* InString, const T* InEnd,            const Self& InSubString, const SizeType InSubLength) noexcept;
    FORCEINLINE static SizeType FindFirst(const T* InString, const SizeType InLength,   const Self& InSubString, const SizeType InSubLength) noexcept;

    FORCEINLINE SizeType FindSecond(const T InRune) const noexcept;
    FORCEINLINE SizeType FindSecond(const T* InString) const noexcept;
    FORCEINLINE SizeType FindSecond(const T* InString, const SizeType InLength) const noexcept;
    FORCEINLINE SizeType FindSecond(const Self& InOther) const noexcept;
    FORCEINLINE SizeType FindSecond(const Self& InOther, const SizeType InLength) const noexcept;
    FORCEINLINE SizeType FindLast(const T InRune) const noexcept;
    FORCEINLINE SizeType FindLast(const T* InString) const noexcept;
    FORCEINLINE SizeType FindLast(const T* InString, const SizeType InLength) const noexcept;
    FORCEINLINE SizeType FindLast(const Self& InOther) const noexcept;
    FORCEINLINE SizeType FindLast(const Self& InOther, const SizeType InLength) const noexcept;
    FORCEINLINE SizeType FindSecondLast(const T InRune) const noexcept;
    FORCEINLINE SizeType FindSecondLast(const T* InString) const noexcept;
    FORCEINLINE SizeType FindSecondLast(const T* InString, const SizeType InLength) const noexcept;
    FORCEINLINE SizeType FindSecondLast(const Self& InOther) const noexcept;
    FORCEINLINE SizeType FindSecondLast(const Self& InOther, const SizeType InLength) const noexcept;

    FORCEINLINE bool Contains(const T InRune) const noexcept;
    FORCEINLINE bool Contains(const T* InString) const noexcept;
    FORCEINLINE bool Contains(const T* InString, const SizeType InLength) const noexcept;
    FORCEINLINE bool Contains(const Self& InOther) const noexcept;
    FORCEINLINE bool Contains(const Self& InOther, const SizeType InLength) const noexcept;

    FORCEINLINE void InlineCut(const SizeType InRuneIndex) noexcept requires (Self::IsDynamic());
    FORCEINLINE Self Cut(const SizeType InRuneIndex) const noexcept requires (Self::IsStrongAlloc());
    FORCEINLINE Self Cut(const SizeType InRuneIndex) const noexcept requires (Self::IsWeakAlloc());
    template <typename TOtherString>
    FORCEINLINE TOtherString Cut(const SizeType InRuneIndex) const noexcept requires (Self::template IsValidOtherString<TOtherString>());

    FORCEINLINE void InlineSub(const SizeType InRuneStartIndex, const SizeType InRuneCount) noexcept requires (Self::IsDynamic());
    FORCEINLINE void InlineSubIdx(const SizeType InRuneStartIndex, const SizeType InRuneEndIndex) noexcept requires (Self::IsDynamic());
    FORCEINLINE Self Sub(const SizeType InRuneStartIndex, const SizeType InRuneCount) const noexcept requires (Self::IsStrongAlloc());
    FORCEINLINE Self Sub(const SizeType InRuneStartIndex, const SizeType InRuneCount) const noexcept requires (Self::IsWeakAlloc());
    FORCEINLINE Self SubIdx(const SizeType InRuneStartIndex, const SizeType InRuneEndIndex) const noexcept;
    template <typename TOtherString>
    FORCEINLINE TOtherString Sub(const SizeType InRuneStartIndex, const SizeType InRuneCount) const noexcept requires (Self::template IsValidOtherString<TOtherString>());
    template <typename TOtherString>
    FORCEINLINE TOtherString SubIdx(const SizeType InRuneStartIndex, const SizeType InRuneEndIndex) const noexcept requires (Self::template IsValidOtherString<TOtherString>());

    FORCEINLINE void InlineRightChop(const SizeType InRuneIndex) noexcept requires (Self::IsDynamic());
    FORCEINLINE Self RightChop(const SizeType InRuneIndex) const noexcept;
    template <typename TOtherString>
    FORCEINLINE TOtherString RightChop(const SizeType InRuneIndex) const noexcept requires (Self::template IsValidOtherString<TOtherString>());
    FORCEINLINE void InlineLeftChop(const SizeType InRuneIndex) noexcept requires (Self::IsDynamic());
    FORCEINLINE Self LeftChop(const SizeType InRuneIndex) const noexcept;
    template <typename TOtherString>
    FORCEINLINE TOtherString LeftChop(const SizeType InRuneIndex) const noexcept requires (Self::template IsValidOtherString<TOtherString>());

    //#
    //# Replace all occurrences of the input string with the replacement string.
    //# @return The number of substrings replaced.
    //#
    FORCEINLINE SizeType Replace(const T InRune, const T InReplacement) noexcept requires (Self::IsContentMutable());
    FORCEINLINE SizeType Replace(const T* InString, const T* InReplacement) noexcept requires (Self::IsContentMutable());
    FORCEINLINE SizeType Replace(const T* InString, const T* InEnd, const T* InReplacement, const T* InReplacementEnd) noexcept requires (Self::IsContentMutable());

    //#
    //# Substitute the range of runes [InStart, InEnd[ with the range of runes [InReplacement, InReplacementEnd[.
    //#
    FORCEINLINE void Substitute(T* InStart, T* InEnd, const T* InReplacement, const T* InReplacementEnd) noexcept requires (Self::IsDynamic());

    FORCEINLINE SizeType Count(const T InRune) const noexcept;
    FORCEINLINE SizeType Count(const T* InString) const noexcept;
    FORCEINLINE SizeType Count(const T* InString, const SizeType InLength) const noexcept;
    FORCEINLINE SizeType Count(const Self& InOther) const noexcept;
    FORCEINLINE SizeType Count(const Self& InOther, const SizeType InLength) const noexcept;

    FORCEINLINE void ToLower() noexcept requires (Self::IsContentMutable());
    FORCEINLINE Self GetLowerCase() const noexcept requires (Self::IsStrongAlloc());
    template <typename TOtherString>
    FORCEINLINE TOtherString GetLowerCase() const noexcept requires (TOtherString::IsStrongAlloc() && Self::template IsValidOtherString<TOtherString>());

    FORCEINLINE SizeType GetLineNumber(const SizeType InRuneIndex) const;

    template <typename... TArgs>
    static Self SprintF(const T* InFormat, const TArgs&... InArgs) noexcept  requires (Self::IsStrongAlloc());

    FORCEINLINE       T* GetBegin()           noexcept requires (Self::IsContentMutable()) { return this->Impl.GetData(); }
    FORCEINLINE const T* GetBegin()     const noexcept { return this->Impl.GetData(); }
    FORCEINLINE       T* GetSlack()           noexcept requires (Self::IsContentMutable()) { return this->Impl.GetSlack(); } // Use with caution. This is not the end of the string.
    FORCEINLINE const T* GetSlack()     const noexcept { return this->Impl.GetSlack(); }
    FORCEINLINE       T* GetEnd()             noexcept requires (Self::IsStrongAlloc() && Self::IsContentMutable()) { return this->GetBegin() + this->GetRuneCount(); }
    FORCEINLINE const T* GetEnd()       const noexcept requires (Self::IsStrongAlloc()) { return this->GetBegin() + this->GetRuneCount(); }
    FORCEINLINE       T* GetEnd()             noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable()) { return this->GetSlack(); }
    FORCEINLINE const T* GetEnd()       const noexcept requires (Self::IsWeakAlloc()) { return this->GetSlack(); }

    FORCEINLINE Iterator<T>       begin()       noexcept requires (Self::IsContentMutable()) { return this->Impl.begin(); }
    FORCEINLINE Iterator<const T> begin() const noexcept { return this->Impl.begin(); }
    FORCEINLINE Iterator<const T> end()   const noexcept requires (Self::IsStrongAlloc()) { auto It = this->Impl.end(); return It == this->begin() ? It : /*Terminator*/ --It; }
    FORCEINLINE Iterator<T>       end()         noexcept requires (Self::IsStrongAlloc() && Self::IsContentMutable()) { auto It = this->Impl.end(); return It == this->begin() ? It : /*Terminator*/ --It; }
    FORCEINLINE Iterator<const T> end()   const noexcept requires (Self::IsWeakAlloc()) { return this->Impl.end(); }
    FORCEINLINE Iterator<T>       end()         noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable()) { return this->Impl.end(); }

    FORCEINLINE constexpr       Alloc& GetUnderlyingDataStructure()       noexcept { return this->Impl; }
    FORCEINLINE constexpr const Alloc& GetUnderlyingDataStructure() const noexcept { return this->Impl; }

#if LAL_CHECK_STRING_VALIDITY
    //# THIS IS FOR INTERNAL USE ONLY. DO NOT CALL MANUALLY.
    void EnsureValidState() const; // Private function. DO NOT CALL MANUALLY.
#endif /* LAL_CHECK_STRING_VALIDITY */

protected:

    Alloc Impl;
};

template <typename InTraits, typename InAlloc>
class _TStringBase : public TStringBase<_TStringBase<InTraits, InAlloc>, InTraits, InAlloc>
{
public:

    using Super = TStringBase<_TStringBase<InTraits, InAlloc>, InTraits, InAlloc>;

    using Self     = typename Super::Self;
    using Traits   = typename Super::Traits;
    using Alloc    = typename Super::Alloc;
    using T        = typename Super::T;
    using SizeType = typename Super::SizeType;

    friend void OnDefaultOnlyMallocMember(Self* MemberField);
    friend void OnDefaultOnlyMallocMember(TArray<Self>* MemberField);

    template <typename TOtherTraits, typename TOtherAlloc>
    friend class _TStringBase;

    template <typename TOtherString>
    FORCEINLINE consteval static bool IsValidOtherString() noexcept { return Super::template IsValidOtherString<TOtherString>(); }

    FORCEINLINE consteval static bool IsDynamic()        noexcept { return Super::IsDynamic();        }
    FORCEINLINE consteval static bool IsWeakAlloc()      noexcept { return Super::IsWeakAlloc();      }
    FORCEINLINE consteval static bool IsStrongAlloc()    noexcept { return Super::IsStrongAlloc();    }
    FORCEINLINE consteval static bool IsContentConst()   noexcept { return Super::IsContentConst();   }
    FORCEINLINE consteval static bool IsContentMutable() noexcept { return Super::IsContentMutable(); }

    FORCEINLINE  _TStringBase() noexcept = default;
    FORCEINLINE  _TStringBase(LNullptrTy) noexcept : Super(nullptr) { }
    FORCEINLINE  _TStringBase(const Self& InOther) noexcept : Super(InOther) { }
    FORCEINLINE  _TStringBase(Self&& InOther) noexcept : Super(std::move(InOther)) { }
    explicit     _TStringBase(const T InRune) noexcept requires (Self::IsStrongAlloc()) : Super(InRune) { }
    explicit     _TStringBase(const T& InRune) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst()) : Super(InRune) { }
    explicit     _TStringBase(      T& InRune) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable()) : Super(InRune) { }
    FORCEINLINE  _TStringBase(const T* InString) noexcept requires (Self::IsStrongAlloc()) : Super(InString) { }
    FORCEINLINE  _TStringBase(const T* InString) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst()) : Super(InString) { }
    FORCEINLINE  _TStringBase(      T* InString) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable()) : Super(InString) { }
    FORCEINLINE  _TStringBase(const T* InString, const SizeType InLength) noexcept requires (Self::IsStrongAlloc()) : Super(InString, InLength) { }
    FORCEINLINE  _TStringBase(const T* InString, const SizeType InLength) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst()) : Super(InString, InLength) { }
    FORCEINLINE  _TStringBase(      T* InString, const SizeType InLength) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable()) : Super(InString, InLength) { }
    FORCEINLINE  _TStringBase(const T* InString, const T* InEnd) noexcept requires (Self::IsStrongAlloc()) : Super(InString, InEnd) { }
    FORCEINLINE  _TStringBase(const T* InString, const T* InEnd) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst()) : Super(InString, InEnd) { }
    FORCEINLINE  _TStringBase(      T* InString,       T* InEnd) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable()) : Super(InString, InEnd) { }
    FORCEINLINE ~_TStringBase() noexcept = default;
    template <typename TOtherString>
    FORCEINLINE  _TStringBase(const TOtherString& Other) noexcept requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) : Super(Other) { }
    template <typename TOtherString>
    FORCEINLINE  _TStringBase(const TOtherString& Other) noexcept requires (Self::IsStrongAlloc() && Self::template IsValidOtherString<TOtherString>()) : Super(Other) { }
    template <typename TOtherString>
    FORCEINLINE  _TStringBase(TOtherString&& Other) noexcept requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) = delete;
    template <typename TOtherString>
    FORCEINLINE  _TStringBase(const TOtherString&& Other) noexcept requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) = delete;
    template <typename TOtherString>
    FORCEINLINE  _TStringBase(TOtherString&& InString) noexcept requires (Self::IsWeakAlloc() && TOtherString::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) : Super(std::move(InString)) { }
    template <typename TOtherString>
    FORCEINLINE  _TStringBase(TOtherString&& InString) noexcept requires (Self::IsStrongAlloc() && TOtherString::IsStrongAlloc() && Self::template IsValidOtherString<TOtherString>()) : Super(std::move(InString)) { }

    FORCEINLINE Self& operator=(LNullptrTy) noexcept { return this->Super::operator=(nullptr); }
    FORCEINLINE Self& operator=(const Self& InOther) noexcept { return this->Super::operator=(InOther); }
    FORCEINLINE Self& operator=(Self&& InOther) noexcept { return this->Super::operator=(std::move(InOther)); }
    FORCEINLINE Self& operator=(const T InRune) noexcept requires (Self::IsStrongAlloc()) { return this->Super::operator=(InRune); }
    FORCEINLINE Self& operator=(const T& InRune) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst()) { return this->Super::operator=(InRune); }
    FORCEINLINE Self& operator=(      T& InRune) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable()) { return this->Super::operator=(InRune); }
    FORCEINLINE Self& operator=(const T* InString) noexcept requires (Self::IsStrongAlloc()) { return this->Super::operator=(InString); }
    FORCEINLINE Self& operator=(const T* InString) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst()) { return this->Super::operator=(InString); }
    FORCEINLINE Self& operator=(      T* InString) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable()) { return this->Super::operator=(InString); }
    template <typename TOtherString>
    FORCEINLINE Self& operator=(const TOtherString& Other) noexcept requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>());
    template <typename TOtherString>
    FORCEINLINE Self& operator=(const TOtherString& Other) noexcept requires (Self::IsStrongAlloc() && Self::template IsValidOtherString<TOtherString>());
    template <typename TOtherString>
    FORCEINLINE Self& operator=(TOtherString&& Other) noexcept requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) = delete;
    template <typename TOtherString>
    FORCEINLINE Self& operator=(const TOtherString&& Other) noexcept requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) = delete;
    template <typename TOtherString>
    FORCEINLINE Self& operator=(TOtherString&& InString) noexcept requires (Self::IsWeakAlloc() && TOtherString::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>());
    template <typename TOtherString>
    FORCEINLINE Self& operator=(TOtherString&& InString) noexcept requires (Self::IsStrongAlloc() && TOtherString::IsStrongAlloc() && Self::template IsValidOtherString<TOtherString>());

    using Super::operator<=>;
};

template<typename InT>
template<typename SizeType>
FORCEINLINE SizeType TStringTraits<InT>::GetStringLength(const T* InString) noexcept
{
    SizeType Out = 0;
    while (*InString++ != Self::Terminator)
    {
        ++Out;
    }

    return Out;
}

template<typename InT>
template<typename SizeType>
SizeType TStringTraits<InT>::GetCharacterCount(const T* InString) noexcept
{
    SizeType Out = 0;

    const auto GetSize = [](const T InFirstByte)
    {
        if ((InFirstByte & 0x80) == 0)
        { /* ASCII byte. */
            return 1;
        }

        if ((InFirstByte & 0xE0) == 0xC0)
        { /* 2-byte sequence. */
            return 2;
        }

        if ((InFirstByte & 0xF0) == 0xE0)
        { /* 3-byte sequence. */
            return 3;
        }

        if ((InFirstByte & 0xF8) == 0xF0)
        { /* 4-byte sequence. */
            return 4;
        }

        checkNoEntry()
        /* Invalid UTF-8. */
        // Return 1; So we do not hang indefinitely.
        return 1;
    };

    while (*InString != Self::Terminator)
    {
        ++Out;
        InString += GetSize(*InString);

        continue;
    }

    return Out;
}

template<typename InT>
template<typename SizeType>
FORCEINLINE SizeType TStringTraits<InT>::GetCharacterCount(const T* InString, const T* InEnd) noexcept
{
    SizeType Out = 0;

    const auto GetSize = [](const T InFirstByte)
    {
        if ((InFirstByte & 0x80) == 0)
        { /* ASCII byte. */
            return 1;
        }

        if ((InFirstByte & 0xE0) == 0xC0)
        { /* 2-byte sequence. */
            return 2;
        }

        if ((InFirstByte & 0xF0) == 0xE0)
        { /* 3-byte sequence. */
            return 3;
        }

        if ((InFirstByte & 0xF8) == 0xF0)
        { /* 4-byte sequence. */
            return 4;
        }

        checkNoEntry()
        /* Invalid UTF-8. */
        // Return 1; So we do not hang indefinitely.
        return 1;
    };

    while (InString != InEnd)
    {
        check( *InString != Self::Terminator )

        ++Out;
        InString += GetSize(*InString);

        continue;
    }

    return Out;
}

template<typename InT>
template<typename SizeType>
FORCEINLINE SizeType TStringTraits<InT>::GetCharacterSize(const T* InCharacter) noexcept
{
    if ((*InCharacter & 0x80) == 0)
    { /* ASCII byte. */
        return 1;
    }

    if ((*InCharacter & 0xE0) == 0xC0)
    { /* 2-byte sequence. */
        return 2;
    }

    if ((*InCharacter & 0xF0) == 0xE0)
    { /* 3-byte sequence. */
        return 3;
    }

    if ((*InCharacter & 0xF8) == 0xF0)
    { /* 4-byte sequence. */
        return 4;
    }

    checkNoEntry()
    /* Invalid UTF-8. */
    // Return 1; So we do not hang indefinitely.
    return 1;
}

template<typename InT>
template<typename SizeType>
void TStringTraits<InT>::GoToMostRecentValidCharacter(const T* Self, SizeType* InOutCursor) noexcept
{
    while (*InOutCursor > INDEX_NONE)
    {
        if
        (
               ((*(Self + *InOutCursor) & 0x80) == 0x00) // ASCII byte. (0xxx xxxx)
            || ((*(Self + *InOutCursor) & 0xC0) == 0xC0) // Multi-byte. (11xx xxxx)
        )
        {
            break;
        }

        --*InOutCursor;
    }

    return;
}

template<typename InT>
FORCEINLINE typename TStringTraits<InT>::T TStringTraits<InT>::ToLower(const T InRune)
{
    if (InRune >= 'A' && InRune <= 'Z')
    {
        return InRune + 32;
    }

    return InRune;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE TStringBase<Derived, InTraits, InAlloc>::TStringBase(const T InRune) noexcept requires (Self::IsStrongAlloc()) : Impl()
{
    this->Reserve(1);
    this->Impl.Add(InRune);
    this->Impl.Add(Traits::Terminator);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE TStringBase<Derived, InTraits, InAlloc>::TStringBase(const T& InRune) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst())
{
    this->Impl.GetUnderlyingDataStructure().Data  = &InRune;
    this->Impl.GetUnderlyingDataStructure().Slack = &InRune + 1;
    this->Impl.GetUnderlyingDataStructure().End   = &InRune + 1;

    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE TStringBase<Derived, InTraits, InAlloc>::TStringBase(T& InRune) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable())
{
    this->Impl.GetUnderlyingDataStructure().Data  = &InRune;
    this->Impl.GetUnderlyingDataStructure().Slack = &InRune + 1;
    this->Impl.GetUnderlyingDataStructure().End   = &InRune + 1;

    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE TStringBase<Derived, InTraits, InAlloc>::TStringBase(const T* InString) noexcept requires (Self::IsStrongAlloc()) : Impl()
{
    const SizeType Length = Traits::template GetStringLength<SizeType>(InString);
    this->Reserve(Length);

    this->Impl.Append(InString, Length + /*Terminator*/1);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE TStringBase<Derived, InTraits, InAlloc>::TStringBase(const T* InString) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst())
{
    const SizeType Length = Traits::template GetStringLength<SizeType>(InString);

    this->Impl.GetUnderlyingDataStructure().Data  = InString;
    this->Impl.GetUnderlyingDataStructure().Slack = InString + Length;
    this->Impl.GetUnderlyingDataStructure().End   = InString + Length;

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE TStringBase<Derived, InTraits, InAlloc>::TStringBase(T* InString) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable())
{
    const SizeType Length = Traits::template GetStringLength<SizeType>(InString);

    this->Impl.GetUnderlyingDataStructure().Data  = InString;
    this->Impl.GetUnderlyingDataStructure().Slack = InString + Length;
    this->Impl.GetUnderlyingDataStructure().End   = InString + Length;

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE TStringBase<Derived, InTraits, InAlloc>::TStringBase(const T* InString, const SizeType InLength) noexcept requires (Self::IsStrongAlloc()) : Impl()
{
    this->Reserve(InLength);

    this->Impl.Append(InString, InLength);
    this->Impl.Add(Traits::Terminator);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE TStringBase<Derived, InTraits, InAlloc>::TStringBase(const T* InString, const SizeType InLength) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst())
{
    this->Impl.GetUnderlyingDataStructure().Data  = InString;
    this->Impl.GetUnderlyingDataStructure().Slack = InString + InLength;
    this->Impl.GetUnderlyingDataStructure().End   = InString + InLength;

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE TStringBase<Derived, InTraits, InAlloc>::TStringBase(T* InString, const SizeType InLength) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable())
{
    this->Impl.GetUnderlyingDataStructure().Data  = InString;
    this->Impl.GetUnderlyingDataStructure().Slack = InString + InLength;
    this->Impl.GetUnderlyingDataStructure().End   = InString + InLength;

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE TStringBase<Derived, InTraits, InAlloc>::TStringBase(const T* InString, const T* InEnd) noexcept requires (Self::IsStrongAlloc()) : Impl()
{
    check( InString <= InEnd && (InString < InEnd ? *InEnd != Traits::Terminator : true) )

    this->Impl.Reserve(InEnd - InString + 1);

    this->Impl.Append(InString, InEnd);
    this->Impl.Add(Traits::Terminator);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE TStringBase<Derived, InTraits, InAlloc>::TStringBase(const T* InString, const T* InEnd) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst())
{
    check( InString <= InEnd )

    this->Impl.GetUnderlyingDataStructure().Data  = InString;
    this->Impl.GetUnderlyingDataStructure().Slack = InEnd;
    this->Impl.GetUnderlyingDataStructure().End   = InEnd;

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE TStringBase<Derived, InTraits, InAlloc>::TStringBase(T* InString, T* InEnd) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable())
{
    check( InString <= InEnd )

    this->Impl.GetUnderlyingDataStructure().Data  = InString;
    this->Impl.GetUnderlyingDataStructure().Slack = InEnd;
    this->Impl.GetUnderlyingDataStructure().End   = InEnd;

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE TStringBase<Derived, InTraits, InAlloc>::TStringBase(const TOtherString& Other) noexcept requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) : Impl(Other.GetUnderlyingDataStructure())
{
    if
    (
        this->Impl.GetUnderlyingDataStructure().Slack - 1 > this->Impl.GetUnderlyingDataStructure().Data
        && *(this->Impl.GetUnderlyingDataStructure().Slack - 1) == Traits::Terminator)
    {
        --this->Impl.GetUnderlyingDataStructure().Slack;
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE TStringBase<Derived, InTraits, InAlloc>::TStringBase(const TOtherString& Other) noexcept requires (Self::IsStrongAlloc() && Self::template IsValidOtherString<TOtherString>()) : Impl(Other.GetUnderlyingDataStructure())
{
    if (T* Last = this->Impl.Peek(); Last && *Last != Traits::Terminator)
    {
        this->Impl.Reserve(this->Impl.GetSize() + 1);
        this->Impl.Emplace(Traits::Terminator);
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE TStringBase<Derived, InTraits, InAlloc>::TStringBase(TOtherString&& InString) noexcept
    requires (Self::IsWeakAlloc() && TOtherString::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) : Impl(std::move(InString.GetUnderlyingDataStructure()))
{
    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE TStringBase<Derived, InTraits, InAlloc>::TStringBase(TOtherString&& InString) noexcept
    requires (Self::IsStrongAlloc() && TOtherString::IsStrongAlloc() && Self::template IsValidOtherString<TOtherString>()) : Impl(std::move(InString.GetUnderlyingDataStructure()))
{
    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self& TStringBase<Derived, InTraits, InAlloc>::operator=(const T InRune) noexcept requires (Self::IsStrongAlloc())
{
    this->Reset(1);

    this->Impl.Add(InRune);
    this->Impl.Add(Traits::Terminator);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return this->GetSelf();
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self& TStringBase<Derived, InTraits, InAlloc>::operator=(const T& InRune) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst())
{
    this->Impl.GetUnderlyingDataStructure().Data  = &InRune;
    this->Impl.GetUnderlyingDataStructure().Slack = &InRune + 1;
    this->Impl.GetUnderlyingDataStructure().End   = &InRune + 1;

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return this->GetSelf();
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self& TStringBase<Derived, InTraits, InAlloc>::operator=(T& InRune) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable())
{
    this->Impl.GetUnderlyingDataStructure().Data  = &InRune;
    this->Impl.GetUnderlyingDataStructure().Slack = &InRune + 1;
    this->Impl.GetUnderlyingDataStructure().End   = &InRune + 1;

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return this->GetSelf();
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self& TStringBase<Derived, InTraits, InAlloc>::operator=(const T* InString) noexcept requires (Self::IsStrongAlloc())
{
    const SizeType Length = Traits::template GetStringLength<SizeType>(InString);
    this->Reset(Length);

    this->Impl.Append(InString, Length + /*Terminator*/1);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return this->GetSelf();
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self& TStringBase<Derived, InTraits, InAlloc>::operator=(const T* InString) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst())
{
    const SizeType Length = Traits::template GetStringLength<SizeType>(InString);

    this->Impl.GetUnderlyingDataStructure().Data  = InString;
    this->Impl.GetUnderlyingDataStructure().Slack = InString + Length;
    this->Impl.GetUnderlyingDataStructure().End   = InString + Length;

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return this->GetSelf();
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self& TStringBase<Derived, InTraits, InAlloc>::operator=(T* InString) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable())
{
    const SizeType Length = Traits::template GetStringLength<SizeType>(InString);

    this->Impl.GetUnderlyingDataStructure().Data  = InString;
    this->Impl.GetUnderlyingDataStructure().Slack = InString + Length;
    this->Impl.GetUnderlyingDataStructure().End   = InString + Length;

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return this->GetSelf();
}

template<typename Derived, typename InTraits, typename InAlloc>
template<typename TOtherDerived>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self& TStringBase<Derived, InTraits, InAlloc>::operator=(const TOtherDerived& Other)
    noexcept requires (Self::IsWeakAlloc() && std::is_same_v<Self, TOtherDerived> == false)
{
    this->Impl = Other.GetUnderlyingDataStructure();

    if
    (
        this->Impl.GetUnderlyingDataStructure().Slack - 1 > this->Impl.GetUnderlyingDataStructure().Data
        && *(this->Impl.GetUnderlyingDataStructure().Slack - 1) == Traits::Terminator)
    {
        --this->Impl.GetUnderlyingDataStructure().Slack;
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return this->GetSelf();
}

template<typename Derived, typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self& TStringBase<Derived, InTraits, InAlloc>::operator=(const TOtherString& Other)
    noexcept requires (Self::IsStriongAlloc() && Self::template IsValidOtherString<TOtherString>())
{
    this->Impl = Other.GetUnderlyingDataStructure();

    if (T* Last = this->Impl.Peek(); Last && *Last != Traits::Terminator)
    {
        this->Impl.Reserve(this->Impl.GetSize() + 1);
        this->Impl.Emplace(Traits::Terminator);
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return this->GetSelf();
}

template<typename Derived, typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self& TStringBase<Derived, InTraits, InAlloc>::operator=(TOtherString&& InString) noexcept
    requires (Self::IsWeakAlloc() && TOtherString::IsWeakAlloc() && std::is_same_v<TOtherString, Derived> == false)
{
    this->Impl = std::move(InString.GetUnderlyingDataStructure());

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return this->GetSelf();
}

template<typename Derived, typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self& TStringBase<Derived, InTraits, InAlloc>::operator=(TOtherString&& InString) noexcept
    requires (Self::IsStrongAlloc() && TOtherString::IsStrongAlloc() && std::is_same_v<TOtherString, Derived> == false)
{
    this->Impl = std::move(InString.GetUnderlyingDataStructure());

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return this->GetSelf();
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::GetRuneCountOfCharacterAt(const SizeType InRuneIndex) const noexcept
{
    return Traits::template GetCharacterSize<SizeType>(this->GetBegin() + InRuneIndex);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::T& TStringBase<Derived, InTraits, InAlloc>::operator[](const SizeType InRuneIndex) noexcept requires (Self::IsStrongAlloc() && Self::IsContentMutable())
{
    JAFG_CHECK_STRING( this->IsValidIndex(InRuneIndex) )
    return *(this->Impl.GetData() + InRuneIndex);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE const typename TStringBase<Derived, InTraits, InAlloc>::T& TStringBase<Derived, InTraits, InAlloc>::operator[](const SizeType InRuneIndex) const noexcept requires (Self::IsStrongAlloc())
{
    JAFG_CHECK_STRING( this->IsValidIndex(InRuneIndex) )
    return *(this->Impl.GetData() + InRuneIndex);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::T& TStringBase<Derived, InTraits, InAlloc>::operator[](const SizeType InRuneIndex) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable())
{
    JAFG_CHECK_STRING( this->Impl.IsValidIndex(InRuneIndex) )
    return *(this->Impl.GetData() + InRuneIndex);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE const typename TStringBase<Derived, InTraits, InAlloc>::T& TStringBase<Derived, InTraits, InAlloc>::operator[](const SizeType InRuneIndex) const noexcept requires (Self::IsWeakAlloc())
{
    JAFG_CHECK_STRING( this->Impl.IsValidIndex(InRuneIndex) )
    return *(this->Impl.GetData() + InRuneIndex);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::operator==(const Self& InOther) const noexcept
{
    if (this->IsEmpty())
    {
        return InOther.IsEmpty();
    }

    return this->Impl.IsDataEqual(InOther.Impl);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::operator==(const T* InString) const noexcept
{
    if (this->IsEmpty())
    {
        return InString == nullptr || *InString == Traits::Terminator;
    }

    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (*(this->Impl.GetData() + Index) != *(InString + Index))
        {
            return false;
        }

        continue;
    }

    return true;
}

template<typename Derived, typename InTraits, typename InAlloc>
bool TStringBase<Derived, InTraits, InAlloc>::operator==(const T InRune) const noexcept
{
    return
           (this->GetRuneCount() == 1 && *this->Peek() == InRune)
        || (this->GetRuneCount() == 0 && InRune == Traits::Terminator);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::Equals(const T* InString, const T* InEnd) const noexcept
{
    check( InString && InEnd )

    if (this->IsEmpty())
    {
        return InString == InEnd
            || (*InString == Traits::Terminator && InEnd == InString + 1);
    }

    for (SizeType Index = 0; Index < this->GetRuneCount(); ++Index)
    {
        if (*(this->Impl.GetData() + Index) != *(InString + Index))
        {
            return false;
        }

        continue;
    }

    return InString + this->GetRuneCount() == InEnd
        || *(InString + this->GetRuneCount()) == Traits::Terminator;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE std::strong_ordering TStringBase<Derived, InTraits, InAlloc>::operator<=>(const Self& InOther) const noexcept
{
    return this->SpaceShip(InOther.GetBegin(), InOther.GetSlack());
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE std::strong_ordering TStringBase<Derived, InTraits, InAlloc>::operator<=>(const T* InString) const noexcept
{
    return this->SpaceShip(InString, InString + Traits::template GetStringLength<SizeType>(InString));
}

template<typename Derived, typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE std::strong_ordering TStringBase<Derived, InTraits, InAlloc>::operator<=>(const TOtherString& InOther) const noexcept requires (Self::template IsValidOtherString<TOtherString>())
{
    return this->SpaceShip(InOther.GetBegin(), InOther.GetSlack());
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE std::strong_ordering TStringBase<Derived, InTraits, InAlloc>::SpaceShip(const T* InString, const T* InSlack) const noexcept
{
    auto Size1 { this->Impl.GetUnderlyingDataStructure().Slack - this->Impl.GetUnderlyingDataStructure().Data };
    auto Size2 { InSlack - InString };
    auto minSize { Maths::Min(Size1, Size2) };

    struct LMyComp final
    {
        FORCEINLINE std::strong_ordering operator()(T Lhs, T Rhs) const noexcept
        {
            T LhsLow { TStringTraits<T>::ToLower(Lhs) };
            T RhsLow { TStringTraits<T>::ToLower(Rhs) };

            if (LhsLow < RhsLow)
            {
                return std::strong_ordering::less;
            }
            if (LhsLow > RhsLow)
            {
                return std::strong_ordering::greater;
            }

            if (Lhs < Rhs)
            {
                return std::strong_ordering::less;
            }
            if (Lhs > Rhs)
            {
                return std::strong_ordering::greater;
            }

            return std::strong_ordering::equal;
        }
    };

    auto result
    {
        std::lexicographical_compare_three_way
        (
            this->Impl.GetUnderlyingDataStructure().Data, this->Impl.GetUnderlyingDataStructure().Data + minSize,
            InString, InString + minSize,
            LMyComp{}
        )
    };

    if (result != 0)
    {
        return result;
    }

    return Size1 <=> Size2;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self TStringBase<Derived, InTraits, InAlloc>::operator/(const T InRune) const noexcept requires (Self::IsStrongAlloc())

{
    Self Out;
    if (*this->Peek() == '/')
    {
        Out.Reserve(this->GetRuneCount() + 1);
        Out = *this;
        Out.Add(InRune);
    }
    else
    {
        Out.Reserve(this->GetRuneCount() + 2);
        Out = *this;
        Out.Add('/');
        Out.Add(InRune);
    }

    return Out;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self TStringBase<Derived, InTraits, InAlloc>::operator/(const T* InString) const noexcept requires (Self::IsStrongAlloc())
{
    const SizeType Length = Traits::template GetStringLength<SizeType>(InString);

    Self Out;
    if (*this->Peek() == '/')
    {
        Out.Reserve(this->GetRuneCount() + Length);
        Out = this->GetSelf();
        Out.Append(InString, Length);
    }
    else
    {
        Out.Reserve(this->GetRuneCount() + Length + 1);
        Out = this->GetSelf();
        Out.Add('/');
        Out.Append(InString, Length);
    }

    return Out;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self TStringBase<Derived, InTraits, InAlloc>::operator/(const Self& InOther) const noexcept requires (Self::IsStrongAlloc())
{
    return this->operator/(InOther.ToPtr());
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self& TStringBase<Derived, InTraits, InAlloc>::AppendPath(const T InRune) noexcept requires (Self::IsDynamic())
{
    if (*this->Peek() == '/')
    {
        this->Add(InRune);
    }
    else
    {
        if (this->IsEmpty() == false)
        {
            this->Reserve(2);
            this->Add('/');
        }
        this->Add(InRune);
    }

    return this->GetSelf();
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self& TStringBase<Derived, InTraits, InAlloc>::AppendPath(const T* InString) noexcept requires (Self::IsDynamic())
{
    return this->AppendPath(InString, Traits::template GetStringLength<SizeType>(InString));
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self& TStringBase<Derived, InTraits, InAlloc>::AppendPath(const T* InString, const SizeType InLength) noexcept requires (Self::IsDynamic())
{
    if (*this->Peek() == '/')
    {
        if (InLength > 0 && *InString == '/')
        {
            this->Append(InString + 1, InLength - 1);
        }
        else
        {
            this->Append(InString, InLength);
        }
    }
    else
    {
        if (this->IsEmpty() == false)
        {
            this->Reserve(InLength + 1);
            this->Add('/');
        }
        if (InLength > 0 && *InString == '/')
        {
            this->Append(InString + 1, InLength - 1);
        }
        else
        {
            this->Append(InString, InLength);
        }
    }

    return this->GetSelf();
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self& TStringBase<Derived, InTraits, InAlloc>::AppendPath(const Self& InOther) noexcept requires (Self::IsDynamic())
{
    return this->AppendPath(InOther.ToPtr(), InOther.GetRuneCount());
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self& TStringBase<Derived, InTraits, InAlloc>::AppendPath(const Self& InOther, const SizeType InLength) noexcept requires (Self::IsDynamic())
{
    return this->AppendPath(InOther.ToPtr(), InLength);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::Add(const T InRune) noexcept requires (Self::IsDynamic())
{
    this->Impl.Pop(); /* Terminator */
    this->Impl.Add(InRune);
    this->Impl.Add(Traits::Terminator);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return this->GetRuneCount();
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::AddAt(const SizeType InRuneIndex, const T InRune) noexcept requires (Self::IsDynamic())
{
    this->Impl.Pop(); /* Terminator */
    this->Impl.AddAt(InRuneIndex, InRune);
    this->Impl.Add(Traits::Terminator);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::Append(const Self& InOther) noexcept requires (Self::IsDynamic())
{
    if (InOther.IsEmpty() == false)
    {
        this->Impl.Pop(); /* Terminator */
        this->Impl.Append(InOther.Impl);
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::Append(const T* InString) noexcept requires (Self::IsDynamic())
{
    const SizeType Length = Traits::template GetStringLength<SizeType>(InString);
    if (Length > 0)
    {
        this->Impl.Pop(); /* Terminator */
        this->Impl.Append(InString, Length + /*Terminator*/1);
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::Append(const T* InString, const SizeType InLength) noexcept requires (Self::IsDynamic())
{
    if (InLength > 0)
    {
        this->Impl.Pop(); /* Terminator */
        this->Impl.Append(InString, InLength);
        this->Impl.Add(Traits::Terminator);
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::AppendAt(const SizeType InRuneIndex, const Self& InOther) noexcept requires (Self::IsDynamic())
{
    if (const SizeType OtherRuneCount = InOther.GetRuneCount(); OtherRuneCount > 0)
    {
        this->Impl.Pop(); /* Terminator */
        this->Impl.AppendAt(InRuneIndex, InOther.ToPtr(), OtherRuneCount);
        this->Impl.Add(Traits::Terminator);
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::AppendAt(const SizeType InRuneIndex, const T* InString) noexcept requires (Self::IsDynamic())
{
    const SizeType Length = Traits::template GetStringLength<SizeType>(InString);

    if (Length > 0)
    {
        this->Impl.Pop(); /* Terminator */
        this->Impl.AppendAt(InRuneIndex, InString, Length);
        this->Impl.Add(Traits::Terminator);
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::AppendAt(const SizeType InRuneIndex, const T* InString, const SizeType InLength) noexcept requires (Self::IsDynamic())
{
    if (InLength > 0)
    {
        this->Impl.Pop(); /* Terminator */
        this->Impl.AppendAt(InRuneIndex, InString, InLength);
        this->Impl.Add(Traits::Terminator);
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::RemoveAt(const SizeType InRuneIndex) noexcept requires (Self::IsDynamic())
{
    JAFG_CHECK_STRING( this->IsValidIndex(InRuneIndex) )
    this->Impl.RemoveAt(InRuneIndex);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::RemoveAt(const SizeType InRuneIndex, const SizeType InLength) noexcept requires (Self::IsDynamic())
{
    JAFG_CHECK_STRING( this->IsValidIndex(InRuneIndex + InLength - 1) )
    this->Impl.RemoveAt(InRuneIndex, InLength);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::RemoveCharacterAt(const SizeType InRuneIndex) noexcept requires (Self::IsDynamic())
{
    JAFG_CHECK_STRING( this->IsValidIndex(InRuneIndex) )
    const SizeType CharacterSize = Traits::template GetCharacterSize<SizeType>(this->Impl.GetData() + InRuneIndex);
    this->Impl.RemoveAt(InRuneIndex, CharacterSize);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return CharacterSize;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::RemoveCharacterAt(const SizeType InRuneIndex, const SizeType InCharacters) noexcept requires (Self::IsDynamic())
{
    JAFG_CHECK_STRING( this->IsValidIndex(InRuneIndex) )

    SizeType Cursor = InRuneIndex;
    SizeType TotalRunes = 0;
    for (SizeType Character = 0; Character < InCharacters; ++Character)
    {
        SizeType CharacterSize = Traits::template GetCharacterSize<SizeType>(this->Impl.GetData() + Cursor);
        TotalRunes += CharacterSize;
        Cursor += CharacterSize;
    }

    this->RemoveAt(InRuneIndex, TotalRunes);

    return TotalRunes;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE const typename TStringBase<Derived, InTraits, InAlloc>::T* TStringBase<Derived, InTraits, InAlloc>::Peek() const noexcept
{
    PRIVATE_JAFG_CHECK_STRING_STATE()
    return this->Impl.GetSize() > 1 ? this->Impl.GetData() + this->GetRuneCount() - 1 : &Traits::Terminator;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::Pop() noexcept requires (Self::IsDynamic())
{
    const bool bOut = this->GetSize() > 1;

    this->Impl.Pop(2); /* Terminator */

    if (this->GetSize() > 0)
    {
        this->Impl.Add(Traits::Terminator);
    }
    else
    {
        this->Empty();
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return bOut;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::PopCharacter() noexcept requires (Self::IsDynamic())
{
    const bool bOut = this->GetSize() > 1;

    this->Impl.Pop();
    SizeType Cursor = this->GetSize() - 1;
    Traits::GoToMostRecentValidCharacter(this->Impl.GetData(), &Cursor);
    Traits::GoToMostRecentValidCharacter(this->Impl.GetData(), &--Cursor);

    if (Cursor > INDEX_NONE)
    {
        SizeType CurCharacterSize = Traits::template GetCharacterSize<SizeType>(this->Impl.GetData() + Cursor);
        SizeType NewStrSize = Cursor + CurCharacterSize;
        this->Impl.Pop(this->GetSize() - NewStrSize - 1); /* Terminator */
        this->Impl[NewStrSize] = Traits::Terminator;
    }
    else
    {
        this->Impl.Empty();
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()

    return bOut;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::Pop(const SizeType InRuneCount) noexcept requires (Self::IsDynamic())
{
    this->Impl.Pop(InRuneCount + 1); /* Terminator */

    if (this->GetSize() > 0)
    {
        this->Impl.Add(Traits::Terminator);
    }
    else
    {
        this->Empty();
    }

    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::PopCharacter(SizeType InCharacterCount) noexcept requires (Self::IsDynamic())
{
    while (InCharacterCount > 0)
    {
        if (this->PopCharacter())
        {
            --InCharacterCount;
            continue;
        }

        break;
    }

    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::StartsWith(const T InRune) const noexcept
{
    return *this->ToPtr() == InRune;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::StartsWith(const T* InString) const noexcept
{
    return this->StartsWith(InString, Traits::template GetStringLength<SizeType>(InString));
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::StartsWith(const T* InString, const SizeType InLength) const noexcept
{
    if (*InString == Traits::Terminator)
    {
        return this->StartsWith(Traits::Terminator);
    }

    SizeType Cursor = 0;
    while (*InString != Traits::Terminator && Cursor < InLength)
    {
        if (this->GetRuneCount() <= Cursor)
        {
            return false;
        }

        if (*(this->Impl.GetData() + Cursor++) != *InString)
        {
            return false;
        }

        ++InString;

        continue;
    }

    return Cursor <= this->GetRuneCount();}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::StartsWith(const Self& InOther) const noexcept
{
    return this->StartsWith(InOther.ToPtr(), InOther.GetRuneCount());
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::StartsWith(const Self& InOther, const SizeType InLength) const noexcept
{
    return this->StartsWith(InOther.ToPtr(), InLength);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::StartsWith(const T* InString, const T* InEnd, const T* InSubString, const T* InSubEnd) noexcept
{
    check( InString && InEnd && InSubString && InSubEnd )

    if (InSubString == InSubEnd)
    {
        return true;
    }

    while (InString != InEnd)
    {
        if (*InString != *InSubString)
        {
            return false;
        }

        ++InString;
        ++InSubString;

        if (InSubString == InSubEnd)
        {
            return true;
        }

        continue;
    }

    return false;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::EndsWith(const T InRune) const noexcept
{
    return *this->Peek() == InRune;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::EndsWith(const T* InString) const noexcept
{
    return this->EndsWith(InString, Traits::template GetStringLength<SizeType>(InString));
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::EndsWith(const T* InString, const SizeType InLength) const noexcept
{
    SizeType Cursor = 0;
    const SizeType StringSize = this->GetSize();
    while (Cursor < StringSize)
    {
        if (*(this->Impl.GetSlack() - 1 - Cursor) != *(InString + InLength - Cursor))
        {
            return false;
        }

        if (Cursor == InLength)
        {
            return true;
        }

        ++Cursor;

        continue;
    }

    return Cursor == StringSize;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::EndsWith(const Self& InOther) const noexcept
{
    return this->EndsWith(InOther.ToPtr(), InOther.GetRuneCount());
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::EndsWith(const Self& InOther, const SizeType InLength) const noexcept
{
    return this->EndsWith(InOther.ToPtr(), InLength);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T InRune) const noexcept
{
    return Self::FindFirst(this->GetBegin(), this->GetEnd(), InRune);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InString, const T* InEnd, const T InRune) noexcept
{
    check( InString <= InEnd )

    const T* Cursor = InString;
    while (Cursor != InEnd)
    {
        if (*Cursor == InRune)
        {
            return Cursor - InString;
        }

        ++Cursor;
    }

    return INDEX_NONE;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InString, const SizeType InLength, const T InRune) noexcept
{
    return Self::FindFirst(InString, InString + InLength, InRune);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InSubString) const noexcept
{
    return Self::FindFirst(this->GetBegin(), this->GetEnd(), InSubString, InSubString + Self::Traits::template GetStringLength<SizeType>(InSubString));
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InString, const T* InEnd, const T* InSubString) noexcept
{
    return Self::FindFirst(InString, InEnd, InSubString, InSubString + Traits::template GetStringLength<SizeType>(InSubString));
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InString, const SizeType InLength, const T* InSubString) noexcept
{
    return Self::FindFirst(InString, InString + InLength, InSubString, InSubString + Self::Traits::template GetStringLength<SizeType>(InSubString));
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InSubString, const T* InSubEnd) const noexcept
{
    return Self::FindFirst(this->GetBegin(), this->GetEnd(), InSubString, InSubEnd);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InString, const T* InEnd, const T* InSubString, const T* InSubEnd) noexcept
{
    JAFG_CHECK_STRING( InString <= InEnd )
    JAFG_CHECK_STRING( InSubString <= InSubEnd )

    const T* MainCursor = InString;
    const SizeType SubLength = InSubEnd - InSubString;
    while (MainCursor != InEnd)
    {
        JAFG_CHECK_STRING( *MainCursor != Self::Traits::Terminator )

        if (static_cast<SizeType>(InEnd - MainCursor) < SubLength)
        {
            return INDEX_NONE;
        }

        const T* SubCursor = InSubString;
        while (SubCursor != InSubEnd)
        {
            JAFG_CHECK_STRING( *SubCursor != Self::Traits::Terminator )

            if (*(MainCursor + (SubCursor - InSubString)) != *SubCursor)
            {
                break;
            }

            ++SubCursor;
            continue;
        }

        if (SubCursor == InSubEnd)
        {
            return MainCursor - InString;
        }

        ++MainCursor;
        continue;
    }

    return INDEX_NONE;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InString, const SizeType InLength, const T* InSubString, const T* InSubEnd) noexcept
{
    return Self::FindFirst(InString, InString + InLength, InSubString, InSubEnd);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InSubString, const SizeType InSubLength) const noexcept
{
    return Self::FindFirst(this->GetBegin(), this->GetEnd(), InSubString, InSubString + InSubLength);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InString, const T* InEnd, const T* InSubString, const SizeType InSubLength) noexcept
{
    return Self::FindFirst(InString, InEnd, InSubString, InSubString + InSubLength);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InString, const SizeType InLength, const T* InSubString, const SizeType InSubLength) noexcept
{
    return Self::FindFirst(InString, InString + InLength, InSubString, InSubString + InSubLength);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const Self& InSubString) const noexcept
{
    return Self::FindFirst(this->GetBegin(), this->GetEnd(), InSubString.GetBegin(), InSubString.GetEnd());
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InString, const T* InEnd, const Self& InSubString) noexcept
{
    return Self::FindFirst(InString, InEnd, InSubString.GetBegin(), InSubString.GetEnd());
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InString, const SizeType InLength, const Self& InSubString) noexcept
{
    return Self::FindFirst(InString, InString + InLength, InSubString.GetBegin(), InSubString.GetEnd());
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const Self& InSubString, const T* InSubEnd) const noexcept
{
    check( InSubString.GetEnd() >= InSubEnd )
    return Self::FindFirst(this->GetBegin(), this->GetEnd(), InSubString.GetBegin(), InSubEnd);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InString, const T* InEnd, const Self& InSubString, const T* InSubEnd) noexcept
{
    check( InSubString.GetEnd() >= InSubEnd )
    return Self::FindFirst(InString, InEnd, InSubString.GetBegin(), InSubEnd);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InString, const SizeType InLength, const Self& InSubString, const T* InSubEnd) noexcept
{
    check( InSubString.GetEnd() >= InSubEnd )
    return Self::FindFirst(InString, InString + InLength, InSubString.GetBegin(), InSubEnd);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const Self& InSubString, const SizeType InSubLength) const noexcept
{
    check( InSubString.GetRuneCount() >= InSubLength )
    return Self::FindFirst(this->GetBegin(), this->GetEnd(), InSubString.GetBegin(), InSubString.GetEnd() + InSubLength);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InString, const T* InEnd, const Self& InSubString, const SizeType InSubLength) noexcept
{
    check( InSubString.GetRuneCount() >= InSubLength )
    return Self::FindFirst(InString, InEnd, InSubString.GetBegin(), InSubString.GetBegin() + InSubLength);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindFirst(const T* InString, const SizeType InLength, const Self& InSubString, const SizeType InSubLength) noexcept
{
    check( InSubString.GetRuneCount() >= InSubLength )
    return Self::FindFirst(InString, InString + InLength, InSubString.GetBegin(), InSubString.GetBegin() + InSubLength);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindSecond(const T InRune) const noexcept
{
    bool bFound = false;

    const SizeType Size = this->GetSize();
    for (SizeType Index = 0; Index < Size; ++Index)
    {
        if (*(this->Impl.GetData() + Index) == InRune)
        {
            if (bFound)
            {
                return Index;
            }
            else
            {
                bFound = true;
            }
        }

        continue;
    }

    return INDEX_NONE;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindSecond(const T* InString) const noexcept
{
    return this->FindSecond(InString, Traits::template GetStringLength<SizeType>(InString));
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindSecond(const T* InString, const SizeType InLength) const noexcept
{
    bool bFound = false;

    SizeType MainCursor = 0;
    SizeType MainStringSize = this->GetSize();
    while (MainCursor < MainStringSize)
    {
        SizeType SubCursor  = 0;

        while (SubCursor < InLength)
        {
            JAFG_CHECK_STRING(*(InString + SubCursor) != Traits::Terminator)

            if (SubCursor == MainStringSize)
            {
                return INDEX_NONE;
            }

            if (*(this->Impl.GetData() + MainCursor + SubCursor) != *(InString + SubCursor))
            {
                break;
            }

            ++SubCursor;
            if (SubCursor == InLength)
            {
                if (bFound)
                {
                    return MainCursor;
                }
                else
                {
                    bFound = true;
                }
            }

            continue;
        }

        ++MainCursor;
        continue;
    }

    return INDEX_NONE;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindSecond(const Self& InOther) const noexcept
{
    return this->FindSecond(InOther.ToPtr(), InOther.GetRuneCount());
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindSecond(const Self& InOther, const SizeType InLength) const noexcept
{
    return this->FindSecond(InOther.ToPtr(), InLength);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindLast(const T InRune) const noexcept
{
    for (SizeType Index = this->GetSize() - 1; Index > INDEX_NONE; --Index)
    {
        if (*(this->Impl.GetData() + Index) == InRune)
        {
            return Index;
        }

        continue;
    }

    return INDEX_NONE;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindLast(const T* InString) const noexcept
{
    return this->FindLast(InString, Traits::template GetStringLength<SizeType>(InString));
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindLast(const T* InString, const SizeType InLength) const noexcept
{
    // This is not efficient at all, so if we use this frequently in larger strings,
    // we must revisit this.
    // Currently only very sparingly used in the codebase - so it doesn't fucking matter.

    SizeType Last = INDEX_NONE;

    SizeType MainCursor = 0;
    SizeType MainStringSize = this->GetSize();
    while (MainCursor < MainStringSize)
    {
        SizeType SubCursor  = 0;

        while (SubCursor < InLength)
        {
            JAFG_CHECK_STRING(*(InString + SubCursor) != Traits::Terminator)

            if (SubCursor == MainStringSize)
            {
                return Last;
            }

            if (*(this->Impl.GetData() + MainCursor + SubCursor) != *(InString + SubCursor))
            {
                break;
            }

            ++SubCursor;
            if (SubCursor == InLength)
            {
                Last = MainCursor;
                break;
            }

            continue;
        }

        ++MainCursor;
        continue;
    }

    return Last;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindLast(const Self& InOther) const noexcept
{
    return this->FindLast(InOther.ToPtr(), InOther.GetRuneCount());
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindLast(const Self& InOther, const SizeType InLength) const noexcept
{
    return this->FindLast(InOther.ToPtr(), InLength);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindSecondLast(const T InRune) const noexcept
{
    bool bFound = false;

    for (SizeType Index = this->GetSize() - 1; Index > INDEX_NONE; --Index)
    {
        if (*(this->Impl.GetData() + Index) == InRune)
        {
            if (bFound)
            {
                return Index;
            }

            bFound = true;
        }

        continue;
    }

    return INDEX_NONE;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindSecondLast(const T* InString) const noexcept
{
    return this->FindSecondLast(InString, Traits::template GetStringLength<SizeType>(InString));
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindSecondLast(const T* InString, const SizeType InLength) const noexcept
{
    // This is not efficient at all, so if we use this frequently in larger strings,
    // we must revisit this.
    // Currently only very sparingly used in the codebase - so it doesn't fucking matter.

    bool bFound = false;

    SizeType Last = INDEX_NONE;

    SizeType MainCursor = 0;
    SizeType MainStringSize = this->GetSize();
    while (MainCursor < MainStringSize)
    {
        SizeType SubCursor  = 0;

        while (SubCursor < InLength)
        {
            JAFG_CHECK_STRING(*(InString + SubCursor) != Traits::Terminator)

            if (SubCursor == MainStringSize)
            {
                if (bFound)
                {
                    return Last;
                }

                bFound = true;
                break;
            }

            if (*(this->Impl.GetData() + MainCursor + SubCursor) != *(InString + SubCursor))
            {
                break;
            }

            ++SubCursor;
            if (SubCursor == InLength)
            {
                Last = MainCursor;
                break;
            }

            continue;
        }

        ++MainCursor;
        continue;
    }

    return Last;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindSecondLast(const Self& InOther) const noexcept
{
    return this->FindSecondLast(InOther.ToPtr(), InOther.GetRuneCount());
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::FindSecondLast(const Self& InOther, const SizeType InLength) const noexcept
{
    return this->FindSecondLast(InOther.ToPtr(), InLength);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::Contains(const T InRune) const noexcept
{
    return this->FindFirst(InRune) != INDEX_NONE;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::Contains(const T* InString) const noexcept
{
    return this->Contains(InString, Traits::template GetStringLength<SizeType>(InString));
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::Contains(const T* InString, const SizeType InLength) const noexcept
{
    return this->FindFirst(InString, InLength) != INDEX_NONE;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::Contains(const Self& InOther) const noexcept
{
    return this->Contains(InOther.ToPtr(), InOther.GetRuneCount());
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<Derived, InTraits, InAlloc>::Contains(const Self& InOther, const SizeType InLength) const noexcept
{
    return this->Contains(InOther.ToPtr(), InLength);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::InlineCut(const SizeType InRuneIndex) noexcept requires (Self::IsDynamic())
{
    this->Impl.Resize(InRuneIndex + /*Terminator*/1, true);
    this->Impl[InRuneIndex] = Traits::Terminator;

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self TStringBase<Derived, InTraits, InAlloc>::Cut(const SizeType InRuneIndex) const noexcept requires (Self::IsStrongAlloc())
{
    Self Out;
    Out.Impl.CopyFrom(this->Impl, InRuneIndex + 1);
    Out.Impl[InRuneIndex] = Traits::Terminator;

#if LAL_CHECK_STRING_VALIDITY
    Out.EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */
    return Out;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self TStringBase<Derived, InTraits, InAlloc>::Cut(const SizeType InRuneIndex) const noexcept requires (Self::IsWeakAlloc())
{
    Self Out;
    Out.Impl.GetUnderlyingDataStructure().Data  = this->GetBegin();
    Out.Impl.GetUnderlyingDataStructure().Slack = this->GetBegin() + InRuneIndex;
    Out.Impl.GetUnderlyingDataStructure().End   = this->GetBegin() + InRuneIndex;

#if LAL_CHECK_STRING_VALIDITY
    Out.EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */
    return Out;
}

template<typename Derived, typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE TOtherString TStringBase<Derived, InTraits, InAlloc>::Cut(const SizeType InRuneIndex) const noexcept requires (Self::template IsValidOtherString<TOtherString>())
{
    TOtherString Out;

    if constexpr (TOtherString::IsStrongAlloc())
    {
        Out.GetUnderlyingDataStructure().CopyFrom(this->Impl, InRuneIndex + 1);
        Out.GetUnderlyingDataStructure()[InRuneIndex] = Traits::Terminator;
    }
    else
    {
        Out.GetUnderlyingDataStructure().GetUnderlyingDataStructure().Data  = this->GetBegin();
        Out.GetUnderlyingDataStructure().GetUnderlyingDataStructure().Slack = this->GetBegin() + InRuneIndex;
        Out.GetUnderlyingDataStructure().GetUnderlyingDataStructure().End   = this->GetBegin() + InRuneIndex;
    }

#if LAL_CHECK_STRING_VALIDITY
    Out.EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */
    return Out;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::InlineSub(const SizeType InRuneStartIndex, const SizeType InRuneCount) noexcept requires (Self::IsDynamic())
{
    *this = this->Sub(InRuneStartIndex, InRuneCount);
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::InlineSubIdx(const SizeType InRuneStartIndex, const SizeType InRuneEndIndex) noexcept requires (Self::IsDynamic())
{
    this->InlineSub(InRuneStartIndex, InRuneEndIndex - InRuneStartIndex);
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self TStringBase<Derived, InTraits, InAlloc>::Sub(const SizeType InRuneStartIndex, const SizeType InRuneCount) const noexcept requires (Self::IsStrongAlloc())
{
    JAFG_CHECK_STRING( InRuneCount > INDEX_NONE )

    Self Out;

    if (InRuneCount > 0)
    {
        Out.Reserve(InRuneCount + /*Terminator*/1);
        Out.Impl.CopyFrom(this->Impl, InRuneStartIndex, InRuneCount);
        Out.Impl.Add(Traits::Terminator);
    }

#if LAL_CHECK_STRING_VALIDITY
    Out.EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */
    return Out;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self TStringBase<Derived, InTraits, InAlloc>::Sub(const SizeType InRuneStartIndex, const SizeType InRuneCount) const noexcept requires (Self::IsWeakAlloc())
{
    JAFG_CHECK_STRING( InRuneCount > INDEX_NONE )

    Self Out;

    if (InRuneCount > 0)
    {
        Out.Impl.GetUnderlyingDataStructure().Data  = this->GetBegin() + InRuneStartIndex;
        Out.Impl.GetUnderlyingDataStructure().Slack = this->GetBegin() + InRuneStartIndex + InRuneCount;
        Out.Impl.GetUnderlyingDataStructure().End   = this->GetBegin() + InRuneStartIndex + InRuneCount;
    }

#if LAL_CHECK_STRING_VALIDITY
    Out.EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */
    return Out;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self TStringBase<Derived, InTraits, InAlloc>::SubIdx(const SizeType InRuneStartIndex, const SizeType InRuneEndIndex) const noexcept
{
    return this->Sub(InRuneStartIndex, InRuneEndIndex - InRuneStartIndex);
}

template<typename Derived, typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE TOtherString TStringBase<Derived, InTraits, InAlloc>::Sub(const SizeType InRuneStartIndex, const SizeType InRuneCount) const noexcept requires (Self::template IsValidOtherString<TOtherString>())
{
    JAFG_CHECK_STRING( InRuneCount > INDEX_NONE )

    TOtherString Out;

    if (InRuneCount > 0)
    {
        if constexpr (TOtherString::IsStrongAlloc())
        {
            Out.Reserve(InRuneCount + /*Terminator*/1);
            Out.GetUnderlyingDataStructure().CopyFrom(this->Impl, InRuneStartIndex, InRuneCount);
            Out.GetUnderlyingDataStructure().Add(Traits::Terminator);
        }
        else
        {
            Out.GetUnderlyingDataStructure().GetUnderlyingDataStructure().Data  = this->GetBegin() + InRuneStartIndex;
            Out.GetUnderlyingDataStructure().GetUnderlyingDataStructure().Slack = this->GetBegin() + InRuneStartIndex + InRuneCount;
            Out.GetUnderlyingDataStructure().GetUnderlyingDataStructure().End   = this->GetBegin() + InRuneStartIndex + InRuneCount;
        }
    }

#if LAL_CHECK_STRING_VALIDITY
    Out.EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */
    return Out;
}

template<typename Derived, typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE TOtherString TStringBase<Derived, InTraits, InAlloc>::SubIdx(const SizeType InRuneStartIndex, const SizeType InRuneEndIndex) const noexcept requires (Self::template IsValidOtherString<TOtherString>())
{
    return this->Sub<TOtherString>(InRuneStartIndex, InRuneEndIndex - InRuneStartIndex);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::InlineRightChop(const SizeType InRuneIndex) noexcept requires (Self::IsDynamic())
{
    this->InlineSub(InRuneIndex, this->GetRuneCount() - InRuneIndex);
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self TStringBase<Derived, InTraits, InAlloc>::RightChop(const SizeType InRuneIndex) const noexcept
{
    return this->Sub(InRuneIndex, this->GetRuneCount() - InRuneIndex);
}

template<typename Derived, typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE TOtherString TStringBase<Derived, InTraits, InAlloc>::RightChop(const SizeType InRuneIndex) const noexcept requires (Self::template IsValidOtherString<TOtherString>())
{
    return this->Sub<TOtherString>(InRuneIndex, this->GetRuneCount() - InRuneIndex);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::InlineLeftChop(const SizeType InRuneIndex) noexcept requires (Self::IsDynamic())
{
    this->InlineCut(InRuneIndex);
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self TStringBase<Derived, InTraits, InAlloc>::LeftChop(const SizeType InRuneIndex) const noexcept
{
    return this->Cut(InRuneIndex);
}

template<typename Derived, typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE TOtherString TStringBase<Derived, InTraits, InAlloc>::LeftChop(const SizeType InRuneIndex) const noexcept requires (Self::template IsValidOtherString<TOtherString>())
{
    return this->Cut<TOtherString>(InRuneIndex);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::Replace(const T InRune, const T InReplacement) noexcept requires (Self::IsContentMutable())
{
    SizeType Out = 0;

    for (T* RESTRICT Bulk = this->Impl.GetData(), *RESTRICT BulkEnd = this->Impl.GetSlack(); Bulk != BulkEnd; ++Bulk)
    {
        if (*Bulk == InRune)
        {
            *Bulk = InReplacement;
            ++Out;
        }

        continue;
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return Out;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::Replace(const T* InString, const T* InReplacement) noexcept requires (Self::IsContentMutable())
{
    SizeType Out = 0;

    while (true)
    {
        const SizeType Where = this->FindFirst(InString);
        if (Where == INDEX_NONE)
        {
            break;
        }

        this->RemoveAt(Where, Traits::template GetStringLength<SizeType>(InString));
        this->AppendAt(Where, InReplacement);
        ++Out;

        continue;
    }

    return Out;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::Replace(const T* InString, const T* InEnd, const T* InReplacement, const T* InReplacementEnd) noexcept
    requires (Self::IsContentMutable())
{
    SizeType Out = 0;

    while (true)
    {
        const SizeType Where = this->FindFirst(InString, InEnd);
        if (Where == INDEX_NONE)
        {
            break;
        }

        this->RemoveAt(Where, InEnd - InString);
        this->AppendAt(Where, InReplacement, InReplacementEnd - InReplacement);
        ++Out;

        continue;
    }

    return Out;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::Substitute(T* InStart, T* InEnd, const T* InReplacement, const T* InReplacementEnd) noexcept requires (Self::IsDynamic())
{
    check( InStart <= InEnd )
    check( InStart >= this->GetBegin() && InEnd < this->GetEnd() )
    check( InReplacement <= InReplacementEnd )
    check( InReplacement < this->GetBegin() || InReplacementEnd > this->Impl.GetUnderlyingDataStructure().End )

    const SizeType DeltaSize = (InReplacementEnd - InReplacement) - (InEnd - InStart);
    if (DeltaSize == 0)
    {
        ::memcpy(InStart, InReplacement, InReplacementEnd - InReplacement);
        return;
    }

    const SizeType InRelStart = InStart - this->GetBegin();
    const SizeType InRelEnd   = InEnd   - this->GetBegin();
    check( InRelStart >= 0 && InRelEnd <= this->GetSize() )

    this->Impl.Reserve(this->GetSize() + DeltaSize);

    ::memmove(this->GetBegin() + InRelEnd + DeltaSize, this->GetBegin() + InRelEnd, this->Impl.GetUnderlyingDataStructure().Slack - this->GetBegin() - InRelEnd);
    ::memcpy(this->GetBegin() + InRelStart, InReplacement, InReplacementEnd - InReplacement);

    this->Impl.GetUnderlyingDataStructure().Slack += DeltaSize;

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::Count(const T InRune) const noexcept
{
    SizeType Out = 0;

    for (const T* RESTRICT Bulk = this->Impl.GetData(), *RESTRICT BulkEnd = this->Impl.GetSlack(); Bulk != BulkEnd; ++Bulk)
    {
        if (*Bulk == InRune)
        {
            ++Out;
        }

        continue;
    }

    return Out;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::Count(const T* InString) const noexcept
{
    return this->Count(InString, Traits::template GetStringLength<SizeType>(InString));
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::Count(const T* InString, const SizeType InLength) const noexcept
{
    SizeType Out = 0;

    SizeType MainCursor = 0;
    SizeType MainStringSize = this->GetSize();
    while (MainCursor < MainStringSize)
    {
        SizeType SubCursor  = 0;

        while (SubCursor < InLength)
        {
            JAFG_CHECK_STRING(*(InString + SubCursor) != Traits::Terminator)

            if (SubCursor == MainStringSize)
            {
                return Out;
            }

            if (*(this->Impl.GetData() + MainCursor + SubCursor) != *(InString + SubCursor))
            {
                break;
            }

            ++SubCursor;
            if (SubCursor == InLength)
            {
                ++Out;
                break;
            }

            continue;
        }

        ++MainCursor;
        continue;
    }

    return Out;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::Count(const Self& InOther) const noexcept
{
    return this->Count(InOther.ToPtr(), InOther.GetRuneCount());
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::Count(const Self& InOther, const SizeType InLength) const noexcept
{
    return this->Count(InOther.ToPtr(), InLength);
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<Derived, InTraits, InAlloc>::ToLower() noexcept requires (Self::IsContentMutable())
{
    for (T* RESTRICT Bulk = this->Impl.GetData(), *RESTRICT BulkEnd = this->Impl.GetSlack(); Bulk != BulkEnd; ++Bulk)
    {
        *Bulk = Traits::ToLower(*Bulk);
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::Self TStringBase<Derived, InTraits, InAlloc>::GetLowerCase() const noexcept requires (Self::IsStrongAlloc())
{
    Self Out; Out.Reserve(this->GetSize());

    for (const T* RESTRICT Bulk = this->Impl.GetData(), *RESTRICT BulkEnd = this->Impl.GetSlack(); Bulk != BulkEnd; ++Bulk)
    {
        Out.Impl.Add(Traits::ToLower(*Bulk));
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return Out;
}

template<typename Derived, typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE TOtherString TStringBase<Derived, InTraits, InAlloc>::GetLowerCase() const noexcept requires (TOtherString::IsStrongAlloc() && Self::template IsValidOtherString<TOtherString>())
{
    TOtherString Out; Out.Reserve(this->GetSize());

    for (const T* RESTRICT Bulk = this->Impl.GetData(), *RESTRICT BulkEnd = this->Impl.GetSlack(); Bulk != BulkEnd; ++Bulk)
    {
        Out.GetUnderlyingDataStructure().Add(Traits::ToLower(*Bulk));
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return Out;
}

template<typename Derived, typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<Derived, InTraits, InAlloc>::SizeType TStringBase<Derived, InTraits, InAlloc>::GetLineNumber(const SizeType InRuneIndex) const
{
    SizeType Out = 0;
    SizeType Cursor = 0;
    while (Cursor <= this->GetSize())
    {
        if (this->operator[](Cursor) == '\n')
        {
            ++Out;
        }

        ++Cursor;
        continue;
    }

    return Out;
}

template<typename Derived, typename InTraits, typename InAlloc>
template<typename... TArgs>
typename TStringBase<Derived, InTraits, InAlloc>::Self TStringBase<Derived, InTraits, InAlloc>::SprintF(const T* InFormat, const TArgs&... InArgs) noexcept requires (Self::IsStrongAlloc())
{
    /**
     * Super supid solution. But who cares right now.
     * Later we write our own implementation with type safety etc.
     *
     * S will be stackallocated if in str is small enough. So its not that bad.
     * But still one unnecessary heap allocation by this class...
     */
    const LStringLegacy S = std::vformat(InFormat, std::make_format_args(InArgs...));
    Self Out = S.c_str();
    return Out;
}

#if LAL_CHECK_STRING_VALIDITY
template<typename Derived, typename InTraits, typename InAlloc>
void TStringBase<Derived, InTraits, InAlloc>::EnsureValidState() const
{
    for (const T *RESTRICT Bulk = this->Impl.GetData(), *RESTRICT BulkEnd = this->Impl.GetSlack(); Bulk != BulkEnd; ++Bulk)
    {
        if constexpr (Self::IsStrongAlloc())
        {
            if (*Bulk == Traits::Terminator && Bulk + 1 != BulkEnd)
            {
                panic("String contains a null terminator but is not the last character.")
            }
        }
        else
        {
            if (*Bulk == Traits::Terminator)
            {
                panic("Weak string contains a null terminator.")
            }
        }

        continue;
    }

    if constexpr (Self::IsStrongAlloc())
    {
        if (this->Impl.IsEmpty() == false)
        {
            if (*this->Impl.Peek() != Traits::Terminator)
            {
                panic("String does not end with a null terminator.")
            }
        }
    }

    return;
}
#endif /* LAL_CHECK_STRING_VALIDITY */

template<typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE typename _TStringBase<InTraits, InAlloc>::Self& _TStringBase<InTraits, InAlloc>::operator=(const TOtherString& Other) noexcept
    requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>())
{
    return this->Super::operator=(Other);
}

template<typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE typename _TStringBase<InTraits, InAlloc>::Self& _TStringBase<InTraits, InAlloc>::operator=(const TOtherString& Other) noexcept
    requires (Self::IsStrongAlloc() && Self::template IsValidOtherString<TOtherString>())
{
    return this->Super::operator=(Other);
}

template<typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE typename _TStringBase<InTraits, InAlloc>::Self& _TStringBase<InTraits, InAlloc>::operator=(TOtherString&& InString) noexcept
    requires (Self::IsWeakAlloc() && TOtherString::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>())
{
    return this->Super::operator=(std::move(InString));
}

template<typename InTraits, typename InAlloc>
template<typename TOtherString>
FORCEINLINE typename _TStringBase<InTraits, InAlloc>::Self& _TStringBase<InTraits, InAlloc>::operator=(TOtherString&& InString) noexcept
    requires (Self::IsStrongAlloc() && TOtherString::IsStrongAlloc() && Self::template IsValidOtherString<TOtherString>())
{
    return this->Super::operator=(std::move(InString));
}

} /* ~Namespace Jafg */

#undef PRIVATE_JAFG_CHECK_STRING_STATE
#undef JAFG_CHECK_STRING

template <>
struct std::formatter<::Jafg::LString> : std::formatter<const char*>
{
    FORCEINLINE auto format
    (
        const ::Jafg::LString& InString,
        ::std::format_context& InContext
    ) const -> ::std::format_context::iterator
    {
        return ::std::formatter<const char*>::format(InString.ToPtr(), InContext);
    }
};

template <>
struct std::formatter<::Jafg::LStringView> : std::formatter<::std::string_view>
{
    FORCEINLINE auto format
    (
        const ::Jafg::LStringView& InStringView,
        ::std::format_context& InContext
    ) const -> ::std::format_context::iterator
    {
        return ::std::formatter<::std::string_view>::format(::std::string_view(InStringView.GetBegin(), InStringView.GetEnd()), InContext);
    }
};
