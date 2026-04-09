// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Overlay.h"
#include "Nodes/Switcher.h"
#include "Nodes/TextButton.h"
#include "Nodes/TabOverlayForward.h"
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

DECLARE_JAFG_WIDGET()
class ENGINE_API WTabOverlaySelector final : public WTextButton
{
    GENERATED_CLASS_BODY()

protected:

    explicit WTabOverlaySelector(LNodeDynamicInit const& Init) noexcept : Super{Init}
    {
        this->SetOmniPadding({3_spt, 0});
        this->SetNormalTextTint({0x90});
        this->SetNormalIconTint({0x90});
        this->SetSelectable(true);

        return;
    }

    template<typename TCxxClass>
    explicit WTabOverlaySelector(TNodeStaticInit<TCxxClass> const& Init, LTabOverlayElement::CreateInfo TabInfo) noexcept : Super{Init}
    {
        this->SetOmniPadding({3_spt, 0});
        this->SetNormalTextTint({0x90});
        this->SetNormalIconTint({0x90});
        this->SetSelectable(true);

        this->SetContent(std::move(TabInfo.DisplayName));
        if (std::holds_alternative<LTexture2Ref>(TabInfo.Icon))
        {
            this->SetIcon(std::move(std::get<LTexture2Ref>(TabInfo.Icon)));
        }
        else if (std::holds_alternative<LString>(TabInfo.Icon))
        {
            this->LoadIconFromTextureViewIdentifier(std::get<LString>(TabInfo.Icon));
        }

        return;
    }

private:

    void LoadIconFromTextureViewIdentifier(LString const& Identifier) noexcept;
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
