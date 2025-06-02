// Copyright mzoesch. All rights reserved.

#pragma once

#include "SystemForward.h"
#include "Finder.h"

namespace Jafg
{

class JUserPreferences;

//#
//# A path that is always relative to a content folder. The content folder that is used is then determined when
//# resolving the absolute path. This allows the user to add their own plugin content folders that have a higher
//# priority than the engine content folder. Absolute paths will then be redirected to the plugin content folder.
//#
template <typename InTPathTy>
class LEnginePathBase final
{
public:

     using Self = LEnginePathBase<InTPathTy>;

     using Alloc     = InTPathTy;
     using T         = typename Alloc::T;
     using SizeType  = typename Alloc::SizeType;

    FORCEINLINE  LEnginePathBase() noexcept = default;
    FORCEINLINE  LEnginePathBase(LNullptrTy) noexcept { }
    FORCEINLINE  LEnginePathBase(const Self& Other) noexcept { this->PathTy = Other.PathTy; this->Impl = Other.Impl; }
    FORCEINLINE  LEnginePathBase(Self&& Other) noexcept { this->PathTy = Other.PathTy; this->Impl = std::move(Other.Impl); }
    FORCEINLINE  LEnginePathBase(const Alloc& Other) noexcept;
    FORCEINLINE  LEnginePathBase(Alloc&& Other) noexcept;
    FORCEINLINE  LEnginePathBase(const EEnginePaths::Type InPathTy, const Alloc& Other) noexcept : PathTy(InPathTy), Impl(Other) { }
    FORCEINLINE  LEnginePathBase(const EEnginePaths::Type InPathTy, Alloc&& Other) noexcept : PathTy(InPathTy), Impl(std::move(Other)) { }
    FORCEINLINE  LEnginePathBase(const EEnginePaths::Type InPathTy) noexcept : PathTy(InPathTy) { }
    FORCEINLINE  LEnginePathBase(const EEnginePaths::Type InPathTy, LNullptrTy) noexcept : PathTy(InPathTy) { }
    FORCEINLINE  LEnginePathBase(const EEnginePaths::Type InPathTy, const T* InPath) noexcept : PathTy(InPathTy), Impl(InPath) { }
    FORCEINLINE ~LEnginePathBase() noexcept = default;

    FORCEINLINE constexpr void SetPathTy(const EEnginePaths::Type InPathTy) noexcept { this->PathTy = InPathTy; }
    FORCEINLINE constexpr auto GetPathTy() const noexcept -> EEnginePaths::Type { return this->PathTy; }

    FORCEINLINE auto GetSize() const noexcept -> SizeType { return this->Impl.GetSize(); }
    FORCEINLINE bool IsEmpty() const noexcept { return this->Impl.IsEmpty(); }
    FORCEINLINE void Reset(const SizeType InReserve) noexcept { this->Impl.Reset(InReserve); }
    FORCEINLINE void Empty() noexcept { this->Impl.Empty(); }

    FORCEINLINE void Reserve(const SizeType Size) noexcept { this->Impl.Reserve(Size); }

    FORCEINLINE Self& operator=(const Self& Other) noexcept = default;

    FORCEINLINE Self& operator=(Self&& Other) noexcept;
    FORCEINLINE Self& operator=(const Alloc& Other) noexcept;
    FORCEINLINE Self& operator=(Alloc&& Other) noexcept;

    FORCEINLINE bool operator==(const Self& Other) const noexcept { return this->Impl == Other.Impl;      }
    FORCEINLINE bool operator!=(const Self& Other) const noexcept { return this->Impl != Other.Impl;      }
    FORCEINLINE bool operator <(const Self& Other) const noexcept { return this->Impl  < Other.Impl;      }
    FORCEINLINE bool operator >(const Self& Other) const noexcept { return this->Impl  > Other.Impl;      }
    FORCEINLINE bool operator<=(const Self& Other) const noexcept { return this->Impl <= Other.Impl;      }
    FORCEINLINE bool operator>=(const Self& Other) const noexcept { return this->Impl >= Other.Impl;      }
    FORCEINLINE bool Equals(const Self& Other)     const noexcept { return this->Impl.Equals(Other.Impl); }

    FORCEINLINE const Alloc& GetRelativeUnresolvedPath() const noexcept { return this->Impl; }

    FORCEINLINE Alloc ResolveRelativePath() const;
    FORCEINLINE Alloc ResolveRelativeEnginePath(const JUserPreferences& InUserPreferences) const;
    FORCEINLINE Alloc ResolveAbsolutePath(const JUserPreferences& InUserPreferences) const;

    FORCEINLINE void AddExtension(const Alloc& InExtension) { this->Impl.AddExtension(InExtension); }

private:

    EEnginePaths::Type PathTy { EEnginePaths::None };
    Alloc Impl;
};

template <typename InTPathTy>
LEnginePathBase<InTPathTy>::LEnginePathBase(const Alloc& Other) noexcept
{
    this->Impl = Other;

    if (this->Impl.StartsWith(Finder::GetEngineRootDir()))
    {
        this->PathTy = EEnginePaths::CustomEngine;
        this->Impl.InlineRightChop(Finder::GetEngineRootDir().GetRuneCount()+1);
    }

    return;
}

template <typename InTPathTy>
LEnginePathBase<InTPathTy>::LEnginePathBase(Alloc&& Other) noexcept
{
    this->Impl = std::move(Other);

    if (this->Impl.StartsWith(Finder::GetEngineRootDir()))
    {
        this->PathTy = EEnginePaths::CustomEngine;
        this->Impl.InlineRightChop(Finder::GetEngineRootDir().GetRuneCount()+1);
    }

    return;
}

template<typename InTPathTy>
typename LEnginePathBase<InTPathTy>::Self& LEnginePathBase<InTPathTy>::operator=(Self&& Other) noexcept
{
    this->PathTy = Other.PathTy;
    this->Impl = std::move(Other.Impl);

    Other.PathTy = EEnginePaths::None;

    return *this;
}

template<typename InTPathTy>
typename LEnginePathBase<InTPathTy>::Self& LEnginePathBase<InTPathTy>::operator=(const Alloc& Other) noexcept
{
    this->Impl = Other;
    return *this;
}

template<typename InTPathTy>
typename LEnginePathBase<InTPathTy>::Self& LEnginePathBase<InTPathTy>::operator=(Alloc&& Other) noexcept
{
    this->Impl = std::move(Other);
    return *this;
}

template <typename InTPathTy>
FORCEINLINE typename LEnginePathBase<InTPathTy>::Alloc LEnginePathBase<InTPathTy>::ResolveRelativePath() const
{
    return Finder::ResolvePathToRelativeModulePath(*this);
}

template <typename InTPathTy>
FORCEINLINE typename LEnginePathBase<InTPathTy>::Alloc LEnginePathBase<InTPathTy>::ResolveRelativeEnginePath(const JUserPreferences& InUserPreferences) const
{
    return Finder::ResolvePathToRelativeEnginePath(*this, InUserPreferences);
}

template <typename InTPathTy>
typename LEnginePathBase<InTPathTy>::Alloc
FORCEINLINE LEnginePathBase<InTPathTy>::ResolveAbsolutePath(const JUserPreferences& InUserPreferences) const
{
    return Finder::ResolvePathToAbsolutePath(*this, InUserPreferences);
}

} /* ~Namespace Jafg */
