// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/FrontendForward.h"
#include "Platform/Surface.h"
#include "Widgets/Node.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/FrontendSubsystem.h"
#include "Rhi/Texture2.h"

namespace Jafg
{

class LUserInput;
class LViewport;
class WUserWidget;

//# Represents an external physical monitor.
struct LPhysicalViewport
{
    void* Identifier{ nullptr };

    // Physical size in millimeters.
    LVec2i32 SizeMm;
    // Virtual size in pixels.
    LVec2i32 WorkareaPx;
    // Offset of the workarea in pixels.
    LVec2i32 WorkareaOffsetPx;

    // Scale of content.
    LVec2F ContentScale;

    //# If multiple monitors of the same name exist, this prefix can help to distinguish them.
    LString Prefix;
    //# Name of the monitor.
    LString Name;

    //# Whether this is the primary monitor.
    //# ??? Just fucking ignore that. For windows yay; for x11 yay; for wayland hell nah.
    // bool bPrimary{ false };

    // In this order: RGB.
    LVec3i32 Bits;

    i32 RefreshRateHz{ 0 };

    FORCEINLINE LString ToHumanReadableName() const
    {
        return Jafg::SprintF("{}{} ({}x{}px)",
            this->Prefix,
            this->Name,
            this->WorkareaPx.x, this->WorkareaPx.y
            );
    }
};

//#
//# The frontend is owned by the local ego and shares its lifetime.
//# The frontend is the main hub for all user interface elements. Create frontend subsystems to automatically
//# add multiple widgets of a type any viewport.
//#
class LFrontendBase
{
public:

    enum ENewSurfaceBehavior
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

    ENGINE_API LEngine&   GetEngine() const noexcept;
    ENGINE_API LLocalEgo& GetLocalEgo() const noexcept;

    FORCEINLINE TArray<LPhysicalViewport> const& GetPhysicalViewports() const noexcept { return this->UsablePhysicalViewports; }

    ENGINE_API void AddSurface(TUnique<LSurface> Surface, ENewSurfaceBehavior Behavior = ENewSurfaceBehavior::NoAction) noexcept;

    FORCEINLINE LSize GetSurfaceCount() const noexcept { return this->Surfaces.size(); }
    FORCEINLINE TArray<TUnique<LSurface>>& GetSurfaces() noexcept { return this->Surfaces; }
    FORCEINLINE TArray<TUnique<LSurface>> const& GetSurfaces() const noexcept { return this->Surfaces; }

    FORCEINLINE bool IsFocusedSurfaceValid() const { return this->FocusedSurface > INDEX_NONE; }
    FORCEINLINE LSurface* GetFocusedSurface() noexcept { if (this->IsFocusedSurfaceValid()) { return this->Surfaces[this->FocusedSurface].get(); } return nullptr; }
    FORCEINLINE LSurface* GetFocusedSurfaceChecked() noexcept { check(this->IsFocusedSurfaceValid()) return this->Surfaces[this->FocusedSurface].get(); }
    FORCEINLINE LSurface* GetFocusedSurfaceAsserted() noexcept { jassert(this->IsFocusedSurfaceValid()) return this->Surfaces[this->FocusedSurface].get(); }
    FORCEINLINE LSurface const* GetFocusedSurface() const noexcept { if (this->IsFocusedSurfaceValid()) { return this->Surfaces[this->FocusedSurface].get(); } return nullptr; }
    FORCEINLINE LSurface const* GetFocusedSurfaceChecked() const noexcept { check(this->IsFocusedSurfaceValid()) return this->Surfaces[this->FocusedSurface].get(); }
    FORCEINLINE LSurface const* GetFocusedSurfaceAsserted() const noexcept { jassert(this->IsFocusedSurfaceValid()) return this->Surfaces[this->FocusedSurface].get(); }

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JFrontendSubsystem)

    FORCEINLINE auto const& GetGuaranteedTextures() const noexcept { return this->GuaranteedTextures; }

protected:

    //#
    //# Represents all physical viewports that may be used by this frontend.
    //# There may be more physical viewports available on the platform, but these have some restrictions that prevent
    //# their use for us.
    //#
    //# Some monitors may become unusable over time (for example, if they are disconnected).
    //#
    TArray<LPhysicalViewport> UsablePhysicalViewports;

private:

    i32 FocusedSurface{ INDEX_NONE };
    TArray<TUnique<LSurface>> Surfaces;
    LSubsystemCollection Collection{ "Frontend" };

    //# Textures the frontend owns and, therefore, are always loaded.
    TArray<LTexture2Ref> GuaranteedTextures;
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
