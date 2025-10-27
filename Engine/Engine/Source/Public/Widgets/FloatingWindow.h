// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "Widgets/VRegion.h"
#include "TextBox.h"
#include "FloatingWindow.generated.h"

namespace Jafg
{

class WTextBox;
class WVRegion;
class WFloatingWindow;

MAKE_DELEGATE_SIGNATURE(OnFloatingWindowClosedSignature, bool, WFloatingWindow& /* InWindow */)

DECLARE_JAFG_WIDGET()
class WFloatingWindow : public WUserWidget
{
    GENERATED_CLASS_BODY(ENGINE_API)

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WFloatingWindow)

public:

    virtual void Construct() override;

    FORCEINLINE WVRegion* GetWindow() noexcept { return this->GetRoot<WVRegion>(); }
    FORCEINLINE WVRegion const* GetWindow() const noexcept { return this->GetRoot<WVRegion>(); }

    FORCEINLINE void SetWindowSize(LVector2 SizeInSpt) noexcept
    {
        this->GetWindow()->SetMinDesiredSize(
        {
              EWidgetSize::StaticPoints
            , Maths::Max(SizeInSpt.X, WFloatingWindow::MinWindowSizeInSpt.X)
            , Maths::Max(SizeInSpt.Y, WFloatingWindow::MinWindowSizeInSpt.Y)
        });
    }
    FORCEINLINE void SetWindowPosition(LVector2 PositionInSpt) noexcept
    {
        this->GetWindow()->SetMargin({EWidgetSize::StaticPoints, PositionInSpt, 0, 0});
    }

    ENGINE_API void SetContentNode(WNode& Content) noexcept;

    FORCEINLINE bool CreateResizeUi() const noexcept { return this->bCreateResizeUi; }
    FORCEINLINE void SetCreateResizeUi(const bool bInCreateResizeUi) noexcept { this->bCreateResizeUi = bInCreateResizeUi; }

    //#
    //# Event called when the window is closed.
    //#
    //# Return true, if the event was handled. Nothing will be further done.
    //# Return false, if the event was not handled. The default behavior will be executed.
    //#
    //# The default behavior is to destroy the window.
    //#
    OnFloatingWindowClosedSignature OnWindowClosed;

    FORCEINLINE void SetWindowTitle(LString Title) noexcept { this->GetMutableWindowTitle().SetContent(std::move(Title)); }
    FORCEINLINE WTextBox const& GetWindowTitle() const noexcept { check( this->WindowTitle != nullptr ) return *this->WindowTitle; }
    FORCEINLINE WTextBox& GetMutableWindowTitle() noexcept { check( this->WindowTitle != nullptr ) return *this->WindowTitle; }

protected:

    LDelegateHandle UiTickMoveHandle{ nullptr };
    void UiTickMove(LViewport const& Viewport);
    TOptional<LVector2> MoveDragOffset;

    bool bCreateResizeUi{ true };
    LDelegateHandle UiTickResizeHandle{ nullptr };
    void UiTickResize(LViewport const& Viewport);
    TOptional<LVector2> ResizeDragOffset;

private:

    WTextBox* WindowTitle{ nullptr };
    static constexpr LVector2 MinWindowSizeInSpt{ 320, 180 };
};

} /* ~Namespace Jafg */
