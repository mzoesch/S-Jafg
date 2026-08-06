// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Overlay.h"
#include "Nodes/DragRegion.h"
#include "Nodes/Switcher.h"
#include "Nodes/TextButton.h"
#include "Nodes/GenericTabInfos.h"
#include "TabOverlay.generated.h"

namespace Jafg
{

class WUserWidget;
class WTabOverlay;
class WTabOverlaySelector;
class WTabOverlayParent;
struct LTabCreateInfo;
struct LFactoryTabOverlay;
struct LFactoryTabOverlayParent;

struct LTabOverlayPossibilities
{
    virtual ~LTabOverlayPossibilities() = default;

    constexpr explicit LTabOverlayPossibilities(WUserWidget& Owner) : Owner{Owner} {}
    PROHIBIT_REALLOC_OF_ANY_FORM(LTabOverlayPossibilities)

    virtual WParent& GetOverlayRoot() noexcept = 0;

    FORCEINLINE void RegisterOverlay(WTabOverlay& TabOverlay) noexcept;
    FORCEINLINE void UnregisterOverlay(WTabOverlay& TabOverlay) noexcept;
    ENGINE_API LFactoryTabOverlay GetNewOverlay();
    ENGINE_API WTabOverlay& FindNewOverlay(f32 Dist = {});

    ENGINE_API LFactoryTabOverlayParent GetNewOverlayParent();

    ENGINE_API WUserWidget& AddWindow(LTabCreateInfo Info, bool bFocus);
    template<typename TWidget> requires std::is_base_of_v<WUserWidget, TWidget> && CTabCandidate<TWidget>
    inline TWidget& AddWindow(bool bFocus)
    {
        return *StaticCast<TWidget>(&this->AddWindow(TWidget::TabCreateInfo(), bFocus));
    }

    WUserWidget& Owner;
    WTabOverlay* Selected{};
    TArray<TReference<WTabOverlay>> Overlays;
};

//#
//# A tab overlay is an overlay widget with a switcher and a selector container.
//# It automatically provides common standard tab logic to its children.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTabOverlay)
class ENGINE_API WTabOverlay : public WOverlay
{
    GENERATED_CLASS_BODY()

    friend WTabOverlayParent;
    friend LFactoryTabOverlay;

protected:

    DEFAULT_NODE_CONSTRUCTORS(WTabOverlay)

public:

    typedef std::pair<WTabOverlaySelector*, WUserWidget*> Tab;

    virtual void Draw(LNodeRenderInfo const& Info) const override;

    virtual void Construct() override;
    virtual void Destruct() override
    {
        if (this->Possibilities)
        {
            this->Possibilities->UnregisterOverlay(*this);
        }
        Super::Destruct();
        return;
    }

    //# Registers a new tab. Do not call #AddChild or any of its variants.
    Tab  RegisterTab(LTabCreateInfo&& Info);
    void CloseTab(WTabOverlaySelector* Selector) { ensure(!this->CloseTabImpl(Selector, false).has_value()); }

    //# Whether the selector parent is now hidden.
    bool HideTabSelector();
    void ShowTabSelector() { check(this->Selectors) this->Selectors->SetVisibility(ENodeVisibility::IntransitiveHitTestInvisible); }

    FORCEINLINE auto const& GetTabs() const noexcept { return this->Tabs; }

    WUserWidget* FindWidgetSlow(LJxxClass const& Class) noexcept;
    template<typename TWidget> requires std::is_base_of_v<WUserWidget, TWidget>
    TWidget* FindWidgetSlow() noexcept
    {
        for (auto& Panel : this->Tabs | std::views::values)
        {
            if (TWidget* Casted{DynamicCast<TWidget>(Panel)})
            {
                return Casted;
            }
        }
        return nullptr;
    }
    template<typename TWidget> requires std::is_base_of_v<WUserWidget, TWidget>
    TWidget const* FindWidgetSlow() const noexcept
    {
        for (auto const& Panel : this->Tabs | std::views::values)
        {
            if (TWidget const* Casted{DynamicCast<TWidget>(Panel)})
            {
                return Casted;
            }
        }
        return nullptr;
    }

    void SetSelectedTab(WTabOverlaySelector& Target);

private:

    void InitializeBoilerplate();
    std::optional<TJxxUnique<WUserWidget>> CloseTabImpl(WTabOverlaySelector* Selector, bool bRelease);
    void CreateTabMenu(std::optional<LVec2F> Hint, WTabOverlaySelector& Selector);

    TArray<Tab> Tabs;
    LTabOverlayPossibilities* Possibilities{};

    //# The class that wraps the tab buttons and the switcher.
    WParent* Wrapper{};
    //# The container for selectors.
    WParent* Selectors{};
    //# The actual selectors.
    TFunction2<void(LFactoryTextButton& Factory)> DefaultSelectorDelegate;
    //# The switcher dictates where the content panels are stored.
    WSwitcher* Switcher{};

    struct StepResult
    {
        inline static constexpr f32 Percentage{0.25f};
        inline static constexpr f32 MaxDistance{300.0f};

        enum EDirection{L,U,R,D,C,};
        WTabOverlay* Overlay;
        EDirection Direction;

        template<EDirection Direction> requires(Direction != C)
        NODISCARD FORCEINLINE static f32 GetMaxDistanceFromEdge(f32 Axis) noexcept
        {
            if constexpr (Direction == L || Direction == U)
            {
                return maths::min(Axis * Percentage, MaxDistance);
            }
            else if constexpr (Direction == R || Direction == D)
            {
                return maths::max(Axis * (1.0f - Percentage), Axis - MaxDistance);
            }
            else
            {
                std::unreachable();
            }
        }
    };
    bool MouseTabMoveTick(WTabOverlaySelector& Selector);
    StepResult StepThrough(WParent& Node, LVec2F Location) const noexcept;
    LDelegateHandle UiTickHandle;
    std::optional<StepResult> LastStepResult;
    std::optional<StepResult::EDirection> DrawOption;
    EModFlags LastMouseFlags{ EModBits::Identity };
    WBox* TempBox{};
    std::optional<LVec2F> CachedMoveTabSize;
};

struct LFactoryTabOverlay : NODE_FACTORY_PARENT(WTabOverlay)
{
    NODE_FACTORY_BODY(WTabOverlay)

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(DefaultSelectorDelegate, DefaultSelectorDelegate)

    decltype(auto) Possibilities(this auto&& Self, LTabOverlayPossibilities& Possibilities) noexcept
    {
        NODE_FACTORY_SELF().Possibilities = &Possibilities;
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) operator[](this auto&& Self, LTabCreateInfo Descriptor) noexcept
    {
        NODE_FACTORY_SELF().RegisterTab(std::move(Descriptor));
        return NODE_FACTORY_RESULT();
    }
};

DECLARE_JAFG_WIDGET()
class ENGINE_API WTabOverlaySelector final : public WTextButtonIconizedDouble
{
    GENERATED_CLASS_BODY()

protected:

    explicit WTabOverlaySelector(LNodeDynamicInit const& Init) noexcept : Super{Init}
    {
        this->CtorLogic();
    }

    template<typename TCxxClass>
    explicit WTabOverlaySelector(TNodeStaticInit<TCxxClass> const& Init, LTabSelectorCreateInfo Info) noexcept : Super{Init}
    {
        this->CtorLogic();

        this->SetContent(std::move(Info.DisplayName));
        this->LeftIcon = Info.Icon.GetResolved();
        return;
    }

private:

    void CtorLogic() noexcept;
    void LoadRightIcon();
};

//# An optional parent to #WTabOverlays.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTabOverlayParent)
class ENGINE_API WTabOverlayParent final : public WDragRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WTabOverlayParent)

public:

#if JAFG_DO_CHECKS
    virtual void Construct() override
    {
        Super::Construct();
        check(this->Possibilities)
    }
#endif /* JAFG_DO_CHECKS */

    virtual void OnRemoveChildPost(WNode& Child) override;

    enum struct EDirection{ Left, Up, Right, Down, };
    void MoveHere(WTabOverlaySelector& WhoSelector, WTabOverlay& Where, std::optional<EDirection> Direction, bool bDuplicate);

    FORCEINLINE void SetPossibilities(LTabOverlayPossibilities& Possibilities) noexcept
    {
        check(this->_HasBegunLife() == false)
        this->Possibilities = &Possibilities;
        return;
    }
    FORCEINLINE LTabOverlayPossibilities* GetPossibilities() const noexcept { return this->Possibilities; }
    FORCEINLINE LTabOverlayPossibilities* GetPossibilitiesChecked() const noexcept
    {
        check(this->Possibilities)
        return this->Possibilities;
    }

    bool bPreventAutoKillOnChildLoss{};

private:

    LTabOverlayPossibilities* Possibilities{};
};

FORCEINLINE LString LexToString(WTabOverlayParent::EDirection Direction) noexcept
{
    switch (Direction)
    {
    case WTabOverlayParent::EDirection::Left: { return "Left"; }
    case WTabOverlayParent::EDirection::Up: { return "Up"; }
    case WTabOverlayParent::EDirection::Right: { return "Right"; }
    case WTabOverlayParent::EDirection::Down: { return "Down"; }
    }
}

struct LFactoryTabOverlayParent : NODE_FACTORY_PARENT(WTabOverlayParent)
{
    NODE_FACTORY_BODY(WTabOverlayParent)

    decltype(auto) Possibilities(this auto&& Self, LTabOverlayPossibilities& Possibilities) noexcept
    {
        NODE_FACTORY_SELF().SetPossibilities(Possibilities);
        return NODE_FACTORY_RESULT();
    }
};

FORCEINLINE void LTabOverlayPossibilities::RegisterOverlay(WTabOverlay& TabOverlay) noexcept
{
    check(!algo::contains(this->Overlays, &TabOverlay, algo::universal_ptr_noop))
    this->Overlays.emplace_back(TabOverlay);
    return;
}

FORCEINLINE void LTabOverlayPossibilities::UnregisterOverlay(WTabOverlay& TabOverlay) noexcept
{
    auto It{algo::find(this->Overlays, &TabOverlay, algo::universal_ptr_noop)};
    check(It != this->Overlays.end())
    this->Overlays.erase(It);

    if (this->Selected == &TabOverlay)
    {
        this->Selected = nullptr;
    }

    return;
}

} /* ~Namespace Jafg */
