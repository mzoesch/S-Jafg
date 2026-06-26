// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/FrontendForward.h"
#include "Platform/Surface.h"
#include "Nodes/Node.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/FrontendSubsystem.h"
#include "Rhi/Texture2.h"

namespace Jafg
{

class LUserInput;
class LViewport;
class WUserWidget;

//#
//# The frontend is owned by the local ego and shares its lifetime.
//# The frontend is the main hub for all user interface elements. Create frontend subsystems to automatically
//# add multiple widgets of a type any viewport.
//#
class LFrontendBase
{
public:

    enum ENewSurfaceBehavior : u8
    {
        //# Just add the new surface.
        NoAction,
        //# Add the new surface and focus it; potentially unfocusing an already existing surface.
        Focus,
        //# Add the new surface and focus it only if no other surface is currently focused.
        FocusIfNoneFocused,
        //# Add the new surface and focus it only if there is no surfaces at all.
        FocusIfNonePresent,
    };

    LFrontendBase() noexcept = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LFrontendBase)
    ~LFrontendBase() = default;

    template<typename T = LFrontendBase>
    NODISCARD FORCEINLINE T* As();
    template<typename T = LFrontendBase>
    NODISCARD FORCEINLINE T const* As() const;
    NODISCARD FORCEINLINE LFrontend* AsFrontend();
    NODISCARD FORCEINLINE LFrontend const* AsFrontend() const;

    void Initialize(LClassOuter* Outer);

    void Tick();
    void TearDown();

    ENGINE_API LEngine const& GetEngine() const noexcept;
    ENGINE_API LEngine& GetMutableEngine() noexcept;
    ENGINE_API LLocalEgo const& GetLocalEgo() const noexcept;
    ENGINE_API LLocalEgo& GetMutableLocalEgo() noexcept;

    FORCEINLINE TArray<LPhysicalViewport> const& GetPhysicalViewports() const noexcept { return this->UsablePhysicalViewports; }

    ENGINE_API LSurface& AddSurface(TUnique<LSurface> Surface, ENewSurfaceBehavior Behavior = ENewSurfaceBehavior::NoAction) noexcept;

    FORCEINLINE std::size_t GetSurfaceCount() const noexcept { return this->Surfaces.size(); }
    FORCEINLINE TArray<TUnique<LSurface>>& GetSurfaces() noexcept { return this->Surfaces; }
    FORCEINLINE TArray<TUnique<LSurface>> const& GetSurfaces() const noexcept { return this->Surfaces; }

    FORCEINLINE bool IsFocusedSurfaceValid() const { return this->FocusedSurface > INDEX_NONE; }
    FORCEINLINE LSurface* GetFocusedSurface() noexcept { if (this->IsFocusedSurfaceValid()) { return this->Surfaces[static_cast<std::size_t>(this->FocusedSurface)].get(); } return nullptr; }
    FORCEINLINE LSurface* GetFocusedSurfaceChecked() noexcept { check(this->IsFocusedSurfaceValid()) return this->Surfaces[static_cast<std::size_t>(this->FocusedSurface)].get(); }
    FORCEINLINE LSurface* GetFocusedSurfaceAsserted() noexcept { jassert(this->IsFocusedSurfaceValid()) return this->Surfaces[static_cast<std::size_t>(this->FocusedSurface)].get(); }
    FORCEINLINE LSurface const* GetFocusedSurface() const noexcept { if (this->IsFocusedSurfaceValid()) { return this->Surfaces[static_cast<std::size_t>(this->FocusedSurface)].get(); } return nullptr; }
    FORCEINLINE LSurface const* GetFocusedSurfaceChecked() const noexcept { check(this->IsFocusedSurfaceValid()) return this->Surfaces[static_cast<std::size_t>(this->FocusedSurface)].get(); }
    FORCEINLINE LSurface const* GetFocusedSurfaceAsserted() const noexcept { jassert(this->IsFocusedSurfaceValid()) return this->Surfaces[static_cast<std::size_t>(this->FocusedSurface)].get(); }

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JFrontendSubsystem)

    NODISCARD std::optional<LPhysicalKey> GetPhysicalKey(ELogicalKey LogicalKey) const noexcept PURE_VIRTUAL()

protected:

    //#
    //# Represents all physical viewports that may be used by this frontend.
    //# There may be more physical viewports available on the platform, but these have some restrictions that prevent
    //# their use for us.
    //#
    //# Some monitors may become unusable over time (for example, if they are disconnected). So do not store them
    //# for a long time. This array is updated when the platform notifies us about (hopefully).
    //#
    TArray<LPhysicalViewport> UsablePhysicalViewports;

private:

    i32 FocusedSurface{ INDEX_NONE };
    TArray<TUnique<LSurface>> Surfaces;
    LSubsystemCollection Collection{ "Frontend" };
};

} /* ~Namespace Jafg */

#include "Framework/FrontendVk.h"

template<typename T>
NODISCARD FORCEINLINE T* Jafg::LFrontendBase::As()
{
    static_assert(std::is_base_of_v<LFrontendBase, T>, "T must be derived from LFrontendBase");
    return static_cast<T*>(this);
}

template<typename T>
NODISCARD FORCEINLINE T const* Jafg::LFrontendBase::As() const
{
    static_assert(std::is_base_of_v<LFrontendBase, T>, "T must be derived from LFrontendBase");
    return static_cast<T const*>(this);
}

NODISCARD FORCEINLINE Jafg::LFrontend* Jafg::LFrontendBase::AsFrontend()
{
    return this->As<LFrontend>();
}

NODISCARD FORCEINLINE Jafg::LFrontend const* Jafg::LFrontendBase::AsFrontend() const
{
    return this->As<LFrontend>();
}
