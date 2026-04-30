// Copyright mzoesch. All rights reserved.

#pragma once

#include "HDragRegion.h"
#include "Nodes/Overlay.h"
#include "Nodes/Switcher.h"
#include "Nodes/TextButton.h"
#include "Nodes/GenericTabInfos.h"
#include "TabOverlay.generated.h"

namespace Jafg
{

class WUserWidget;
class WTabOverlay;
class WTabOverlaySelector;
class WTabOverlayHParent;
struct LTabCreateInfo;
struct LFactoryTabOverlay;
struct LFactoryTabOverlayHParent;

struct LTabOverlayPossibilities
{
    virtual ~LTabOverlayPossibilities() = default;

    constexpr explicit LTabOverlayPossibilities(WUserWidget& Owner) : Owner{Owner} {}
    PROHIBIT_REALLOC_OF_ANY_FORM(LTabOverlayPossibilities)

    WUserWidget& Owner;

    WTabOverlay* Selected{};
    TArray<TReference<WTabOverlay>> Overlays;

    void FindNewOverlay();

    ENGINE_API WUserWidget& AddWindow(LTabCreateInfo Info, bool bFocus);
    template<typename TWidget> requires std::is_base_of_v<WUserWidget, TWidget> && CTabCandidate<TWidget>
    inline TWidget& AddWindow(bool bFocus)
    {
        return *StaticCast<TWidget>(&this->AddWindow(TWidget::TabCreateInfo(), bFocus));
    }
};

//#
//# A tab overlay is an overlay widget with a switcher and a selector container.
//# It automatically provides common standard tab logic to its children.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTabOverlay)
class ENGINE_API WTabOverlay : public WOverlay
{
    GENERATED_CLASS_BODY()

    friend WTabOverlayHParent;
    friend LFactoryTabOverlay;

protected:

    DEFAULT_NODE_CONSTRUCTORS(WTabOverlay)

public:

    typedef std::pair<WTabOverlaySelector*, WUserWidget*> Tab;

    virtual void Construct() override;
    virtual void Destruct() override
    {
        if (this->Possibilities)
        {
            if (auto It{algo::find(this->Possibilities->Overlays, StaticCast<WTabOverlay>(this), algo::universal_ptr_noop{})}; It != this->Possibilities->Overlays.end())
            {
                this->Possibilities->Overlays.erase(It);
            }
            else
            {
                LOG_WARNING(LogWidgets, "The tab overlay [{}] was not registered in its associated possibilities.", this->GetNameAsString())
            }
        }
        Super::Destruct();
        return;
    }

    //# Registers a new tab. Do not call #AddChild or any of its variants.
    Tab  RegisterTab(LTabCreateInfo&& Info);
    void CloseTab(WTabOverlaySelector* Selector) { ensure(!this->CloseTabImpl(Selector, false).has_value()); }

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

    bool bKillIfNoChildren{};

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
};

struct LFactoryTabOverlay : NODE_FACTORY_PARENT(WTabOverlay)
{
    NODE_FACTORY_BODY(WTabOverlay)

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(DefaultSelectorDelegate, DefaultSelectorDelegate)

    decltype(auto) Possibilities(this auto&& Self, LTabOverlayPossibilities& Possibilities) noexcept
    {
        check(!algo::contains(Possibilities.Overlays, StaticCast<WTabOverlay>(&Self.GetRawNode()), algo::universal_ptr_noop{}))
        Possibilities.Overlays.emplace_back(*StaticCast<WTabOverlay>(&Self.GetRawNode()));
        NODE_FACTORY_SELF().Possibilities = &Possibilities;
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) KillIfNoChildren(this auto&& Self, bool bValue) noexcept
    {
        NODE_FACTORY_SELF().bKillIfNoChildren = bValue;
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) operator[](this auto&& Self, LTabCreateInfo Descriptor) noexcept
    {
        NODE_FACTORY_SELF().RegisterTab(std::move(Descriptor));
        return NODE_FACTORY_RESULT();
    }
};

DECLARE_JAFG_WIDGET()
class ENGINE_API WTabOverlaySelector final : public WTextButton
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

    virtual void Draw(LNodeRenderInfo const& Info) const override
    {
        if (this->GetContent() == "Tag Inspector")
        {
            JAFG_PLATFORM_NO_DISCARD_CTRL_PATH
        }
        Super::Draw(Info);
        return;
    }
    virtual LNodeReply Sweep(LNodeSweepInfo const& Info, std::optional<LVec2F> const& Location) override
    {
        return Super::Sweep(Info, Location);
    }

private:

    inline void CtorLogic() noexcept
    {
        this->SetSelectable(true);
        this->Style.SetEverywhere<&LBoxBrush::Padding>({3_spt, 0});
        this->TextStyle.NormalBrush.Tint = {0x90};
        this->LeftIconStyle.NormalBrush.Tint = {0x90};
        this->RightIconStyle.NormalBrush.Tint = {0x90};
        this->LoadRightIcon();
    }

    void LoadRightIcon();
};

//# An optional parent to #WTabOverlays on the horizontal axis.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTabOverlayHParent)
class ENGINE_API WTabOverlayHParent final : public WHDragRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WTabOverlayHParent)

public:

    enum struct EDirection
    {
        Left, Right,
    };

    void Move(WTabOverlaySelector& Selector, EDirection Direction);

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

private:

    LTabOverlayPossibilities* Possibilities{};
};

struct LFactoryTabOverlayHParent : NODE_FACTORY_PARENT(WTabOverlayHParent)
{
    NODE_FACTORY_BODY(WTabOverlayHParent)

    decltype(auto) Possibilities(this auto&& Self, LTabOverlayPossibilities& Possibilities) noexcept
    {
        NODE_FACTORY_SELF().SetPossibilities(Possibilities);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
