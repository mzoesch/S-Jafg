// Copyright mzoesch. All rights reserved.

#include "Nodes/EditableTextButton.h"
#include "User/LocalEgo.h"
#include "Engine/Engine.h"
#include "Rhi/NodeRenderInfo.h"
#include "Framework/FontSubsystem.h"
#include "User/UserPreferences.h"

void Jafg::WEditableTextButton::Draw(LNodeRenderInfo const& Info) const
{
    LVec2F CachedTextDrawOffset{this->TextDrawOffset};
    LVec2F CachedTextThrust{this->TextThrust};

    LVec2F TopLeftMostOuter{this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation)};

    if (this->IsFocusWidget() && !(this->GetTextRenderData().Collection.GlyphInfos.empty() || this->TextBrush.bSkipBrushDraw))
    {
        LGlyphCollection::Info& Back{this->GetTextRenderData().Collection.GlyphInfos.back()};
        LVec2F LastTopRight{this->GetRelativeTextTopLeft() + Back.Pencil + LVec2F{Back.Rect.z, 0.0f}};
        LVec2F Extent{this->GetAnchoredSize_v2()
            - this->Brush.Padding.GetDesiredSize().InStaticPoints(this->GetViewport())
            - this->TextDrawOffset
            - this->TextPlayroomReduction
            };

        LVec2F Overdraw{LastTopRight - Extent};
        if (Overdraw.x > 0.0f)
        {
            this->TextDrawOffset.x -= Overdraw.x;
            this->TextThrust.x -= Overdraw.x;
        }
    }

    Super::Draw(Info);

    if (this->GetContent().empty() && !(this->PlaceholderContent.empty() || this->TextBrush.bSkipBrushDraw))
    {
        this->GetTextRenderData().Render(Info,
            {
                .Offset = TopLeftMostOuter + this->GetRelativeTextTopLeft(),
                .Extent = this->GetAnchoredSize_v2()
                    - this->Brush.Padding.GetDesiredSize().InStaticPoints(this->GetViewport())
                    - this->TextDrawOffset
                    - this->TextPlayroomReduction
            },
            {
                .Cutoff = this->TextCutoff,
                .Tint = this->PlaceholderTint,
                .OutlineTint = this->TextBrush.OutlineTint,
                .OutlineThickness = this->TextBrush.OutlineThickness,
            }
            );
    }

    if (this->CaretBlinker < this->CaretBrush.CaretBlinkerSpeed && this->IsFocusWidget())
    {
        LVec2F CaretSize{maths::round(LVec2F{2.0, this->GetTextRenderData().DesiredSize.y} * this->CaretBrush.Size)};
        LVec2F CaretTopLeft{
              this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation)
            + LVec2F{this->Brush.Padding.GetLeftOffset().InStaticPoints(this->GetViewport()), 0.0}
            + LVec2F{0.0, (this->GetDesiredSize_v2().y - CaretSize.y) * 0.5}
            + this->TextDrawOffset
            };

        LStringView CaretContent;
        if (this->CaretCursor < this->GetContent().size())
        {
            CaretContent = algo::left_sub<LString, LStringView>(this->GetContent(), this->CaretCursor);
        }
        else
        {
            CaretContent = this->GetContent();
        }
        if (!CaretContent.empty())
        {
            auto& Collection{this->GetTextRenderData().Collection};
            auto Glyph{Collection.end_string(CaretContent)};
            if (Glyph != Collection.end())
            {
                if (Glyph->Cluster < CaretContent.size())
                {
                    for (auto Length{algo::utf8_char_length(CaretContent[Glyph->Cluster])-1}; Length > 0; --Length)
                    {
                        if (Glyph != Collection.end())
                        {
                            ++Glyph;
                        }
                    }
                }
                else
                {
                    check(Glyph->Cluster == CaretContent.size())
                }
            }
            if (Glyph == Collection.end())
            {
                CaretTopLeft.x += Collection.PencilEnd.x;
            }
            else
            {
                CaretTopLeft.x += Glyph->Pencil.x;
            }
        }

        Info.AddInstance({.Rect={maths::round(CaretTopLeft), CaretSize}, .Tint=this->CaretBrush.Tint});
    }

    this->TextDrawOffset = CachedTextDrawOffset;
    this->TextThrust = CachedTextThrust;

    return;
}

void Jafg::WEditableTextButton::UpdateDesiredSize() const
{
    /* TODO: Not optimal. Can we cache this? */
    if (this->GetContent().empty() && !this->PlaceholderContent.empty())
    {
        this->GetTextRenderData().Update(this->GetViewport()
            , *this->GetFrontend().GetSubsystemChecked<JFontSubsystem>()
            , this->TextBrush
            , this->PlaceholderContent
            );
    }

    Super::UpdateDesiredSize();

    return;
}

bool Jafg::WEditableTextButton::UserInterfaceTick()
{
    bool bHandled{};

    if (this->GetViewport().GetSurface().HasBufferedPlatformInput())
    {
        LString BufferedInput{this->GetViewport().GetSurface().ConsumeBufferedPlatformInput()};
        check(!BufferedInput.empty())
        LString NewContent{this->GetContent()};
        NewContent.insert(this->CaretCursor, BufferedInput);

        if (this->ContentPredicate.IsValid() && !this->ContentPredicate.Invoke(NewContent))
        {
            LOG_VERBOSE(LogWidgets, "Content predicate failed for [{}]. Discarding content change request.", NewContent)
        }
        else
        {
            bHandled = true;

            auto Writer{this->GetMutableTextContent()};
            Writer.first = std::move(NewContent);
            for (auto Idx{0uz}; Idx < BufferedInput.size(); ++Idx)
            {
                this->IncreaseCaretCursor();
            }
            this->OnContentChanged.InvokeIfBound(*this, this->GetContent());
        }
    }

    if (!bHandled)
    {
        this->CaretBlinker += GEngine->DeltaTime;
        if (this->CaretBlinker > this->CaretBrush.CaretBlinkerSpeed * 2.0f)
        {
            this->CaretBlinker = 0.0f;
        }
    }

    return {};
}

Jafg::LNodeReply Jafg::WEditableTextButton::OnCursorEnter()
{
    if (auto Reply{Super::OnCursorEnter()}; Reply.IsHandled())
    {
        if (Reply.GetCursor() != ECursor::None)
        {
            return Reply;
        }
        this->bBeamed = true;
        return {ECursor::Beam};
    }
    return {};
}

void Jafg::WEditableTextButton::OnCursorLeave()
{
    if (this->bBeamed)
    {
        this->bBeamed = false;
        this->GetViewport().GetSurface()._SetMouseCursor(ECursor::Default);
    }

    Super::OnCursorLeave();
    return;
}

void Jafg::WEditableTextButton::OnFocusReceived()
{
    Super::OnFocusReceived();
    this->CaretBlinker = 0.0f;

    check(!this->UserInterfaceTickDelegateHandle)
    this->UserInterfaceTickDelegateHandle = this->GetViewport().OnLateTick.Emplace(this, &WEditableTextButton::UserInterfaceTick);

    if (this->GetViewport().GetSurface().HasMouseLocationForOrtho())
    {
        this->MoveCaretTo(this->GetViewport().GetSurface().GetMouseLocationValue());
    }
    else
    {
        this->SetCaretCursorToEnd();
    }

    return;
}

void Jafg::WEditableTextButton::OnFocusLost()
{
    Super::OnFocusLost();

    if (this->UserInterfaceTickDelegateHandle.IsValid())
    {
        this->GetViewport().OnLateTick.Remove(&this->UserInterfaceTickDelegateHandle);
    }
    this->OnTextCommit(ETextCommit::FocusLost);

    return;
}

Jafg::LNodeReply Jafg::WEditableTextButton::OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    if (auto Reply{Super::OnKeyEventFocused(Info, Event)}; Reply.IsHandled())
    {
        return Reply;
    }

    if (!this->bEnabled)
    {
        return LNodeReply::Unhandled();
    }

    if (Event.Is<ERawInputStateBits::Press|ERawInputStateBits::Repeat>(Info.Frontend.GetPhysicalKey(ELogicalKey::BackSpace))) // || PlatformDelete?
    {
        if (!this->GetContent().empty() && this->CaretCursor > 0)
        {
            auto RemoveNum{algo::utf8_char_length(this->GetContent()[this->CaretCursor - 1]) };
            auto Writer{this->GetMutableTextContent()};
            Writer.first.erase(
                Writer.first.begin() + (this->CaretCursor - 1),
                Writer.first.begin() + (this->CaretCursor - 1 + RemoveNum)
                );
            for (auto Idx{0uz}; Idx < RemoveNum; ++Idx)
            {
                this->ReduceCaretCursor();
            }
            this->OnContentChanged.InvokeIfBound(*this, this->GetContent());
        }

        this->CaretBlinker = 0.0f;
        return LNodeReply::Handled();
    }

    if (Event.Is<ERawInputStateBits::Press|ERawInputStateBits::Repeat>(Info.Frontend.GetPhysicalKey(ELogicalKey::Left)))
    {
        if (algo::valid_index(this->GetContent(), this->CaretCursor - 1))
        {
            auto Size{algo::utf8_char_length(this->GetContent()[this->CaretCursor])};
            for (auto Idx{0uz}; Idx < Size; ++Idx)
            {
                this->ReduceCaretCursor();
            }
        }

        this->CaretBlinker = 0.0f;
        return LNodeReply::Handled();
    }

    if (Event.Is<ERawInputStateBits::Press|ERawInputStateBits::Repeat>(Info.Frontend.GetPhysicalKey(ELogicalKey::Right)))
    {
        if (algo::valid_index(this->GetContent(), this->CaretCursor))
        {
            auto Size{algo::utf8_char_length(this->GetContent()[this->CaretCursor])};
            for (auto Idx{0uz}; Idx < Size; ++Idx)
            {
                this->IncreaseCaretCursor();
            }
        }

        this->CaretBlinker = 0.0f;
        return LNodeReply::Handled();
    }

    if (Event.Is<ERawInputStateBits::Press>(
          Info.Frontend.GetPhysicalKey(ELogicalKey::Enter)
        , Info.Frontend.GetPhysicalKey(ELogicalKey::NumPadEnter)))
    {
        if (this->OnAllowContentCommit.IsValid())
        {
            if (!this->OnAllowContentCommit.Invoke(*this))
            {
                return LNodeReply::Unhandled();
            }
        }

        this->OnTextCommit(ETextCommit::OnEnter);
        return Super::OnKeyEventFocused(Info, Event);
    }

    if (Event.Is<ERawInputStateBits::Press>(Info.Frontend.GetPhysicalKey(ELogicalKey::LeftMouseButton)))
    {
        if (Info.Surface.HasMouseLocationForOrtho())
        {
            this->MoveCaretTo(Info.Surface.GetMouseLocationValue());
        }
        return LNodeReply::Handled();
    }

    if (Event.Is<ERawInputStateBits::Press>(Info.Frontend.GetPhysicalKey(ELogicalKey::Escape)))
    {
        if (this->GetViewport().GetFocusedWidget() == this)
        {
            this->OnTextCommit(ETextCommit::OnCleared);
            return LNodeReply{TClassStorage<WNode>{}};
        }
    }

    return LNodeReply::Unhandled();
}

void Jafg::WEditableTextButton::OnChangedImpl() noexcept
{
    this->CaretBlinker = 0.0f;
    this->CaretCursor = maths::min(this->CaretCursor, this->GetContent().size());

    Super::OnChangedImpl();

    return;
}

std::size_t Jafg::WEditableTextButton::MoveCaretTo(LVec2F Location)
{
    /*
     * This is a workaround and bugprone. We are using the widget location data from the last frame.
     * Meaning that things could have changed in the meantime... It is maybe safer to update the caret in the
     * draw method of this widget instead? But who gives a shit right now? The user would need to click / tap keys
     * in the same frame. I do not think that my users will have the brainpower to actually operate computers fast
     * and right.
     */
    const f32 BaseTopLeft
    {
        this->GetAnchoredTopLeftFromMostOuter().x + this->TextDrawOffset.x// + this->CaretBrush.HOffset
    };

    f32 RelativeTopLeft{Location.x - BaseTopLeft};
    if (RelativeTopLeft < 0.0)
    {
        this->SetCaretCursorToBegin();
    }
    else if (!this->GetContent().empty())
    {
        auto& Collection{this->GetTextRenderData().Collection};
        if (auto It{Collection.end_location({RelativeTopLeft, Location.y})}; It == Collection.end())
        {
            this->SetCaretCursorToEnd();
        }
        else
        {
            this->SetCaretCursor(It->Cluster);
        }
    }
    else
    {
        this->SetCaretCursorToEnd();
    }

    return this->CaretCursor;
}

void Jafg::WEditableTextButtonIconizedLeft::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);
    if (this->Icon.get())
    {
        Detail::DrawIcon(Info, this->GetIconTopLeft(Info.Translation), this->Icon, this->IconBrush);
    }
    return;
}

void Jafg::WEditableTextButtonIconizedLeft::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();
    auto Size{this->GetIconSize(this->GetViewport())};
    this->SetDesiredSizeInSpt(this->GetDesiredSize_v2() + Size);
    this->TextDrawOffset = Size;
    return;
}
