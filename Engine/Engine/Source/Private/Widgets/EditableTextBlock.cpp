// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/EditableTextBlock.h"

Jafg::LSimpleString Jafg::LexToString(const ETextCommit::Type InType)
{
    switch (InType)
    {
    case ETextCommit::OnEnter:   { return "OnEnter"; }
    case ETextCommit::OnCleared: { return "OnCleared"; }
    case ETextCommit::FocusLost: { return "FocusLost"; }
    default:                     { checkNoEntry() return { }; }
    }
}

Jafg::WEditableTextBlock::WEditableTextBlock(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->SetVisibility(EWidgetVisibility::Visible);
    this->SetAnchor(EAnchor::Fill);
    return;
}

Jafg::LCursorReply Jafg::WEditableTextBlock::OnCursorEnter()
{
    return { EMouseCursor::Beam };
}

Jafg::LCursorReply Jafg::WEditableTextBlock::OnCursorLeave()
{
    return { EMouseCursor::Default };
}

void Jafg::WEditableTextBlock::OnFocusReceived()
{
    Super::OnFocusReceived();
    LOG_WARNING(LogTemporal, "Focus received.")
}

void Jafg::WEditableTextBlock::OnFocusLost()
{
    Super::OnFocusLost();
    LOG_WARNING(LogTemporal, "Focus lost.")
}

void Jafg::WEditableTextBlock::OnTextCommit(const LSimpleString& InText, const ETextCommit::Type InCommitType)
{
    LOG_WARNING(LogTemporal, "{}: {}", LexToString(InCommitType), InText)
}
