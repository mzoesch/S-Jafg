// Copyright mzoesch. All rights reserved.

#include "User/Frontend/ConsoleStdIn.h"
#include "Widgets/EditableTextBox.h"
#include "Widgets/VRegion.h"
#include "Widgets/ScrollRegion.h"
#include "Cli/CliCommand.h"
#include "Cli/CliStatics.h"
#include "Cli/CommandLineInterface.h"

void Jafg::WConsoleStdIn::Construct()
{
    NewNode(WEditableTextBox).SaveTo(&this->EditableTextBox)
        .Anchor(EAnchor::VBottom | EAnchor::HFill)
        .Padding({2_spt, 4})
        .OnCommit(LEditableTextBoxCommitDelegate::Create(this, &WConsoleStdIn::OnCommit))
        .OnChanged(LEditableTextBoxChangedDelegate::Create(this, &WConsoleStdIn::OnChanged))
        .TrailingParent(this);

    NewNode(WVRegion).SaveTo(&this->IntellisenseContainer)
        .Anchor(EAnchor::VBottom)
        .Padding({0_spt, 0, 0, 30})
    [
            NewNode(WScrollRegion)
                .MinDesiredSize({60_spt, 50})
                .UseChildrenDesiredSize(true)
            [
                NewNode(WVRegion).SaveTo(&this->IntellisensePredictions)
                    .Anchor(EAnchor::VBottom)
                    .MinDesiredSize({60_spt, 0})
                    .Padding(2_spt)
                    .Type(ERegionBrush::OutlineBox)
                    .Tint(this->GetCDR()->IntellisenseTint)
                    .OutlineTint(Colors::Black)
            ]
            +
            NewNode(WRegion).SaveTo(&this->IntellisenseHelpContainer)
                .Padding(2_spt)
                .Type(ERegionBrush::OutlineBox)
                .Tint(this->GetCDR()->IntellisenseTint)
                .OutlineTint(Colors::Black)
            [
                NewNode(WTextBox).SaveTo(&this->IntellisenseHelp)
                    .Padding(3_spt)
                    .Padding({2_spt, 4})
                    .TextScale(0.25)
                    .TextColor(Colors::White)
            ]
            +
            NewNode(WVRegion).SaveTo(&this->Intellisense)
                .Padding(3_spt)
                .Type(ERegionBrush::OutlineBox)
                .Tint(this->GetCDR()->IntellisenseTint)
                .OutlineTint(Colors::Black)
            [
                NewNode(WTextBox).SaveTo(&this->IntellisenseText)
                    .Brush(LTextBoxBrush::Body())
            ]
    ]
        .TrailingParent(this);

    Super::Construct();

    this->IntellisenseContainer->SetVisibility(EWidgetVisibility::Collapsed);

    this->IntellisensePredictions->SetVisibility(EWidgetVisibility::Collapsed);
    this->IntellisenseHelpContainer->SetVisibility(EWidgetVisibility::Collapsed);
    this->Intellisense->SetVisibility(EWidgetVisibility::Collapsed);

    return;
}

Jafg::LReply Jafg::WConsoleStdIn::OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    check( this->EditableTextBox )
    check( this->IntellisenseContainer )

    if (this->IntellisenseContainer->IsPainted() && this->IntellisensePredictions->IsPainted())
    {
        if (InKeyEvent.GetKey() == EKeys::Up)
        {
            if (this->TryGoIntellisensePredictionUp())
            {
                return LReply::Handled();
            }
        }
        else if (InKeyEvent.GetKey() == EKeys::Down)
        {
            if (this->TryGoIntellisensePredictionDown())
            {
                return LReply::Handled();
            }
        }
        else if (InKeyEvent.GetKey() == EKeys::Tab)
        {
            if (this->IsCurrentSelectedIntellisensePredictionValid())
            {
                this->ApplyCurrentIntellisensePrediction();
                return LReply::Handled();
            }
        }
    }
    else
    {
        if (InKeyEvent.GetKey() == EKeys::Up)
        {
            this->GoHistoryBack();
            return LReply::Handled();
        }
        if (InKeyEvent.GetKey() == EKeys::Down)
        {
            this->GoHistoryForward();
            return LReply::Handled();
        }
    }

    return Super::OnKeyDown(InViewport, InKeyEvent);
}

void Jafg::WConsoleStdIn::AddToHistory(LString const& Text)
{
    check( Text.empty() == false )

    TArray<LString>& CdrHistory = GetMutableCDR()->History;

    if (CdrHistory.empty() == false)
    {
        if (CdrHistory.back() == Text)
        {
            return;
        }
    }

    CdrHistory.emplace_back(Text);

    if (CdrHistory.size() > GetMutableCDR()->MaxHistorySize)
    {
        CdrHistory.erase(CdrHistory.begin());
    }

    return;
}

void Jafg::WConsoleStdIn::OnCommit(LString const& Text, ETextCommit::Type CommitType)
{
    if (CommitType == ETextCommit::OnCleared)
    {
        if (this->IntellisenseContainer->IsPainted())
        {
            this->IntellisenseContainer->SetVisibility(EWidgetVisibility::Collapsed);
            return;
        }
    }

    if (Text.empty() && this->bIgnoreEmptyEnterCommits && CommitType == ETextCommit::OnEnter)
    {
        return;
    }

    if (CommitType != ETextCommit::OnEnter && this->bIgnoreNonEnterCommits)
    {
        return;
    }

    if (Text.empty() == false)
    {
        this->AddToHistory(Text);
    }

    if (this->OnTextCommit.IsValid() && this->OnTextCommit.Invoke(Text, CommitType))
    {
        return;
    }

    if (CommitType == ETextCommit::OnEnter)
    {
        this->bIgnoreOnChangedCall = true;
        this->EditableTextBox->EmptyContent();
        this->bIgnoreOnChangedCall = false;

        this->IntellisensePredictions->RemoveChildren();
        this->Intellisense->RemoveChildren();
        this->IntellisensePredictions->SetVisibility(EWidgetVisibility::Collapsed);
        this->Intellisense->SetVisibility(EWidgetVisibility::Collapsed);
        this->UpdateIntellisense();
    }

    return;
}

void Jafg::WConsoleStdIn::OnChanged(LString const& Text)
{
    if (this->bIgnoreOnChangedCall)
    {
        return;
    }

    LCommandLineInterface const& Cli{ this->GetCommandLineInterface() };

    this->IntellisensePredictions->RemoveChildren();
    this->Intellisense->RemoveChildren();

    if (Text.empty())
    {
        i32 AddedCommands{ 0 };

        for (auto const& Command : Cli.GetCommands())
        {
            if (AddedCommands >= static_cast<i32>(GetCDR()->MaxIntellisensePredictions))
            {
                break;
            }

            ++AddedCommands;
            this->AddIntellisensePrediction(Command.GetIdentifier());

            continue;
        }

        if (AddedCommands)
        {
            this->IntellisensePredictions->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
        }
        else
        {
            this->IntellisensePredictions->SetVisibility(EWidgetVisibility::Collapsed);
        }

        this->UpdateIntellisense();
        return;
    }

    const LString CommandStr{ CliStatics::GetCommandFromText(Text) };
    if (Text.find(' ') == LString::npos)
    {
        TArray<LCliCommand const*> Predictions;

        for (auto const& Command : Cli.GetCommands())
        {
            if (Predictions.size() >= GetCDR()->MaxIntellisensePredictions)
            {
                break;
            }

            if (Command.GetIdentifier().starts_with(CommandStr))
            {
                Predictions.emplace_back(&Command);
            }

            continue;
        }

        LCliCommand const* Command{ nullptr };

        if (Predictions.empty())
        {
            this->IntellisensePredictions->SetVisibility(EWidgetVisibility::Collapsed);
        }
        else
        {
            if (Predictions.size() == 1 && Predictions[0]->GetIdentifier() == CommandStr)
            {
                this->IntellisensePredictions->SetVisibility(EWidgetVisibility::Collapsed);
                Command = Cli.GetCommandChecked(CommandStr);
            }
            else
            {
                this->IntellisensePredictions->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
                for (LCliCommand const* Prediction : Predictions)
                {
                    this->AddIntellisensePrediction(Prediction->GetIdentifier());
                    continue;
                }
            }
        }

        this->UpdateIntellisense(Command);
    }
    else if (LCliCommand const* Command{ Cli.GetCommand(CommandStr) }; Command)
    {
        check( this->IntellisensePredictions->GetChildren().empty() )

        if
        (
            TArray Suggestions{ Cli.GetCommonSuggestions(Text, this->GetCDR()->MaxIntellisensePredictions) };
            Suggestions.empty())
        {
            this->IntellisensePredictions->SetVisibility(EWidgetVisibility::Collapsed);
        }
        else
        {
            this->IntellisensePredictions->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
            for (LString& Suggestion : Suggestions)
            {
                check( Suggestions.empty() == false )
                this->AddIntellisensePrediction(std::move(Suggestion));
                check( Suggestion.empty() )
                continue;
            }
        }

        this->UpdateIntellisense(Command);
    }
    else
    {
        check( this->IntellisensePredictions->GetChildren().empty() )
        this->IntellisensePredictions->SetVisibility(EWidgetVisibility::Collapsed);
        this->UpdateIntellisense();
    }

    return;
}

void Jafg::WConsoleStdIn::AddIntellisensePrediction(LString Text)
{
    NewNode(WTextBox)
        .Anchor(EAnchor::HFill)
        .Padding({2_spt, 4})
        .Content(std::move(Text))
        .TextScale(0.25)
        .TextColor(Colors::White)
        .Tint(this->GetCDR()->IntellisenseHighlightTint)
        .FinishWithParentAt(0, this->IntellisensePredictions);

    return;
}

void Jafg::WConsoleStdIn::AddIntellisense(LString Text)
{
    NewNode(WTextBox)
        .Anchor(EAnchor::HFill)
        .Padding({2_spt, 4})
        .Content(std::move(Text))
        .TextScale(0.25)
        .TextColor(Colors::White)
        .Tint(this->GetCDR()->IntellisenseHighlightTint)
        .FinishWithParentAt(0, this->Intellisense);

    return;
}

void Jafg::WConsoleStdIn::UpdateIntellisense(LCliCommand const* TargetCommand /* = nullptr */)
{
    if (this->IntellisensePredictions->IsPainted())
    {
        check( this->IntellisensePredictions->GetChildren().empty() == false)

        if (this->IsCurrentSelectedIntellisensePredictionValid() == false)
        {
            WTextBox const* ChildText{ this->IntellisensePredictions->GetChildren().back()->Content->AsStatic<WTextBox>() };
            this->CurrentIntellisensePrediction = ChildText->GetContent();
            check( this->IsCurrentSelectedIntellisensePredictionValid() )
        }

        for (LWidgetSlot const* Child : this->IntellisensePredictions->GetChildren())
        {
            if
            (
                WTextBox* TextBlock{ Child->Content->AsStatic<WTextBox>() };
                TextBlock->GetContent() == this->CurrentIntellisensePrediction
            )
            {
                TextBlock->SetType(ERegionBrush::Box);
            }
            else
            {
                TextBlock->SetType(ERegionBrush::None);
            }

            continue;
        }
    }
    else
    {
        check( this->IntellisensePredictions->GetChildren().empty() )
        algo::orphan(&this->CurrentIntellisensePrediction);
    }

    LCliCommand const* Command{ TargetCommand };
    if (Command == nullptr)
    {
        if
        (
            algo::find(
                  this->IntellisensePredictions->GetChildren()
                , this->CurrentIntellisensePrediction
                , [](auto const& E){ return E->Content->template AsStatic<WTextBox>()->GetContent(); }
                ) != this->IntellisensePredictions->GetChildren().end()
        )
        {
            Command = this->GetCommandLineInterface().GetCommandChecked(this->CurrentIntellisensePrediction);
        }
    }

    if (Command)
    {
        for (LCommandParams const& Overload : Command->GetOverloads())
        {
            if (LString Repr{ Overload.GetCatRepresentation() }; Repr.empty() == false)
            {
                this->AddIntellisense(std::move(Repr));
            }
        }
        if (this->Intellisense->GetChildren().empty())
        {
            this->Intellisense->SetVisibility(EWidgetVisibility::Collapsed);
        }
        else
        {
            this->Intellisense->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
        }
        if (Command->GetHelp().empty())
        {
            this->IntellisenseHelpContainer->SetVisibility(EWidgetVisibility::Collapsed);
        }
        else
        {
            this->IntellisenseHelp->SetContent(Command->GetHelp());
            this->IntellisenseHelpContainer->SetVisibility(EWidgetVisibility::TransitiveHitTestInvisible);
        }
    }
    else
    {
        this->IntellisenseHelpContainer->SetVisibility(EWidgetVisibility::Collapsed);
        this->Intellisense->SetVisibility(EWidgetVisibility::Collapsed);
    }

    if (this->Intellisense->IsPainted() || this->IntellisensePredictions->IsPainted())
    {
        this->IntellisenseContainer->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
    }
    else
    {
        check( this->IntellisenseHelpContainer->IsPainted() == false )
        this->IntellisenseContainer->SetVisibility(EWidgetVisibility::Collapsed);
    }

    return;
}

bool Jafg::WConsoleStdIn::TryGoIntellisensePredictionUp()
{
    if
    (
        auto It{ algo::find(
              this->IntellisensePredictions->GetChildren()
            , this->CurrentIntellisensePrediction
            , [](auto const& E){ return E->Content->template AsStatic<WTextBox>()->GetContent(); }
            )};
        It != this->IntellisensePredictions->GetChildren().end()
    )
    {
        if (It != this->IntellisensePredictions->GetChildren().begin())
        {
            It = algo::prev(It);

            this->CurrentIntellisensePrediction = (*It)->Content->AsStatic<WTextBox>()->GetContent();
            this->OnChanged(this->EditableTextBox->GetContent());

            return true;
        }
    }

    return false;
}

bool Jafg::WConsoleStdIn::TryGoIntellisensePredictionDown()
{
    if
    (
        auto It{ algo::find(
              this->IntellisensePredictions->GetChildren()
            , this->CurrentIntellisensePrediction
            , [](auto const& E){ return E->Content->template AsStatic<WTextBox>()->GetContent(); }
            )};
        It != this->IntellisensePredictions->GetChildren().end()
    )
    {
        It = algo::next(It);
        if (It != this->IntellisensePredictions->GetChildren().end())
        {
            this->CurrentIntellisensePrediction = (*It)->Content->AsStatic<WTextBox>()->GetContent();
            this->OnChanged(this->EditableTextBox->GetContent());

            return true;
        }
    }

    return false;
}

void Jafg::WConsoleStdIn::ApplyCurrentIntellisensePrediction()
{
    check( this->IsCurrentSelectedIntellisensePredictionValid() )

    if (auto Space{ this->EditableTextBox->GetContent().rfind(' ') }; Space == algo::npos)
    {
        if (this->bPrefixCommand)
        {
            this->EditableTextBox->SetContent(Jafg::SprintF("/{}", this->CurrentIntellisensePrediction));
        }
        else
        {
            this->EditableTextBox->SetContent(Jafg::SprintF("{}", this->CurrentIntellisensePrediction));
        }
    }
    else
    {
        LString Temp{ this->EditableTextBox->GetContent().substr(0, Space + 1) };
        Temp.append(this->CurrentIntellisensePrediction);

        this->EditableTextBox->SetContent(std::move(Temp));
    }

    this->EditableTextBox->SetCaretCursorToEnd();

    this->OnChanged(this->EditableTextBox->GetContent());

    return;
}

void Jafg::WConsoleStdIn::GoHistoryBack()
{
    check( this->EditableTextBox )

    const i32 LastHistoryCursor{ this->HistoryCursor };
    TArray<LString> const& CdrHistory = this->GetCDR()->History;
    this->HistoryCursor = maths::clamp(this->HistoryCursor + 1, static_cast<i32>(INDEX_NONE), static_cast<i32>(CdrHistory.size()) - 1);

    if (this->HistoryCursor == INDEX_NONE || this->HistoryCursor == LastHistoryCursor)
    {
        return;
    }

    if (const LString Item{ *this->GetCurrentHistoryItemChecked() }; this->EditableTextBox->GetContent() != Item)
    {
        this->bIgnoreOnChangedCall = true;
        this->EditableTextBox->SetContent(Item);
        this->bIgnoreOnChangedCall = false;

        this->EditableTextBox->SetCaretCursorToEnd();
    }

    return;
}

void Jafg::WConsoleStdIn::GoHistoryForward()
{
    check( this->EditableTextBox )

    const i32 LastHistoryCursor{ this->HistoryCursor };
    this->HistoryCursor = maths::clamp(this->HistoryCursor - 1, static_cast<i32>(INDEX_NONE), static_cast<i32>(this->GetCDR()->History->size()) - 1);

    if (this->HistoryCursor == INDEX_NONE || this->HistoryCursor == LastHistoryCursor)
    {
        if (this->HistoryCursor != LastHistoryCursor)
        {
            this->bIgnoreOnChangedCall = true;
            this->EditableTextBox->SetContent({});
            this->bIgnoreOnChangedCall = false;

            this->EditableTextBox->SetCaretCursorToEnd();
        }

        return;
    }

    this->bIgnoreOnChangedCall = true;
    this->EditableTextBox->SetContent(*this->GetCurrentHistoryItemChecked());
    this->bIgnoreOnChangedCall = false;

    this->EditableTextBox->SetCaretCursorToEnd();

    return;
}

bool Jafg::WConsoleStdIn::IsCurrentSelectedIntellisensePredictionValid() const
{
    for (const LWidgetSlot* Child : this->IntellisensePredictions->GetChildren())
    {
        if
        (
            const WTextBox* TextBlock { Child->Content->AsStatic<WTextBox>() };
            TextBlock->GetContent() == this->CurrentIntellisensePrediction
        )
        {
            return true;
        }
    }

    return false;
}
