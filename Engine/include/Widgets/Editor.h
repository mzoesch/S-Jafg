// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/GenericTabInfos.h"
#include "Nodes/TabOverlay.h"
#include "Editor.generated.h"

namespace Jafg
{

class WTabOverlay;

struct LEditorLayout final
{
    struct LNodes;
    struct LFlow;
    struct LSurface;
    typedef std::variant<LNodes, LFlow> LChild;

    struct LNodes final
    {
        f32 Dist{};
        TArray<LString> Children;
    };

    struct LFlow final
    {
        ENodePrimitiveControlflow Controlflow;
        TArray<LChild> Children;
    };

    struct LSurface final
    {
        bool bFullscreen{};
        bool bBorderless{};
        LVec2u32 Dimensions{1280, 720};
        LFlow Layout;
    };

    LPath Path;
    TArray<LSurface> Surfaces;

};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LEditorLayout::LNodes, Dist, Children)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LEditorLayout::LFlow, Controlflow, Children)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LEditorLayout::LSurface, bFullscreen, bBorderless, Dimensions, Layout)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LEditorLayout, Surfaces)
inline void to_json(json& j, LEditorLayout::LChild const& C){ std::visit([&j](auto&& Arg){ j = Arg; }, C); }
inline void from_json(json const& j, LEditorLayout::LChild& C)
{
    Serde::JsonExpectType<LEditorLayout::LChild>(j, json::value_t::object);
    if (j.contains("Controlflow"))
    {
        if (j.contains("Dist"))
        {
            Serde::JsonSink<LEditorLayout::LChild>(j, "Expected either a LFlow or LNodes but found keys for both. Failed to parse layout.");
        }
        C = j.get<LEditorLayout::LFlow>();
    }
    else
    {
        if (j.contains("Controlflow"))
        {
            Serde::JsonSink<LEditorLayout::LChild>(j, "Expected either a LFlow or LNodes but found keys for both. Failed to parse layout.");
        }
        C = j.get<LEditorLayout::LNodes>();
    }

    return;
}

DECLARE_JAFG_WIDGET()
class ENGINE_API WEditor final : public WUserWidget, public LTabOverlayPossibilities
{
    GENERATED_CLASS_BODY()

protected:

    inline  explicit WEditor(LNodeDynamicInit const& Init) noexcept
        : Super{Init}, LTabOverlayPossibilities{*static_cast<WUserWidget*>(this)}
    {
        this->SetShouldTick(true);
    }
    template<typename TCxxClass>
    inline explicit WEditor(TNodeStaticInit<TCxxClass> const& Init) noexcept
        : Super{Init}, LTabOverlayPossibilities{*static_cast<WUserWidget*>(this)}
    {
        this->SetShouldTick(true);
    }

public:

    static inline constexpr LPath GetUserLayoutsPath() noexcept
    {
        return Finder::GetSavedDir() / "Layouts";
    }
    static void BeginClassLife(LBeginClassLifeInfo const& Info);

    // LTabOverlayPossibilities implementation
    virtual WParent& GetOverlayRoot() noexcept override;
    // ~LTabOverlayPossibilities implementation

    virtual void Construct() override;
    virtual void Tick() override;

    FORCEINLINE TArray<LPath> const& GetDiscoveredLayouts() const noexcept { return this->DiscoveredLayouts; }
    TArray<LPath> const& DiscoverLayouts();
    LEditorLayout LoadEditorLayout(LPath Path);
    void ApplyEditorLayout(LEditorLayout const& Layout);

private:

    TArray<LPath> DiscoveredLayouts;

    WParent* OverlayRoot;
    WParent* Bar{};

    algo::clock::time_point LastRatePoint;
    WTextBox* Rate{};
};

} /* ~Namespace Jafg */
