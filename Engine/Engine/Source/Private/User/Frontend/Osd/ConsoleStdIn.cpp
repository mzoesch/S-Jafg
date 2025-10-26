// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/ConsoleStdIn.h"
#include "Widgets/EditableTextBox.h"

void Jafg::WConsoleStdIn::Construct()
{
    NewNode(WEditableTextBox).SaveTo(&this->EditableTextBox)
        .Anchor(EAnchor::VBottom | EAnchor::HFill)
        .Padding({2_spt, 4})
        .TrailingParent(this);

    Super::Construct();

    return;
}
