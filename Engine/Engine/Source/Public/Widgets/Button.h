// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Region.h"
#include "Widgets/TextBlock.h"
#include "Button.generated.h"

namespace Jafg
{

class WButton;
class WTextButton;
class WTextBlock;
struct LTextBlockBrush;

MAKE_DELEGATE_SIGNATURE(LOnButtonKeyEvent, void, WButton* Self, LKeyEvent& InKeyEvent)

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

    using Super         = TWidgetFactoryButton<TNode>;
    using TFactoryRetTy = typename Super::TFactoryRetTy;

    FORCEINLINE TFactoryRetTy& Content(const LString& InContent) { this->This()->SetContent(InContent); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Content(LString&& InContent) { this->This()->SetContent(std::move(InContent)); return this->Self(); }

    FORCEINLINE TFactoryRetTy& TextBlockBrush(const LTextBlockBrush& InBrush) { this->This()->SetTextBlockBrush(InBrush); return this->Self(); }
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
    virtual LReply OnKeyDown(LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyUp(LKeyEvent& InKeyEvent) override;

    LOnButtonKeyEvent OnPrimaryPressDelegate;
    LOnButtonKeyEvent OnPrimaryReleaseDelegate;
    LOnButtonKeyEvent OnSecondaryPressDelegate;
    LOnButtonKeyEvent OnSecondaryReleaseDelegate;

    //# These virtual methods are called when the associated delegate is not bound.
    virtual void OnPrimaryPress()     { }
    virtual void OnPrimaryRelease()   { }
    virtual void OnSecondaryPress()   { }
    virtual void OnSecondaryRelease() { }

    FORCEINLINE void SetNormalBrush(const LRegionBrush& InBrush) { this->NormalBrush = InBrush; }
    FORCEINLINE auto GetNormalBrush() const -> const LRegionBrush& { return this->NormalBrush; }
    FORCEINLINE void SetHoverBrush(const LRegionBrush& InBrush) { this->HoverBrush = InBrush; }
    FORCEINLINE auto GetHoverBrush() const -> const LRegionBrush& { return this->HoverBrush; }
    FORCEINLINE void SetPressBrush(const LRegionBrush& InBrush) { this->PressBrush = InBrush; }
    FORCEINLINE auto GetPressBrush() const -> const LRegionBrush& { return this->PressBrush; }

    FORCEINLINE void SetLetUiReactToEvents(const bool bInLetUiReactToEvents) { this->bLetUiReactToEvents = bInLetUiReactToEvents; }
    FORCEINLINE auto GetLetUiReactToEvents() const -> bool { return this->bLetUiReactToEvents; }

protected:

    bool bLetUiReactToEvents = true;
    LRegionBrush NormalBrush = { LColor::Black };
    LRegionBrush HoverBrush  = { LColor::Gray };
    LRegionBrush PressBrush  = { LColor::White };
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryTextButton)
class ENGINE_API WTextButton : public WButton
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WTextButton)

public:

    virtual void Construct() override;
    virtual void Draw(LViewport& Context) const override;

    //#
    //# @return True if the content was set to the #ButtonText widget.
    //#         False if it was stored in the #IntermediateContent.
    bool SetContent(const LString& InContent);
    bool SetContent(LString&& InContent);

    //# Same logic as with #SetContent.
    bool SetTextBlockBrush(const LTextBlockBrush& InBrush);
    bool ResetTextBlockBrush() { this->IntermediateTextBlockBrush.Reset(); return true; }

    //#
    //# @return True if the content (if available) was set to the #ButtonText widget.
    //#         False if the #IntermediateContent was empty or the ButtonText widget was not available.
    //#
    bool LoadIntermediateContent();

    FORCEINLINE bool IsButtonTextValid() const { return this->ButtonText != nullptr; }
    FORCEINLINE auto GetButtonText() -> WTextBlock* { return this->ButtonText; }
    FORCEINLINE auto GetButtonText() const -> const WTextBlock* { return this->ButtonText; }
    FORCEINLINE auto GetButtonTextChecked() -> WTextBlock* { check( this->ButtonText ); return this->ButtonText; }
    FORCEINLINE auto GetButtonTextChecked() const -> const WTextBlock* { check( this->ButtonText ); return this->ButtonText; }
    FORCEINLINE auto GetButtonTextAsserted() -> WTextBlock* { jassert( this->ButtonText ); return this->ButtonText; }
    FORCEINLINE auto GetButtonTextAsserted() const -> const WTextBlock* { jassert( this->ButtonText ); return this->ButtonText; }

protected:

    WTextBlock* ButtonText = nullptr;

private:

    LString IntermediateContent;
    TOptional<LTextBlockBrush> IntermediateTextBlockBrush;
};

} /* ~Namespace Jafg */
