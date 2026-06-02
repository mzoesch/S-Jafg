// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/TextButton.h"
#include "EditableTextButton.generated.h"

namespace Jafg
{

class WEditableTextButton;
class WEditableTextButtonIconizedLeft;
struct LFactoryEditableTextButton;
typedef TFactoryIconized<WEditableTextButtonIconizedLeft> LFactoryEditableTextButtonIconizedLeft;

enum struct ETextCommit : u8
{
    //# Text was committed via an enter key (or similar) press. This does not mean a loss of focus.
    OnEnter,
    //# Focus was lost due to a press of the escape key.
    OnCleared,
    //# Focus was lost due to some other reason.
    FocusLost,
};
inline LStringView LexToString(ETextCommit Enum) noexcept
{
    switch (Enum)
    {
    case ETextCommit::OnEnter:   { return "OnEnter"; }
    case ETextCommit::OnCleared: { return "OnCleared"; }
    case ETextCommit::FocusLost: { return "FocusLost"; }
    default:                     { std::unreachable(); }
    }
}

//#
//# A caret is a blinking line, block, or bitmap in the client area of a window. The caret typically indicates
//# the place at which glyphs will be inserted.
//#
struct LCaretBrush final
{
    LColor Tint{ Colors::White };
    //# The size in percent from the default. Default is (2px x DesiredSize.y).
    LVec2F Size{ 1.0f, 0.75f };
    //# Horizontal offset.
    f32 HOffset{ 0.0f };
    //# The blinker speed of the caret in seconds.
    f32 CaretBlinkerSpeed{ 0.5f };
};

//# A text button which the user can edit.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryEditableTextButton)
class WEditableTextButton : public WTextButton
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WEditableTextButton)
    {
        this->Anchor = EAnchor::Fill;
    }

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;

    virtual LNodeReply OnCursorEnter() override;
    virtual void OnCursorLeave() override;
    virtual void OnFocusReceived() override;
    virtual void OnFocusLost() override;
    virtual LNodeReply OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override;

    LCaretBrush CaretBrush;

    //# The placeholder text is a text that is displayed when no content is available.
    LString PlaceholderContent;
    LColor PlaceholderTint{ Colors::Gray };

    //#
    //# Delegate, which is invoked when the user tries to commit the content of the editable text box.
    //# @return True, if the user is allowed to commit the content, false otherwise.
    //# @note   This is useful if the content cannot be applied due to some validation, and handling the content validation
    //#         directly with the #LEditableTextBoxPredicateDelegate is not desired or viable.
    //#
    EVENT_DECL(OnAllowContentCommit, bool(WEditableTextButton& Self))

    //#
    //# Delegate, which is invoked when the user commits the content of the editable text box.
    //#
    EVENT_DECL(OnContentCommitted, void(WEditableTextButton& Self, LString const& Content, ETextCommit Commit))

    //#
    //# Delegate, which is invoked when the contents of the editable text box changed in any way.
    //#
    EVENT_DECL(OnContentChanged, void(WEditableTextButton& Self, LString const& Content))

    //#
    //# A delegate that is invoked, if bounded, when the user tries to change the content of the editable text box.
    //# @return True, if the content change is allowed, false otherwise. E.g.: you might only want to allow the user to
    //#         type in numbers, or you want to validate the content before allowing it. If false is returned, the
    //#         the content will not be changed.
    //# @note   After this delegate is invoked and the new content was accepted, the #LEditableTextBoxChangedDelegate
    //#         will still be invoked. Change events should be handled there. This should only be used for validation.
    //#         If this validation fails, the #LEditableTextBoxChangedDelegate will not be invoked.
    //#
    EVENT_DECL(ContentPredicate, bool(LString const& Content))

    //# @return The actual caret that was set.
    FORCEINLINE std::size_t SetCaretCursor(std::size_t Idx) noexcept
    {
        this->CaretCursor = maths::clamp(Idx, 0uz, this->GetContent().size());
        return this->CaretCursor;

    }
    FORCEINLINE std::size_t SetCaretCursorToBegin() noexcept { return this->SetCaretCursor(0uz); }
    FORCEINLINE std::size_t SetCaretCursorToEnd() noexcept { return this->SetCaretCursor(this->GetContent().size()); }
    FORCEINLINE std::size_t ReduceCaretCursor() noexcept
    {
        if (this->CaretCursor > 0uz)
        {
            this->SetCaretCursor(this->CaretCursor - 1);
        }
        return this->CaretCursor;
    }
    FORCEINLINE std::size_t IncreaseCaretCursor() noexcept
    {
        if (this->CaretCursor < std::numeric_limits<std::size_t>::max())
        {
            this->SetCaretCursor(this->CaretCursor + 1);
        }
        return this->CaretCursor;
    }
    std::size_t MoveCaretTo(LVec2F Location);

protected:

    virtual void OnChangedImpl() noexcept override;

private:

    bool UserInterfaceTick();

    inline void OnTextCommit(ETextCommit Commit)
    {
        (void)this->OnContentCommitted.InvokeIfBound(*this, this->GetContent(), Commit);
    }

    bool bBeamed{};
    f32 CaretBlinker{};
    std::size_t CaretCursor{};
    LDelegateHandle UserInterfaceTickDelegateHandle;
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryEditableTextButtonIconizedLeft)
class WEditableTextButtonIconizedLeft : public WEditableTextButton, public LIconized
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WEditableTextButtonIconizedLeft)

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;

    // TODO: Also add decoupled icon; Extra class?

private:

    NODISCARD LVec2F GetIconTopLeft(LVec2F Translation) const noexcept
    {
        return this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation)
            + IconLocation.GetRelativeLeftIconTopLeft
                <&WEditableTextButtonIconizedLeft::Icon, &WEditableTextButtonIconizedLeft::IconBrush>(*this, this->Brush.Padding);
    }
};

struct LFactoryEditableTextButton : NODE_FACTORY_PARENT(WEditableTextButton)
{
    NODE_FACTORY_BODY(WEditableTextButton)

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnAllowCommit, OnAllowCommit)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnContentCommitted, OnContentCommitted)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnContentChanged, OnContentChanged)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(ContentPredicate, ContentPredicate)

    decltype(auto) PlaceholderContent(this auto&& Self, LString Content) noexcept
    {
        NODE_FACTORY_SELF().PlaceholderContent = std::move(Content);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PlaceHolderTint(this auto&& Self, LColor const& Tint) noexcept
    {
        NODE_FACTORY_SELF().PlaceholderTint = Tint;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretBrush(this auto&& Self, LCaretBrush const& Brush) noexcept
    {
        NODE_FACTORY_SELF().CaretBrush = Brush;
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) CaretTint(this auto&& Self, LColor const& Tint) noexcept
    {
        NODE_FACTORY_SELF().CaretBrush.Tint = Tint;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretSize(this auto&& Self, LVec2F const& Size) noexcept
    {
        NODE_FACTORY_SELF().CaretBrush.Size = Size;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretHOffset(this auto&& Self, f32 Offset) noexcept
    {
        NODE_FACTORY_SELF().CaretBrush.HOffset = Offset;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretBlinkerSpeed(this auto&& Self, f32 Speed) noexcept
    {
        NODE_FACTORY_SELF().CaretBrush.CaretBlinkerSpeed = Speed;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
