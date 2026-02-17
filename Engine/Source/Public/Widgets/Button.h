// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Region.h"
#include "Widgets/TextBox.h"
#include "Widgets/ButtonForward.h"
#include "Button.generated.h"

namespace Jafg
{

class WButton;
class WTextButton;
struct LFactoryTextButton;
typedef TFunction<void(WButton* Self, LKeyEvent const& InKeyEvent)>  LOnButtonKeyEvent;
typedef TFactoryButtonBase<WButton> LFactoryButton;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryButton)
class WButton : public WRegion, public LButtonBase
{
    GENERATED_CLASS_BODY()

protected:

    explicit WButton(LNodeDynamicInit const& Init) noexcept : Super{Init}
    {
        this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);
    }
    template<typename TCxxClass>
    explicit WButton(TNodeStaticInit<TCxxClass> const& Init) noexcept : Super{Init}
    {
        this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);
    }

public:

    virtual void Construct() override;
    virtual LCursorReply OnCursorEnter() override;
    virtual LCursorReply OnCursorLeave() override;
    virtual LReply OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyUp(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;

    LOnButtonKeyEvent OnPrimaryPressDelegate;
    LOnButtonKeyEvent OnPrimaryReleaseDelegate;
    LOnButtonKeyEvent OnSecondaryPressDelegate;
    LOnButtonKeyEvent OnSecondaryReleaseDelegate;

    void SetEnabled(const bool bInEnabled) override;
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTextButton)
class WTextButton : public WButton
{
    GENERATED_CLASS_BODY()

protected:

    explicit WTextButton(LNodeDynamicInit const& Init) noexcept : Super{Init}
    {
        this->SetPadding({4,2});
    }
    template<typename TCxxClass>
    explicit WTextButton(TNodeStaticInit<TCxxClass> const& Init) noexcept : Super{Init}
    {
        this->SetPadding({4,2});
    }

public:

    virtual void Construct() override;
    virtual void Draw(LViewport& Context) const override;

    //#
    //# @return True if the content was set to the #ButtonText widget.
    //#         False if it was stored in the #IntermediateContent.
    bool SetContent(const LString& InContent);
    bool SetContent(LString&& InContent);

    //# Same logic as with #SetContent.
    bool SetTextBoxBrush(const LTextBoxBrush& InBrush);
    bool ResetTextBoxBrush() { this->IntermediateTextBoxBrush.reset(); return true; }

    //#
    //# @return True if the content (if available) was set to the #ButtonText widget.
    //#         False if the #IntermediateContent was empty or the ButtonText widget was not available.
    //#
    bool LoadIntermediateContent();

    FORCEINLINE bool IsButtonTextValid() const noexcept { return this->ButtonText != nullptr; }
    FORCEINLINE auto GetButtonText() noexcept -> WTextBox* { return this->ButtonText; }
    FORCEINLINE auto GetButtonText() const noexcept -> const WTextBox* { return this->ButtonText; }
    FORCEINLINE auto GetButtonTextChecked() noexcept -> WTextBox* { check(this->ButtonText) return this->ButtonText; }
    FORCEINLINE auto GetButtonTextChecked() const noexcept -> const WTextBox* { check(this->ButtonText) return this->ButtonText; }
    FORCEINLINE auto GetButtonTextAsserted() noexcept -> WTextBox* { jassert(this->ButtonText) return this->ButtonText; }
    FORCEINLINE auto GetButtonTextAsserted() const noexcept -> const WTextBox* { jassert(this->ButtonText) return this->ButtonText; }

protected:

    WTextBox* ButtonText{};

private:

    LString IntermediateContent;
    TOptional<LTextBoxBrush> IntermediateTextBoxBrush;
};

struct LFactoryTextButton : NODE_FACTORY_PARENT(WTextButton)
{
    NODE_FACTORY_BODY(WTextButton)

    decltype(auto) Content(this auto&& Self, LString Content) noexcept
    {
        NODE_FACTORY_SELF().SetContent(std::move(Content));
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) TextBlockBrush(this auto&& Self, LTextBoxBrush const& Brush)
    {
        NODE_FACTORY_SELF().SetTextBoxBrush(Brush);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
