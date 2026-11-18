// Copyright mzoesch. All rights reserved.

#pragma once

#include "JgcMinimal.afx"
#include "Nodes/UserWidget.h"
#include "JgcStatusHud.generated.h"

namespace Jafg
{

class WText;
class WVParent;
class LLocalLackey;

} /* ~Namespace Jafg */

namespace Jgc
{

//#
//# A heads-up display in the bottom right of the view: A list of rows, each a label and a value (e.g., "Jetpack ON").
//# It lives in the viewport over the rendered world of a local lackey (see #Jafg::LLocalLackey::GetWorldOverlayViewport).
//#
//# Owners keep it in a slot (see #Show and #Hide). The viewport may be destroyed before the owner or the other way
//# around, therefore each side unlinks itself before it goes away: The HUD nulls the slot when it is destroyed.
//#
DECLARE_JAFG_WIDGET()
class JGC_API WStatusHud : public Jafg::WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WStatusHud)

public:

    virtual ~WStatusHud() override;

    virtual void Construct() override;

    //# @return The index of the new row.
    std::size_t AddRow(LString Label);
    void SetRow(std::size_t Row, LStringView Value, LColor const& Tint) noexcept;
    //# Green ON or gray OFF.
    void SetToggle(std::size_t Row, bool bEngaged) noexcept;

    //# Creates a HUD in the view of #Lackey and puts it into #Slot. Whatever was in #Slot is hidden first.
    static WStatusHud& Show(Jafg::LLocalLackey& Lackey, WStatusHud** Slot);
    //# Destroys the HUD in #Slot, if any, and empties it.
    static void Hide(WStatusHud** Slot) noexcept;

private:

    Jafg::WVParent* List{};
    TArray<LString> Labels;
    TArray<Jafg::WText*> Values;
    WStatusHud** OwnerSlot{};
};

} /* ~Namespace Jgc */
