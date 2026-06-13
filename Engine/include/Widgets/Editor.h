// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/GenericTabInfos.h"
#include "Nodes/TabOverlay.h"
#include "Nodes/VRegion.h"
#include "Engine/Engine.h"
#include "Editor.generated.h"

namespace Jafg
{

class WTabOverlay;
class WEditor;
class WButton;
class WTextButton;
class WEditorCategorySeparator;
struct LFactoryEditorCategorySeparator;
struct LEditorLayout;

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
    serde::JsonExpectType<LEditorLayout::LChild>(j, json::value_t::object);
    if (j.contains("Controlflow"))
    {
        if (j.contains("Dist"))
        {
            serde::JsonSink<LEditorLayout::LChild>(j, "Expected either a LFlow or LNodes but found keys for both. Failed to parse layout.");
        }
        C = j.get<LEditorLayout::LFlow>();
    }
    else
    {
        if (j.contains("Controlflow"))
        {
            serde::JsonSink<LEditorLayout::LChild>(j, "Expected either a LFlow or LNodes but found keys for both. Failed to parse layout.");
        }
        C = j.get<LEditorLayout::LNodes>();
    }

    return;
}

ENGINE_API void ToggleEditorNodesTransitively(WNode& Node, bool bEnabled);

DECLARE_JAFG_WIDGET()
class ENGINE_API WEditorBackground final : public WVRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WEditorBackground)
    {
        this->Anchor = EAnchor::HFill;
        this->Brush.Tint = Colors::Black;
        this->Space = 1_spt;
        this->Padding = {0_spt, 0, 0, 1};
    }
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryEditorCategorySeparator)
class ENGINE_API WEditorCategorySeparator final : public WTextButtonIconizedDouble
{
    GENERATED_CLASS_BODY()

    friend LFactoryEditorCategorySeparator;

protected:

    explicit WEditorCategorySeparator(LNodeDynamicInit const& Init) noexcept
        : Super{Init}
    {
        this->_ctor_Logic();
    }
    template<typename TCxxClass> explicit WEditorCategorySeparator(TNodeStaticInit<TCxxClass> const& Init, LString Category) noexcept
        : Super{Init}
    {
        this->SetContent(std::move(Category));
        this->_ctor_Logic();
    }

public:

    virtual LNodeReply OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override;

private:

    void _ctor_Logic();
    TArray<std::pair<ENodeVisibility, WNode*>> Nodes;
};

struct LFactoryEditorCategorySeparator : NODE_FACTORY_PARENT(WEditorCategorySeparator)
{
    NODE_FACTORY_BODY(WEditorCategorySeparator)

    decltype(auto) operator[](this auto&& Self, LNodeFactoryBase&& F) noexcept
    {
        check(!F._IsReleased())
        auto& Node{DETAIL_JAFG_NODE_FACTORY_SELF()};

        Self.GetMutableSiblings().emplace_back(&F.GetRawNode());
        Node.Nodes.emplace_back(F.GetRawNode().GetVisibility(), &F.GetRawNode());
        for (auto* Sibling: F.GetSiblings())
        {
            check(Sibling)
            Node.Nodes.emplace_back(Sibling->GetVisibility(), Sibling);
            Self.GetMutableSiblings().emplace_back(Sibling);
        }
        algo::orphan(&F.GetMutableSiblings());
        checkCode(F._Release())
        checkCode(F._Decommission())

        return NODE_FACTORY_RESULT();
    }
};

DECLARE_JAFG_WIDGET()
class ENGINE_API WEditor final : public WUserWidget, public LTabOverlayPossibilities
{
    GENERATED_CLASS_BODY()

protected:

    inline explicit WEditor(LNodeDynamicInit const& Init) noexcept
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
