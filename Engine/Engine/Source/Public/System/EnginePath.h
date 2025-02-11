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

     using T         = InTPathTy;
     using TPathTy   = InTPathTy;
     using LPathTy   = TPathTy;
     using SizeType  = typename LPathTy::SizeType;
     using LStringTy = typename LPathTy::LStringTy;
     using LRune     = typename LPathTy::LRune;

    FORCEINLINE  LEnginePathBase() noexcept = default;
    FORCEINLINE  LEnginePathBase(LNullptrTy) noexcept { }
    FORCEINLINE  LEnginePathBase(const LEnginePathBase<T>& Other) noexcept { this->PathTy = Other.PathTy; this->Data = Other.Data; }
    FORCEINLINE  LEnginePathBase(LEnginePathBase<T>&& Other) noexcept { this->PathTy = Other.PathTy; this->Data = std::forward<LPathTy>(Other.Data); }
    FORCEINLINE  LEnginePathBase(const LPathTy& Other) noexcept;
     FORCEINLINE LEnginePathBase(LPathTy&& Other) noexcept;
     FORCEINLINE LEnginePathBase(const EEnginePaths::Type InPathTy, const LPathTy& Other) noexcept : PathTy(InPathTy), Data(Other) { }
    FORCEINLINE  LEnginePathBase(const EEnginePaths::Type InPathTy, LPathTy&& Other) noexcept : PathTy(InPathTy), Data(std::move(Other)) { }
    FORCEINLINE  LEnginePathBase(const EEnginePaths::Type InPathTy) noexcept : PathTy(InPathTy) { }
    FORCEINLINE  LEnginePathBase(const EEnginePaths::Type InPathTy, LNullptrTy) noexcept : PathTy(InPathTy) { }
    FORCEINLINE  LEnginePathBase(const EEnginePaths::Type InPathTy, const LRune* InPath) noexcept : PathTy(InPathTy), Data(InPath) { }
    FORCEINLINE ~LEnginePathBase() noexcept = default;

    FORCEINLINE auto SetPathTy(const EEnginePaths::Type InPathTy) noexcept -> void { this->PathTy = InPathTy; }
    FORCEINLINE auto GetPathTy() const noexcept -> EEnginePaths::Type { return this->PathTy; }

    FORCEINLINE auto GetSize() const noexcept -> SizeType { return this->Data.GetSize(); }
    FORCEINLINE auto IsEmpty() const noexcept -> bool     { return this->Data.IsEmpty(); }
    FORCEINLINE auto Reset(const SizeType InReserve) noexcept -> void { this->Data.Reset(InReserve); }
    FORCEINLINE auto Empty() noexcept -> void { this->Data.Empty(); }

    FORCEINLINE auto Reserve(const SizeType Size) noexcept -> void { this->Data.Reserve(Size); }

    FORCEINLINE auto operator=(const LEnginePathBase<T>& Other) noexcept -> LEnginePathBase<T>& = default;
    FORCEINLINE auto operator=(LEnginePathBase<T>&& Other) noexcept -> LEnginePathBase<T>& { this->Data = std::forward<LPathTy>(Other.Data); return *this; }
    FORCEINLINE auto operator=(const LPathTy& Other) noexcept -> LEnginePathBase<T>& { this->Data = Other; return *this; }
    FORCEINLINE auto operator=(LPathTy&& Other) noexcept -> LEnginePathBase<T>& { this->Data = std::move(Other); return *this; }

    FORCEINLINE auto operator==(const LEnginePathBase<T>& Other) const noexcept -> bool { return this->Data == Other.Data;  }
    FORCEINLINE auto operator!=(const LEnginePathBase<T>& Other) const noexcept -> bool { return this->Data != Other.Data;  }
    FORCEINLINE auto operator <(const LEnginePathBase<T>& Other) const noexcept -> bool { return this->Data  < Other.Data;  }
    FORCEINLINE auto operator >(const LEnginePathBase<T>& Other) const noexcept -> bool { return this->Data  > Other.Data;  }
    FORCEINLINE auto operator<=(const LEnginePathBase<T>& Other) const noexcept -> bool { return this->Data <= Other.Data;  }
    FORCEINLINE auto operator>=(const LEnginePathBase<T>& Other) const noexcept -> bool { return this->Data >= Other.Data;  }
    FORCEINLINE auto Equals(const LEnginePathBase<T>& Other) const noexcept -> bool { return this->Data.Equals(Other.Data); }

    FORCEINLINE auto GetRelativeUnresolvedPath() const noexcept -> const LPathTy& { return this->Data; }

    inline LPathTy ResolveRelativePath() const;
    inline LPathTy ResolveRelativeEnginePath(const JUserPreferences& InUserPreferences) const;
    inline LPathTy ResolveAbsolutePath(const JUserPreferences& InUserPreferences) const;

    FORCEINLINE void AddExtension(const LStringTy& InExtension) { this->Data.AddExtension(InExtension); }

private:

    EEnginePaths::Type PathTy = EEnginePaths::None;
    LPathTy            Data   = nullptr;
};

template <typename InTPathTy>
LEnginePathBase<InTPathTy>::LEnginePathBase(const LPathTy& Other) noexcept
{
    this->Data = Other;

    if (this->Data.GetPath().StartsWith(Finder::GetEngineRootDir().GetPath()))
    {
        this->PathTy = EEnginePaths::CustomEngine;
        this->Data.GetMutablePath().InlineRightChop(Finder::GetEngineRootDir().GetPath().GetRuneCount()+1);
    }

    return;
}

template <typename InTPathTy>
LEnginePathBase<InTPathTy>::LEnginePathBase(LPathTy&& Other) noexcept
{
    this->Data = std::move(Other);

    if (this->Data.GetPath().StartsWith(Finder::GetEngineRootDir().GetPath()))
    {
        this->PathTy = EEnginePaths::CustomEngine;
        this->Data.GetMutablePath().InlineRightChop(Finder::GetEngineRootDir().GetPath().GetRuneCount()+1);
    }

    return;
}

template <typename InTPathTy>
typename LEnginePathBase<InTPathTy>::LPathTy LEnginePathBase<InTPathTy>::ResolveRelativePath() const
{
    return Finder::ResolvePathToRelativeModulePath(*this);
}

template <typename InTPathTy>
typename LEnginePathBase<InTPathTy>::LPathTy LEnginePathBase<InTPathTy>::ResolveRelativeEnginePath(const JUserPreferences& InUserPreferences) const
{
    return Finder::ResolvePathToRelativeEnginePath(*this, InUserPreferences);
}

template <typename InTPathTy>
typename LEnginePathBase<InTPathTy>::LPathTy
LEnginePathBase<InTPathTy>::ResolveAbsolutePath(const JUserPreferences& InUserPreferences) const
{
    return Finder::ResolvePathToAbsolutePath(*this, InUserPreferences);
}

} /* ~Namespace Jafg */
