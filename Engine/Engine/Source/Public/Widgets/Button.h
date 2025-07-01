// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Region.h"
#include "Widgets/TextBox.h"
#include "Button.generated.h"

namespace Jafg
{

class WButton;
class WTextButton;
class WTextBox;
struct LTextBoxBrush;

MAKE_DELEGATE_SIGNATURE(LOnButtonKeyEvent, void, WButton* Self, const LKeyEvent& InKeyEvent)

struct LButtonStyle
{
    LRegionBrush NormalBrush   { .Type = ERegionBrush::OutlineBox, .Tint = LColor::Gray,          .OutlineTint = LColor::Black };
    LRegionBrush HoverBrush    { .Type = ERegionBrush::OutlineBox, .Tint = LColor::Gray,          .OutlineTint = LColor::White };
    LRegionBrush PressBrush    { .Type = ERegionBrush::OutlineBox, .Tint = LColor::Gray,          .OutlineTint = LColor::White };
    LRegionBrush DisabledBrush { .Type = ERegionBrush::OutlineBox, .Tint = LColor::NotSoDarkGray, .OutlineTint = LColor::Black };
};

template <typename TNode>
class TWidgetFactoryButton : public TWidgetFactoryRegion<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactoryRegion)

    FORCEINLINE TFactoryRetTy& OnPrimaryPress(LOnButtonKeyEvent&& InDelegate) { this->This()->OnPrimaryPressDelegate = std::move(InDelegate); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OnPrimaryRelease(LOnButtonKeyEvent&& InDelegate) { this->This()->OnPrimaryReleaseDelegate = std::move(InDelegate); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OnSecondaryPress(LOnButtonKeyEvent&& InDelegate) { this->This()->OnSecondaryPressDelegate = std::move(InDelegate); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OnSecondaryRelease(LOnButtonKeyEvent&& InDelegate) { this->This()->OnSecondaryReleaseDelegate = std::move(InDelegate); return this->Self(); }

    FORCEINLINE TFactoryRetTy& NormalBrush(const LRegionBrush& InBrush) { this->This()->SetNormalBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& HoverBrush(const LRegionBrush& InBrush) { this->This()->SetHoverBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& PressBrush(const LRegionBrush& InBrush) { this->This()->SetPressBrush(InBrush); return this->Self(); }

    FORCEINLINE TFactoryRetTy& UiResponsive(const bool bTrue) { this->This()->SetLetUiReactToEvents(bTrue); return this->Self(); }
};

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
class ENGINE_API WButton : public WRegion
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

    //# These virtual methods are called when the associated delegate is not bound.
    virtual void OnPrimaryPress()     { }
    virtual void OnPrimaryRelease()   { }
    virtual void OnSecondaryPress()   { }
    virtual void OnSecondaryRelease() { }

    FORCEINLINE void SetNormalBrush(const LRegionBrush& InBrush) noexcept { this->Style.NormalBrush = InBrush; }
    FORCEINLINE auto GetNormalBrush() const noexcept -> const LRegionBrush& { return this->Style.NormalBrush; }
    FORCEINLINE void SetHoverBrush(const LRegionBrush& InBrush) noexcept { this->Style.HoverBrush = InBrush; }
    FORCEINLINE auto GetHoverBrush() const noexcept -> const LRegionBrush& { return this->Style.HoverBrush; }
    FORCEINLINE void SetPressBrush(const LRegionBrush& InBrush) noexcept { this->Style.PressBrush = InBrush; }
    FORCEINLINE auto GetPressBrush() const noexcept -> const LRegionBrush& { return this->Style.PressBrush; }
    FORCEINLINE void SetDisabledBrush(const LRegionBrush& InBrush) noexcept { this->Style.DisabledBrush = InBrush; }
    FORCEINLINE auto GetDisabledBrush() const noexcept -> const LRegionBrush& { return this->Style.DisabledBrush; }

    FORCEINLINE void SetLetUiReactToEvents(const bool bInLetUiReactToEvents) { this->bLetUiReactToEvents = bInLetUiReactToEvents; }
    FORCEINLINE bool GetLetUiReactToEvents() const { return this->bLetUiReactToEvents; }

    void SetEnabled(const bool bInEnabled);
    FORCEINLINE bool IsEnabled() const noexcept { return this->bEnabled; }

protected:

    bool bLetUiReactToEvents : 1 { true };
    bool bEnabled : 1 { true };

    LButtonStyle Style;
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
    bool ResetTextBoxBrush() { this->IntermediateTextBoxBrush.Reset(); return true; }

    //#
    //# @return True if the content (if available) was set to the #ButtonText widget.
    //#         False if the #IntermediateContent was empty or the ButtonText widget was not available.
    //#
    bool LoadIntermediateContent();

    FORCEINLINE bool IsButtonTextValid() const { return this->ButtonText != nullptr; }
    FORCEINLINE auto GetButtonText() -> WTextBox* { return this->ButtonText; }
    FORCEINLINE auto GetButtonText() const -> const WTextBox* { return this->ButtonText; }
    FORCEINLINE auto GetButtonTextChecked() -> WTextBox* { check( this->ButtonText ); return this->ButtonText; }
    FORCEINLINE auto GetButtonTextChecked() const -> const WTextBox* { check( this->ButtonText ); return this->ButtonText; }
    FORCEINLINE auto GetButtonTextAsserted() -> WTextBox* { jassert( this->ButtonText ); return this->ButtonText; }
    FORCEINLINE auto GetButtonTextAsserted() const -> const WTextBox* { jassert( this->ButtonText ); return this->ButtonText; }

protected:

    WTextBox* ButtonText { nullptr };

private:

    LString IntermediateContent;
    TOptional<LTextBoxBrush> IntermediateTextBoxBrush;
};

} /* ~Namespace Jafg */
