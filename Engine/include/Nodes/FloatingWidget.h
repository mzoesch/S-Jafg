// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/VRegion.h"
#include "Nodes/Text.h"
#include "FloatingWidget.generated.h"

namespace Jafg
{

class WText;
class WTextBox;
class WVRegion;
class WFloatingWidget;
struct LFactoryFloatingWidget;

//# A widget that floats around.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryFloatingWidget)
class ENGINE_API WFloatingWidget : public WUserWidget
{
    GENERATED_CLASS_BODY(ENGINE_API)

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WFloatingWidget)
    {
        this->Anchor = EAnchor::TopLeft;
    }

public:

    //# Prefer this over #RemoveFromParent2 or #MarkAsGarbage_v2.
    void DestroyFloatingWidgetControlled();

    //#
    //# This event will let you create the content of the floating window. It has to be bound at construction time.
    //# @param FloatingWidget The floating window invoking this event.
    //# @param Container      The container to add the content to.
    //#
    EVENT_DECL(ContentEvent, void(WFloatingWidget& FloatingWidget, WParent& Container))
    virtual void Construct() override;

    virtual WNode& OnAddChild(std::size_t Index, TJxxUnique<WNode> Child, bool bConstructed) override
    {
        check(this->GetChildren().empty() && "WFloatingWidget can only have one child.")
        return Super::OnAddChild(Index, std::move(Child), bConstructed);
    }

    inline WNode& GetWindow() noexcept { check(this->GetChildren().size() == 1) return *this->GetChildren()[0]; }
    inline WNode const& GetWindow() const noexcept { check(this->GetChildren().size() == 1) return *this->GetChildren()[0]; }

    FORCEINLINE void SetWindowSize(LNodeSize2 Size) noexcept
    {
        auto StaticPintSize{Size.InStaticPoints(this->GetViewport())};
        this->GetWindow().MinDesiredSize = {ENodeSize::StaticPoints
            , maths::max(StaticPintSize.x, WFloatingWidget::MinWindowSizeInSpt.x)
            , maths::max(StaticPintSize.y, WFloatingWidget::MinWindowSizeInSpt.y)
            };
    }
    FORCEINLINE void SetWindowPosition(LVec2F Offset) noexcept
    {
        this->GetWindow().GetParent()->Padding = {ENodeSize::StaticPoints, std::round(Offset.x), std::round(Offset.y), 0, 0};
    }

    FORCEINLINE constexpr bool IsDecorated() const noexcept { return this->bDecorate; }
    FORCEINLINE constexpr void SetDecorated(bool b) noexcept { check(this->_HasBegunLife() == false) this->bDecorate = b; }
    FORCEINLINE constexpr bool IsCreateResizeUi() const noexcept { return this->bCreateResizeUi; }
    FORCEINLINE constexpr void SetCreateResizeUi(bool b) noexcept { check(this->_HasBegunLife() == false) this->bCreateResizeUi = b; }

    //#
    //# Event called when the window is closed.
    //#
    //# Return true, if the event was handled. Nothing will be further done.
    //# Return false, if the event was not handled. The default behavior will be executed.
    //#
    //# The default behavior is to destroy the window.
    //#
    //# @note Only relevant if the window is decorated or the custom implementation uses this behavior.
    //#
    EVENT_DECL(OnWindowClosedEvent, bool(WFloatingWidget& InWindow))

    FORCEINLINE constexpr void SetInitialTitle(LString S) noexcept { check(this->_HasBegunLife() == false) this->InitialTitle = std::move(S); }
    FORCEINLINE constexpr bool IsWindowTitleValid() const noexcept { return this->WindowTitle != nullptr; }
    FORCEINLINE void SetWindowTitle(LString Title) noexcept { this->GetMutableWindowTitle().SetContent(std::move(Title)); }
    FORCEINLINE constexpr WText const& GetWindowTitle() const noexcept { check(this->WindowTitle) return *this->WindowTitle; }
    FORCEINLINE constexpr WText& GetMutableWindowTitle() noexcept { check(this->WindowTitle) return *this->WindowTitle; }

    FORCEINLINE constexpr void SetInitialWindowSize(LNodeSize2 Size) noexcept { check(this->_HasBegunLife() == false) this->InitialWindowSize = Size; }
    FORCEINLINE constexpr void SetInitialWindowPosition(LVec2F Position) noexcept { check(this->_HasBegunLife() == false) this->InitialWindowPosition = Position; }
    FORCEINLINE constexpr void SetInitialWindowAnchor(EAnchor Anchor) noexcept { check(this->_HasBegunLife() == false) this->InitialWindowAnchor = Anchor; }
    FORCEINLINE constexpr void SetInitialWindowVisibility(ENodeVisibility Visibility) noexcept { check(this->_HasBegunLife() == false) this->InitialWindowVisibility = Visibility; }

protected:

    bool UiTickMove();
    LDelegateHandle UiTickMoveHandle;
    std::optional<LVec2F> MoveDragOffset;

    bool bDecorate{ true };
    bool bCreateResizeUi{ true };
    LDelegateHandle UiTickResizeHandle;
    bool UiTickResize();
    std::optional<LVec2F> ResizeDragOffset;

private:

    LString InitialTitle;
    WText* WindowTitle{};

    LNodeSize2 InitialWindowSize{ 640_spt, 360 };
    static constexpr LVec2F MinWindowSizeInSpt{ 1, 1 };
    LVec2F InitialWindowPosition{ maths::zero_vector<LVec2F> };
    EAnchor InitialWindowAnchor{ EAnchor::TopLeft };
    ENodeVisibility InitialWindowVisibility{ ENodeVisibility::Visible };
};

struct LFactoryFloatingWidget : NODE_FACTORY_PARENT(WFloatingWidget)
{
    NODE_FACTORY_BODY(WFloatingWidget)

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(Content, ContentEvent)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnWindowClosed, OnWindowClosedEvent)

    constexpr decltype(auto) Decorate(this auto&& Self, bool b) noexcept
    {
        NODE_FACTORY_SELF().SetDecorated(b);
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) CreateResizeUi(this auto&& Self, bool b) noexcept
    {
        NODE_FACTORY_SELF().SetCreateResizeUi(b);
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) InitialTitle(this auto&& Self, LString S) noexcept
    {
        NODE_FACTORY_SELF().SetInitialTitle(std::move(S));
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) InitialWindowSize(this auto&& Self, LNodeSize2 Size) noexcept
    {
        NODE_FACTORY_SELF().SetInitialWindowSize(Size);
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) InitialWindowPosition(this auto&& Self, LVec2F Position) noexcept
    {
        NODE_FACTORY_SELF().SetInitialWindowPosition(Position);
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) InitialWindowAnchor(this auto&& Self, EAnchor Anchor) noexcept
    {
        NODE_FACTORY_SELF().SetInitialWindowAnchor(Anchor);
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) InitialWindowVisibility(this auto&& Self, ENodeVisibility Visibility)
    {
        NODE_FACTORY_SELF().SetInitialWindowVisibility(Visibility);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
