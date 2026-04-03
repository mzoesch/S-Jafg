// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Overlay.h"
#include "Nodes/Switcher.h"
#include "Nodes/TextButton.h"
#include "TabOverlay.generated.h"

namespace Jafg
{

struct LFactoryTabOverlay;
struct LTabOverlayElement;

//#
//# A tab overlay is an overlay widget with a switcher and a selector container-
//# It automatically provides common standard tab logic to its children.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTabOverlay)
class ENGINE_API WTabOverlay : public WOverlay
{
    GENERATED_CLASS_BODY()

    //# For subclass access.
    friend LFactoryTabOverlay;

protected:

    DEFAULT_NODE_CONSTRUCTORS(WTabOverlay)

public:

    virtual void Construct() override;

    //# Registers a new tab. Do not call #AddChild or any of its variants.
    void RegisterTab(LTabOverlayElement&& Descriptor);

private:

    void InitializeBoilerplate();

    //# The class that wraps the tab buttons and the switcher.
    TSubclassOf<WParent> WrapperClass;
    TFunction<void(LFactoryNode& Factory)> WrapperDelegate;
    WParent* Wrapper{};
    //# The container for selectors.
    TSubclassOf<WParent> SelectorsClass;
    TFunction<void(LFactoryNode& Factory)> SelectorsDelegate;
    WParent* Selectors{};
    //# The actual selectors.
    TFunction<void(LFactoryNode& Factory)> DefaultSelectorDelegate;
    //# The switcher dictates where the content panels are stored.
    TSubclassOf<WSwitcher> SwitcherClass;
    TFunction<void(LFactoryNode& Factory)> SwitcherDelegate;
    WSwitcher* Switcher{};
};

//# Use this struct to describe a tap you want to add to #WTabOverlay.
struct LTabOverlayElement
{
    struct Info
    {
        LString DisplayName;
        LTexture2Ref Icon;
    };

    std::variant<Info, Detail::LNodeFactoryBase> Selector;
    Detail::LNodeFactoryBase Panel;

    decltype(auto) operator+(LTabOverlayElement&& Sibling) && noexcept
    {
        this->_Siblings.emplace_back(std::move(Sibling));
        return std::move(*this);
    }

    /* Jafg internal member. Do not use. */
    TArray<LTabOverlayElement> _Siblings;
};

DECLARE_JAFG_WIDGET()
class ENGINE_API WTabOverlaySelector final : public WTextButton
{
    GENERATED_CLASS_BODY()

protected:

    explicit WTabOverlaySelector(LNodeDynamicInit const& Init) noexcept : Super{Init}
    {
        this->SetOmniPadding({3_pt, 2});
        this->SetNormalTextTint({0x90});
        this->SetNormalIconTint({0x90});
        this->SetSelectable(true);
    }

    template<typename TCxxClass>
    explicit WTabOverlaySelector(TNodeStaticInit<TCxxClass> const& Init, LTabOverlayElement::Info TabInfo) noexcept : Super{Init}
    {
        this->SetOmniPadding({3_pt, 2});
        this->SetNormalTextTint({0x90});
        this->SetNormalIconTint({0x90});
        this->SetSelectable(true);
        this->SetContent(std::move(TabInfo.DisplayName));
        this->SetIcon(std::move(TabInfo.Icon));
    }
};

struct LFactoryTabOverlay : NODE_FACTORY_PARENT(WTabOverlay)
{
    NODE_FACTORY_BODY(WTabOverlay)

    template<typename TParent> requires std::is_base_of_v<WParent, TParent>
    decltype(auto) WrapperClass(this auto&& Self, TSubclassOf<TParent> Class, TFunction<void(typename TParent::LFactory&)> Factory = {}) noexcept
    {
        NODE_FACTORY_SELF().WrapperClass = Class;
        if (Factory.IsValid())
        {
            NODE_FACTORY_SELF().WrapperDelegate = [Fac = std::move(Factory)](LFactoryNode& Factory)
            {
                check(Factory.GetRawNode().IsA<TParent>())
                Fac(static_cast<typename TParent::LFactory&>(Factory));
            };
        }
        return NODE_FACTORY_RESULT();
    }
    template<typename TParent> requires std::is_base_of_v<WParent, TParent>
    decltype(auto) SelectorsClass(this auto&& Self, TSubclassOf<TParent> Class, TFunction<void(typename TParent::LFactory&)> Factory = {}) noexcept
    {
        NODE_FACTORY_SELF().SelectorsClass = Class;
        if (Factory.IsValid())
        {
            NODE_FACTORY_SELF().WrapperDelegate = [Fac = std::move(Factory)](LFactoryNode& Factory)
            {
                check(Factory.GetRawNode().IsA<TParent>())
                Fac(static_cast<typename TParent::LFactory&>(Factory));
            };
        }
        return NODE_FACTORY_RESULT();
    }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    decltype(auto) DefaultSelectorDelegate(this auto&& Self, TFunction<void(typename TNode::LFactory&)> Factory) noexcept
    {
        check(Factory.IsValid())
        NODE_FACTORY_SELF().WrapperDelegate = [Fac = std::move(Factory)](LFactoryNode& Factory)
        {
            check(Factory.GetRawNode().IsA<TNode>())
            Fac(static_cast<typename TNode::LFactory&>(Factory));
        };
        return NODE_FACTORY_RESULT();
    }
    template<typename TSwitcher> requires std::is_base_of_v<WSwitcher, TSwitcher>
    decltype(auto) SwitcherClass(this auto&& Self, TSubclassOf<TSwitcher> Class, TFunction<void(typename TSwitcher::LFactory&)> Factory = {}) noexcept
    {
        NODE_FACTORY_SELF().SwitcherClass = Class;
        if (Factory.IsValid())
        {
            check(Factory.IsCopyable())
            NODE_FACTORY_SELF().WrapperDelegate = [Fac = std::move(Factory)](LFactoryNode& Factory)
            {
                check(Factory.GetRawNode().IsA<TSwitcher>())
                Fac(static_cast<typename TSwitcher::LFactory&>(Factory));
            };
        }
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) operator[](this auto&& Self, LTabOverlayElement&& Descriptor) noexcept
    {
        NODE_FACTORY_SELF().RegisterTab(std::move(Descriptor));
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
