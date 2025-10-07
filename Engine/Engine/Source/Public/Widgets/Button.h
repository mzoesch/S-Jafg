// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Region.h"
#include "Widgets/TextBox.h"
#include "Widgets/ButtonForward.h"
#include "Button.generated.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryTextButton : public TWidgetFactoryButton<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactoryButton)

    FORCEINLINE TFactoryRetTy& Content(const LString& InContent) { this->This()->SetContent(InContent); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Content(LString&& InContent) { this->This()->SetContent(std::move(InContent)); return this->Self(); }

    FORCEINLINE TFactoryRetTy& Enabled() { this->This()->SetEnabled(true); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Disabled() { this->This()->SetEnabled(false); return this->Self(); }

    FORCEINLINE TFactoryRetTy& TextBlockBrush(const LTextBoxBrush& InBrush) { this->This()->SetTextBoxBrush(InBrush); return this->Self(); }
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryButton)
class ENGINE_API WButton : public WRegion, public LButtonBase
{
    GENERATED_CLASS_BODY()

protected:

    explicit WButton(const LObjectInitializer& ObjectInitializer);

public:

    virtual void Construct() override;
    virtual LCursorReply OnCursorEnter() override;
    virtual LCursorReply OnCursorLeave() override;
    virtual LReply OnKeyDown(const LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyUp(const LViewport& InViewport, const LKeyEvent& InKeyEvent) override;

    LOnButtonKeyEvent OnPrimaryPressDelegate;
    LOnButtonKeyEvent OnPrimaryReleaseDelegate;
    LOnButtonKeyEvent OnSecondaryPressDelegate;
    LOnButtonKeyEvent OnSecondaryReleaseDelegate;

    void SetEnabled(const bool bInEnabled) override;
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryTextButton)
class ENGINE_API WTextButton : public WButton
{
    GENERATED_CLASS_BODY()

protected:

    explicit WTextButton(const ::Jafg::LObjectInitializer& ObjectInitializer);

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
    FORCEINLINE auto GetButtonTextChecked() noexcept -> WTextBox* { check( this->ButtonText ); return this->ButtonText; }
    FORCEINLINE auto GetButtonTextChecked() const noexcept -> const WTextBox* { check( this->ButtonText ); return this->ButtonText; }
    FORCEINLINE auto GetButtonTextAsserted() noexcept -> WTextBox* { jassert( this->ButtonText ); return this->ButtonText; }
    FORCEINLINE auto GetButtonTextAsserted() const noexcept -> const WTextBox* { jassert( this->ButtonText ); return this->ButtonText; }

protected:

    WTextBox* ButtonText { nullptr };

private:

    LString IntermediateContent;
    TOptional<LTextBoxBrush> IntermediateTextBoxBrush;
};

} /* ~Namespace Jafg */
