// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/EditableTextBlock.h"

Jafg::WEditableTextBlock::WEditableTextBlock(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->SetVisibility(EWidgetVisibility::Visible);
    this->SetAnchor(EAnchor::Fill);
    return;
}

Jafg::LCursorReply Jafg::WEditableTextBlock::OnCursorEnter()
{
    LOG_WARNING(LogTemporal, "Called")
    return Super::OnCursorEnter();
}

Jafg::LCursorReply Jafg::WEditableTextBlock::OnCursorLeave()
{
    LOG_WARNING(LogTemporal, "Called")
    return Super::OnCursorLeave();
}
